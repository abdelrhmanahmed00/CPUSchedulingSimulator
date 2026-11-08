#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "core/Process.h"
#include "core/ScheduleResult.h"

class Sidebar;
class StatCard;
class GanttChartPanel;
class QStackedWidget;
class QTableWidget;
class QComboBox;
class QPushButton;
class QLabel;

// Top-level application window: owns the sidebar navigation, the four
// pages (Overview/Simulator/Algorithms/About), and all the wiring between
// the process input table, the selected SchedulingAlgorithm, and the
// result-displaying widgets (stat cards, Gantt chart, results table).
//
// This class only collects input and displays ScheduleResult — it never
// reimplements scheduling logic itself.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRunSimulation();
    void onResetClicked();
    void onAddProcessClicked();

private:
    QWidget *buildSimulatorPage();
    QWidget *buildOverviewPage();
    QWidget *buildAlgorithmsPage();
    QWidget *buildAboutPage();

    void seedDefaultProcesses();
    void refreshProcessTable();
    void editProcessByPid(int pid);
    void deleteProcessByPid(int pid);
    void applyResult(const ScheduleResult &result);
    void clearResults();

    std::vector<Process> m_processes;

    Sidebar *m_sidebar = nullptr;
    QStackedWidget *m_stack = nullptr;

    QComboBox *m_algorithmCombo = nullptr;
    QPushButton *m_runButton = nullptr;
    QPushButton *m_resetButton = nullptr;

    StatCard *m_totalProcessesCard = nullptr;
    StatCard *m_avgWaitingCard = nullptr;
    StatCard *m_avgTurnaroundCard = nullptr;
    StatCard *m_totalExecutionCard = nullptr;

    QTableWidget *m_processTable = nullptr;
    QTableWidget *m_resultsTable = nullptr;
    GanttChartPanel *m_ganttPanel = nullptr;
};
#endif // MAINWINDOW_H
