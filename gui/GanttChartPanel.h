#ifndef GANTTCHARTPANEL_H
#define GANTTCHARTPANEL_H

#include <QFrame>
#include <vector>

#include "core/GanttSegment.h"

class GanttChartWidget;
class QPushButton;
class QWidget;
class QHBoxLayout;

// The dark "glass" card hosting the Gantt chart: header with Play/Pause/
// Reset/speed controls, a horizontally scrollable canvas, and a legend
// mapping each process id to its color.
class GanttChartPanel : public QFrame
{
    Q_OBJECT

public:
    explicit GanttChartPanel(QWidget *parent = nullptr);

    void setSegments(const std::vector<GanttSegment> &segments, bool autoPlay = true);
    void clear();

private:
    void rebuildLegend(const std::vector<GanttSegment> &segments);
    void updateControlStates();

    GanttChartWidget *m_chart = nullptr;
    QPushButton *m_playButton = nullptr;
    QPushButton *m_pauseButton = nullptr;
    QPushButton *m_resetButton = nullptr;
    QHBoxLayout *m_legendLayout = nullptr;
    QWidget *m_legendRow = nullptr;
};

#endif // GANTTCHARTPANEL_H
