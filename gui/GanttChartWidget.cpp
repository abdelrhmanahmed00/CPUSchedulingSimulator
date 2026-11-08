#include "GanttChartWidget.h"

#include <QFontMetrics>
#include <QPainter>
#include <QTimer>
#include <algorithm>
#include <set>

#include "Theme.h"

namespace {
constexpr int kLeftMargin = 24;
constexpr int kTopMargin = 38;
constexpr int kRowHeight = 60;
constexpr int kBottomLabelHeight = 24;
constexpr double kPixelsPerUnit = 34.0;
constexpr int kTickMs = 30;
constexpr double kBaselineSeconds = 4.0; // wall-clock time for a 1x-speed full playback
}

GanttChartWidget::GanttChartWidget(QWidget *parent)
    : QWidget(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(kTickMs);
    connect(m_timer, &QTimer::timeout, this, &GanttChartWidget::onTick);
    setMinimumHeight(kTopMargin + kRowHeight + kBottomLabelHeight + 16);
}

void GanttChartWidget::rebuildColorMap()
{
    std::set<int> pids;
    for (const GanttSegment &seg : m_segments) {
        if (seg.pid != -1)
            pids.insert(seg.pid);
    }
    m_colorForPid.clear();
    int index = 0;
    for (int pid : pids) {
        m_colorForPid.insert(pid, Theme::colorForIndex(index));
        ++index;
    }
}

void GanttChartWidget::setSegments(const std::vector<GanttSegment> &segments)
{
    m_timer->stop();
    m_playing = false;
    m_segments = segments;
    m_visibleTime = 0.0;
    m_totalDuration = 0.0;
    for (const GanttSegment &seg : m_segments)
        m_totalDuration = std::max(m_totalDuration, static_cast<double>(seg.endTime));

    rebuildColorMap();

    const QSize hint = sizeHint();
    setMinimumWidth(hint.width());
    resize(hint);
    updateGeometry();
    update();
}

void GanttChartWidget::clear()
{
    setSegments({});
}

int GanttChartWidget::contentWidth() const
{
    return kLeftMargin * 2 + static_cast<int>(m_totalDuration * kPixelsPerUnit);
}

QSize GanttChartWidget::sizeHint() const
{
    return QSize(std::max(contentWidth(), 320), kTopMargin + kRowHeight + kBottomLabelHeight + 16);
}

QSize GanttChartWidget::minimumSizeHint() const
{
    return sizeHint();
}

void GanttChartWidget::play()
{
    if (m_segments.empty())
        return;
    if (m_visibleTime >= m_totalDuration)
        m_visibleTime = 0.0;
    m_playing = true;
    m_timer->start();
    emit playStateChanged(true);
}

void GanttChartWidget::pause()
{
    m_playing = false;
    m_timer->stop();
    emit playStateChanged(false);
}

void GanttChartWidget::resetAnimation()
{
    m_timer->stop();
    m_playing = false;
    m_visibleTime = 0.0;
    emit playStateChanged(false);
    update();
}

void GanttChartWidget::setSpeedMultiplier(double multiplier)
{
    m_speed = multiplier;
}

void GanttChartWidget::onTick()
{
    if (m_totalDuration <= 0.0) {
        pause();
        return;
    }

    const double ticksForBaseline = (kBaselineSeconds * 1000.0) / kTickMs;
    const double stepPerTick = (m_totalDuration / ticksForBaseline) * m_speed;

    m_visibleTime += stepPerTick;
    if (m_visibleTime >= m_totalDuration) {
        m_visibleTime = m_totalDuration;
        pause();
        emit animationFinished();
    }
    update();
}

void GanttChartWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Theme::GanttBackground);

    if (m_segments.empty()) {
        painter.setPen(QColor(255, 255, 255, 110));
        QFont f = painter.font();
        f.setPointSize(11);
        painter.setFont(f);
        painter.drawText(rect(), Qt::AlignCenter,
                          QStringLiteral("Run a simulation to see the execution timeline"));
        return;
    }

    const int rowY = kTopMargin + 8;

    // Ruler baseline.
    painter.setPen(QPen(QColor(255, 255, 255, 50), 1));
    const int rulerRight = kLeftMargin + static_cast<int>(m_totalDuration * kPixelsPerUnit);
    painter.drawLine(kLeftMargin, kTopMargin, rulerRight, kTopMargin);

    // Collect tick boundaries (segment starts/ends) and draw ticks + labels.
    std::set<int> boundaries;
    for (const GanttSegment &seg : m_segments) {
        boundaries.insert(seg.startTime);
        boundaries.insert(seg.endTime);
    }

    QFont tickFont = painter.font();
    tickFont.setPointSize(8);
    painter.setFont(tickFont);

    // One label per unique boundary time, revealed progressively as the
    // animation passes it. Drawing it here (rather than per-segment) avoids
    // doubling the label where one segment's end meets the next one's start.
    for (int t : boundaries) {
        const int x = kLeftMargin + static_cast<int>(t * kPixelsPerUnit);
        painter.setPen(QPen(QColor(255, 255, 255, 70), 1));
        painter.drawLine(x, kTopMargin - 4, x, kTopMargin + 4);

        if (t <= m_visibleTime) {
            painter.setPen(QColor(255, 255, 255, 150));
            painter.drawText(x - 18, kTopMargin - 18, 36, 14, Qt::AlignHCenter, QString::number(t));
        }
    }

    // Segments, progressively revealed up to m_visibleTime.
    QFont labelFont = painter.font();
    labelFont.setPointSize(10);
    labelFont.setBold(true);

    for (const GanttSegment &seg : m_segments) {
        if (seg.startTime >= m_visibleTime)
            continue;

        const double visibleEnd = std::min<double>(seg.endTime, m_visibleTime);
        if (visibleEnd <= seg.startTime)
            continue;

        const int x = kLeftMargin + static_cast<int>(seg.startTime * kPixelsPerUnit);
        const int revealedWidth = std::max(1, static_cast<int>((visibleEnd - seg.startTime) * kPixelsPerUnit));

        const QRect revealedRect(x, rowY, revealedWidth, kRowHeight);
        const bool isIdle = (seg.pid == -1);
        const QColor color = isIdle ? QColor("#2E2F4A") : m_colorForPid.value(seg.pid, Theme::PrimaryPurple);

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawRoundedRect(revealedRect, 8, 8);

        if (isIdle) {
            painter.save();
            painter.setClipRect(revealedRect);
            QPen hatchPen(QColor(255, 255, 255, 16), 6);
            painter.setPen(hatchPen);
            for (int xx = revealedRect.left() - kRowHeight; xx < revealedRect.right(); xx += 12)
                painter.drawLine(xx, revealedRect.bottom(), xx + kRowHeight, revealedRect.top());
            painter.restore();
        }

        // Process/idle label, drawn once enough width has been revealed to fit it.
        const QString label = isIdle ? QStringLiteral("Idle") : QStringLiteral("P%1").arg(seg.pid);
        painter.setFont(labelFont);
        const int textWidth = QFontMetrics(labelFont).horizontalAdvance(label);
        if (revealedWidth - 14 >= textWidth) {
            painter.setPen(isIdle ? QColor(255, 255, 255, 140) : QColor(255, 255, 255, 240));
            painter.drawText(revealedRect, Qt::AlignCenter, label);
        }
    }

    // Playhead.
    if (m_visibleTime > 0.0 && m_visibleTime < m_totalDuration) {
        const int x = kLeftMargin + static_cast<int>(m_visibleTime * kPixelsPerUnit);
        painter.setPen(QPen(Theme::CyanAccent, 2));
        painter.drawLine(x, kTopMargin - 8, x, rowY + kRowHeight + 16);
    }
}
