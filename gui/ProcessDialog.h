#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <vector>

#include "core/Process.h"

class QSpinBox;
class QLabel;

// Modal dialog for adding or editing a single Process. Validates that the
// arrival time is non-negative, burst time is positive, and the pid is
// unique among the other existing processes.
class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    // existingProcesses: all processes currently in the table, used for the
    // uniqueness check. editingPid: the pid of the process being edited (so
    // it doesn't conflict with itself), or -1 when adding a new process.
    explicit ProcessDialog(const std::vector<Process> &existingProcesses,
                            int editingPid = -1,
                            QWidget *parent = nullptr);

    void setInitialValues(const Process &process);
    Process resultProcess() const;

private slots:
    void handleAccept();

private:
    std::vector<Process> m_existingProcesses;
    int m_editingPid;

    QSpinBox *m_pidSpin = nullptr;
    QSpinBox *m_arrivalSpin = nullptr;
    QSpinBox *m_burstSpin = nullptr;
    QLabel *m_errorLabel = nullptr;
};

#endif // PROCESSDIALOG_H
