#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QMap>
#include <vector>

#include "core/GanttSegment.h"

class QTimer;

// Paints the scheduling timeline as a single dark horizontal lane: block
// width is proportional to execution duration, each process id gets a
// stable distinct color, idle spans are drawn as a hatched neutral block,
// and a time ruler shows exact start/end boundaries. Supports a progressive
// left-to-right reveal animation driven by an internal QTimer.
class GanttChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GanttChartWidget(QWidget *parent = nullptr);

    void setSegments(const std::vector<GanttSegment> &segments);
    void clear();

    bool isPlaying() const { return m_playing; }
    bool hasContent() const { return !m_segments.empty(); }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void play();
    void pause();
    void resetAnimation();
    void setSpeedMultiplier(double multiplier);

signals:
    void playStateChanged(bool playing);
    void animationFinished();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onTick();

private:
    void rebuildColorMap();
    int contentWidth() const;

    std::vector<GanttSegment> m_segments;
    QMap<int, QColor> m_colorForPid;
    double m_totalDuration = 0.0;
    double m_visibleTime = 0.0;
    double m_speed = 1.0;
    bool m_playing = false;
    QTimer *m_timer = nullptr;
};

#endif // GANTTCHARTWIDGET_H
