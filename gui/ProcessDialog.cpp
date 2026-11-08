#include "ProcessDialog.h"

#include <QColor>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <algorithm>

namespace {
// A labeled input row: a small caption label stacked above the spin box,
// matching the dashboard's form conventions elsewhere.
QVBoxLayout *makeFieldRow(const QString &labelText, QSpinBox *field, QWidget *parent)
{
    auto *label = new QLabel(labelText, parent);
    label->setObjectName("fieldLabel");

    auto *rowLayout = new QVBoxLayout();
    rowLayout->setSpacing(6);
    rowLayout->addWidget(label);
    rowLayout->addWidget(field);
    return rowLayout;
}

// QSpinBox formats and parses its text using the widget's QLocale, which
// otherwise defaults to QLocale::system(). On a system set to a locale whose
// default numbering system is Eastern Arabic (e.g. ar_SA), that would render
// ٠١٢... instead of 0123... Forcing English (US) here keeps the digits
// Western Arabic regardless of the OS locale/keyboard, without touching any
// system-wide setting.
void useWesternArabicNumerals(QSpinBox *spinBox)
{
    QLocale locale(QLocale::English, QLocale::UnitedStates);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    spinBox->setLocale(locale);
}
}

ProcessDialog::ProcessDialog(const std::vector<Process> &existingProcesses, int editingPid, QWidget *parent)
    : QDialog(parent)
    , m_existingProcesses(existingProcesses)
    , m_editingPid(editingPid)
{
    setObjectName("processDialog");
    setWindowTitle(editingPid == -1 ? QStringLiteral("Add Process") : QStringLiteral("Edit Process"));
    setModal(true);
    setFixedWidth(380);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(18, 18, 18, 18);

    auto *glassCard = new QFrame(this);
    glassCard->setObjectName("glassDialogCard");

    auto *shadow = new QGraphicsDropShadowEffect(glassCard);
    shadow->setBlurRadius(36);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(101, 86, 232, 70));
    glassCard->setGraphicsEffect(shadow);

    outer->addWidget(glassCard);

    auto *layout = new QVBoxLayout(glassCard);
    layout->setContentsMargins(26, 24, 26, 22);
    layout->setSpacing(18);

    auto *titleBox = new QVBoxLayout();
    titleBox->setSpacing(3);
    auto *title = new QLabel(editingPid == -1 ? QStringLiteral("Add Process") : QStringLiteral("Edit Process"), glassCard);
    title->setObjectName("dialogTitle");
    auto *subtitle = new QLabel(editingPid == -1
                                     ? QStringLiteral("Define a new process for the schedule.")
                                     : QStringLiteral("Update this process's timing values."),
                                 glassCard);
    subtitle->setObjectName("dialogSubtitle");
    titleBox->addWidget(title);
    titleBox->addWidget(subtitle);
    layout->addLayout(titleBox);

    m_pidSpin = new QSpinBox(glassCard);
    m_pidSpin->setRange(1, 9999);
    m_pidSpin->setFixedHeight(38);
    useWesternArabicNumerals(m_pidSpin);

    int suggestedPid = 1;
    for (const Process &p : m_existingProcesses)
        suggestedPid = std::max(suggestedPid, p.pid + 1);
    m_pidSpin->setValue(editingPid == -1 ? suggestedPid : editingPid);
    m_pidSpin->setEnabled(editingPid == -1); // pid is fixed once a process exists, to keep results stable

    m_arrivalSpin = new QSpinBox(glassCard);
    m_arrivalSpin->setRange(0, 100000);
    m_arrivalSpin->setFixedHeight(38);
    useWesternArabicNumerals(m_arrivalSpin);

    m_burstSpin = new QSpinBox(glassCard);
    m_burstSpin->setRange(1, 100000);
    m_burstSpin->setValue(1);
    m_burstSpin->setFixedHeight(38);
    useWesternArabicNumerals(m_burstSpin);

    auto *fieldsLayout = new QVBoxLayout();
    fieldsLayout->setSpacing(14);
    fieldsLayout->addLayout(makeFieldRow(QStringLiteral("Process ID"), m_pidSpin, glassCard));
    fieldsLayout->addLayout(makeFieldRow(QStringLiteral("Arrival Time"), m_arrivalSpin, glassCard));
    fieldsLayout->addLayout(makeFieldRow(QStringLiteral("Burst Time"), m_burstSpin, glassCard));
    layout->addLayout(fieldsLayout);

    m_errorLabel = new QLabel(glassCard);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setVisible(false);
    layout->addWidget(m_errorLabel);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);

    auto *cancelButton = new QPushButton(QStringLiteral("Cancel"), glassCard);
    cancelButton->setObjectName("dialogCancelButton");
    cancelButton->setCursor(Qt::PointingHandCursor);

    auto *okButton = new QPushButton(editingPid == -1 ? QStringLiteral("Add Process") : QStringLiteral("Save Changes"), glassCard);
    okButton->setObjectName("dialogSaveButton");
    okButton->setCursor(Qt::PointingHandCursor);
    okButton->setDefault(true);

    buttonRow->addWidget(cancelButton);
    buttonRow->addWidget(okButton);
    layout->addLayout(buttonRow);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(okButton, &QPushButton::clicked, this, &ProcessDialog::handleAccept);
}

void ProcessDialog::setInitialValues(const Process &process)
{
    m_pidSpin->setValue(process.pid);
    m_arrivalSpin->setValue(process.arrivalTime);
    m_burstSpin->setValue(process.burstTime);
}

Process ProcessDialog::resultProcess() const
{
    Process p;
    p.pid = m_pidSpin->value();
    p.arrivalTime = m_arrivalSpin->value();
    p.burstTime = m_burstSpin->value();
    return p;
}

void ProcessDialog::handleAccept()
{
    const int pid = m_pidSpin->value();
    const int arrival = m_arrivalSpin->value();
    const int burst = m_burstSpin->value();

    if (arrival < 0) {
        m_errorLabel->setText(QStringLiteral("Arrival Time cannot be negative."));
        m_errorLabel->setVisible(true);
        return;
    }

    if (burst <= 0) {
        m_errorLabel->setText(QStringLiteral("Burst Time must be greater than zero."));
        m_errorLabel->setVisible(true);
        return;
    }

    for (const Process &p : m_existingProcesses) {
        if (p.pid == pid && pid != m_editingPid) {
            m_errorLabel->setText(QStringLiteral("Process ID %1 is already in use.").arg(pid));
            m_errorLabel->setVisible(true);
            return;
        }
    }

    accept();
}
