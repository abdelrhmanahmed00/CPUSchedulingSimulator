#include "GanttChartPanel.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <set>

#include "GanttChartWidget.h"
#include "Theme.h"

GanttChartPanel::GanttChartPanel(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("ganttCard");

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(22, 20, 22, 20);
    outer->setSpacing(14);

    // Header row: title/caption on the left, playback controls on the right.
    auto *headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    auto *titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);
    auto *title = new QLabel(QStringLiteral("Execution Timeline"), this);
    title->setObjectName("ganttTitle");
    auto *caption = new QLabel(QStringLiteral("Gantt chart of the computed schedule"), this);
    caption->setObjectName("ganttCaption");
    titleBox->addWidget(title);
    titleBox->addWidget(caption);

    headerRow->addLayout(titleBox);
    headerRow->addStretch(1);

    m_playButton = new QPushButton(QStringLiteral("▶  Play"), this);
    m_playButton->setObjectName("ganttControlButton");
    m_pauseButton = new QPushButton(QStringLiteral("⏸  Pause"), this);
    m_pauseButton->setObjectName("ganttControlButton");
    m_resetButton = new QPushButton(QStringLiteral("⟲  Reset"), this);
    m_resetButton->setObjectName("ganttControlButton");

    auto *speedCombo = new QComboBox(this);
    speedCombo->setObjectName("speedCombo");
    speedCombo->addItem(QStringLiteral("0.5x"), 0.5);
    speedCombo->addItem(QStringLiteral("1x"), 1.0);
    speedCombo->addItem(QStringLiteral("2x"), 2.0);
    speedCombo->addItem(QStringLiteral("4x"), 4.0);
    speedCombo->setCurrentIndex(1);

    headerRow->addWidget(m_playButton);
    headerRow->addWidget(m_pauseButton);
    headerRow->addWidget(m_resetButton);
    headerRow->addWidget(speedCombo);

    outer->addLayout(headerRow);

    // Scrollable canvas.
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("ganttScrollArea");
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_chart = new GanttChartWidget(scrollArea);
    scrollArea->setWidget(m_chart);
    scrollArea->setFixedHeight(140);

    outer->addWidget(scrollArea);

    // Legend row.
    m_legendRow = new QWidget(this);
    m_legendLayout = new QHBoxLayout(m_legendRow);
    m_legendLayout->setContentsMargins(0, 0, 0, 0);
    m_legendLayout->setSpacing(16);
    m_legendLayout->addStretch(1);
    outer->addWidget(m_legendRow);

    connect(m_playButton, &QPushButton::clicked, m_chart, &GanttChartWidget::play);
    connect(m_pauseButton, &QPushButton::clicked, m_chart, &GanttChartWidget::pause);
    connect(m_resetButton, &QPushButton::clicked, m_chart, &GanttChartWidget::resetAnimation);
    connect(speedCombo, &QComboBox::currentIndexChanged, this, [this, speedCombo](int) {
        m_chart->setSpeedMultiplier(speedCombo->currentData().toDouble());
    });
    connect(m_chart, &GanttChartWidget::playStateChanged, this, [this](bool) { updateControlStates(); });
    connect(m_chart, &GanttChartWidget::animationFinished, this, [this]() { updateControlStates(); });

    updateControlStates();
}

void GanttChartPanel::setSegments(const std::vector<GanttSegment> &segments, bool autoPlay)
{
    m_chart->setSegments(segments);
    rebuildLegend(segments);
    if (autoPlay && !segments.empty())
        m_chart->play();
    updateControlStates();
}

void GanttChartPanel::clear()
{
    m_chart->clear();
    rebuildLegend({});
    updateControlStates();
}

void GanttChartPanel::rebuildLegend(const std::vector<GanttSegment> &segments)
{
    QLayoutItem *item = nullptr;
    while ((item = m_legendLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    std::set<int> pids;
    bool hasIdle = false;
    for (const GanttSegment &seg : segments) {
        if (seg.pid == -1)
            hasIdle = true;
        else
            pids.insert(seg.pid);
    }

    int index = 0;
    for (int pid : pids) {
        auto *entry = new QWidget(m_legendRow);
        auto *entryLayout = new QHBoxLayout(entry);
        entryLayout->setContentsMargins(0, 0, 0, 0);
        entryLayout->setSpacing(6);

        auto *dot = new QLabel(entry);
        dot->setFixedSize(10, 10);
        dot->setStyleSheet(QStringLiteral("background: %1; border-radius: 5px;")
                                .arg(Theme::colorForIndex(index).name()));

        auto *label = new QLabel(QStringLiteral("P%1").arg(pid), entry);
        label->setStyleSheet(QStringLiteral("color: #C9C8DC; font-size: 11px; font-weight: 600;"));

        entryLayout->addWidget(dot);
        entryLayout->addWidget(label);
        m_legendLayout->addWidget(entry);
        ++index;
    }

    if (hasIdle) {
        auto *entry = new QWidget(m_legendRow);
        auto *entryLayout = new QHBoxLayout(entry);
        entryLayout->setContentsMargins(0, 0, 0, 0);
        entryLayout->setSpacing(6);

        auto *dot = new QLabel(entry);
        dot->setFixedSize(10, 10);
        dot->setStyleSheet(QStringLiteral("background: #2E2F4A; border: 1px solid rgba(255,255,255,60); border-radius: 5px;"));

        auto *label = new QLabel(QStringLiteral("Idle"), entry);
        label->setStyleSheet(QStringLiteral("color: #C9C8DC; font-size: 11px; font-weight: 600;"));

        entryLayout->addWidget(dot);
        entryLayout->addWidget(label);
        m_legendLayout->addWidget(entry);
    }

    m_legendLayout->addStretch(1);
}

void GanttChartPanel::updateControlStates()
{
    const bool hasContent = m_chart->hasContent();
    m_playButton->setEnabled(hasContent);
    m_pauseButton->setEnabled(hasContent);
    m_resetButton->setEnabled(hasContent);
}
