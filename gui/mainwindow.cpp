#include "mainwindow.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <memory>

#include "GanttChartPanel.h"
#include "ProcessDialog.h"
#include "Sidebar.h"
#include "StatCard.h"
#include "Theme.h"

#include "algorithms/FCFSScheduler.h"
#include "algorithms/SJFScheduler.h"
#include "algorithms/SRTFScheduler.h"

namespace {
QFrame *makeCard(QWidget *parent = nullptr)
{
    auto *frame = new QFrame(parent);
    frame->setObjectName("card");
    return frame;
}

QTableWidgetItem *makeCenteredItem(const QString &text)
{
    auto *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

void styleResultTable(QTableWidget *table)
{
    table->setObjectName("dataTable");
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setDefaultSectionSize(42);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

// Pins the table's height to exactly fit its header plus all of its rows, so
// it never needs an internal scrollbar and grows/shrinks as rows are
// added/removed. The surrounding page QScrollArea handles any overflow.
void resizeTableToContents(QTableWidget *table)
{
    int rowsHeight = 0;
    for (int row = 0; row < table->rowCount(); ++row)
        rowsHeight += table->rowHeight(row);

    const int headerHeight = table->horizontalHeader()->isVisible() ? table->horizontalHeader()->height() : 0;
    const int frameHeight = table->frameWidth() * 2;

    table->setFixedHeight(headerHeight + rowsHeight + frameHeight);
}

// Paints each row of the algorithm dropdown's popup: a soft lavender hover
// tint, a stronger purple tint + dot indicator for the currently-selected
// algorithm (drawn ourselves rather than relying on the platform's native
// checkmark glyph, which doesn't follow the app's stylesheet), and an
// unclipped single-line label so long algorithm names stay fully readable.
class AlgorithmItemDelegate : public QStyledItemDelegate
{
public:
    explicit AlgorithmItemDelegate(QComboBox *combo, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_combo(combo)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isCurrent = m_combo && index.row() == m_combo->currentIndex();
        const bool isHovered = option.state & QStyle::State_MouseOver;

        const QRect itemRect = option.rect.adjusted(6, 3, -6, -3);

        if (isCurrent) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(101, 86, 232, 32));
            painter->drawRoundedRect(itemRect, 10, 10);
        } else if (isHovered) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(101, 86, 232, 15));
            painter->drawRoundedRect(itemRect, 10, 10);
        }

        const QRect textRect = itemRect.adjusted(14, 0, isCurrent ? -26 : -14, 0);
        QFont font = painter->font();
        font.setPointSize(12);
        font.setBold(isCurrent);
        painter->setFont(font);
        painter->setPen(isCurrent ? QColor("#6556E8") : QColor("#24243A"));
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

        if (isCurrent) {
            constexpr int kDotSize = 7;
            const QRect dot(itemRect.right() - kDotSize - 10,
                             itemRect.center().y() - kDotSize / 2,
                             kDotSize, kDotSize);
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#6556E8"));
            painter->drawEllipse(dot);
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(40);
        return size;
    }

private:
    QComboBox *m_combo;
};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("CPU Scheduling Simulator"));
    resize(1360, 980);
    setMinimumSize(1040, 720);

    qApp->setStyleSheet(Theme::globalStyleSheet());

    auto *central = new QWidget(this);
    central->setObjectName("mainArea");
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_sidebar = new Sidebar(central);
    rootLayout->addWidget(m_sidebar);

    m_stack = new QStackedWidget(central);
    m_stack->addWidget(buildOverviewPage());
    m_stack->addWidget(buildSimulatorPage());
    m_stack->addWidget(buildAlgorithmsPage());
    m_stack->addWidget(buildAboutPage());
    m_stack->setCurrentIndex(1);
    rootLayout->addWidget(m_stack, 1);

    setCentralWidget(central);

    connect(m_sidebar, &Sidebar::pageSelected, m_stack, &QStackedWidget::setCurrentIndex);

    seedDefaultProcesses();
    refreshProcessTable();
}

MainWindow::~MainWindow() = default;

void MainWindow::seedDefaultProcesses()
{
    m_processes = {
        {1, 0, 8},
        {2, 1, 4},
        {3, 2, 9},
        {4, 3, 5},
    };
}

QWidget *MainWindow::buildSimulatorPage()
{
    auto *scrollArea = new QScrollArea();
    scrollArea->setObjectName("pageScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *content = new QWidget();
    content->setObjectName("pageContent");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(22);

    // --- Header -----------------------------------------------------
    auto *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    auto *titleBox = new QVBoxLayout();
    titleBox->setSpacing(4);
    auto *title = new QLabel(QStringLiteral("CPU Scheduling Simulator"));
    title->setObjectName("simulatorPageTitle");
    auto *subtitle = new QLabel(QStringLiteral("Visualize and compare FCFS, SJF, and SRTF scheduling in real time."));
    subtitle->setObjectName("pageSubtitle");
    titleBox->addWidget(title);
    titleBox->addWidget(subtitle);
    headerRow->addLayout(titleBox);
    headerRow->addStretch(1);

    m_algorithmCombo = new QComboBox();
    m_algorithmCombo->setObjectName("algorithmCombo");
    m_algorithmCombo->addItem(QStringLiteral("FCFS — First Come First Served"));
    m_algorithmCombo->addItem(QStringLiteral("SJF — Shortest Job First"));
    m_algorithmCombo->addItem(QStringLiteral("SRTF — Shortest Remaining Time First"));

    m_algorithmCombo->setView(new QListView(m_algorithmCombo));
    m_algorithmCombo->setItemDelegate(new AlgorithmItemDelegate(m_algorithmCombo, m_algorithmCombo));
    m_algorithmCombo->view()->setObjectName("algorithmComboPopup");
    m_algorithmCombo->view()->setTextElideMode(Qt::ElideNone);
    m_algorithmCombo->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    auto *popupShadow = new QGraphicsDropShadowEffect(m_algorithmCombo->view());
    popupShadow->setBlurRadius(32);
    popupShadow->setOffset(0, 8);
    popupShadow->setColor(QColor(101, 86, 232, 90));
    m_algorithmCombo->view()->setGraphicsEffect(popupShadow);

    m_runButton = new QPushButton(QStringLiteral("▶  Run Simulation"));
    m_runButton->setObjectName("primaryButton");
    m_runButton->setCursor(Qt::PointingHandCursor);

    m_resetButton = new QPushButton(QStringLiteral("Reset"));
    m_resetButton->setObjectName("secondaryButton");
    m_resetButton->setCursor(Qt::PointingHandCursor);

    headerRow->addWidget(m_algorithmCombo);
    headerRow->addWidget(m_runButton);
    headerRow->addWidget(m_resetButton);
    layout->addLayout(headerRow);

    // --- Stat cards ---------------------------------------------------
    auto *statsRow = new QHBoxLayout();
    statsRow->setSpacing(16);
    m_totalProcessesCard = new StatCard(QStringLiteral("Total Processes"), Theme::PrimaryPurple);
    m_avgWaitingCard = new StatCard(QStringLiteral("Avg Waiting Time"), Theme::PinkAccent);
    m_avgTurnaroundCard = new StatCard(QStringLiteral("Avg Turnaround Time"), Theme::CyanAccent);
    m_totalExecutionCard = new StatCard(QStringLiteral("Total Execution Time"), Theme::DarkNavy);
    statsRow->addWidget(m_totalProcessesCard);
    statsRow->addWidget(m_avgWaitingCard);
    statsRow->addWidget(m_avgTurnaroundCard);
    statsRow->addWidget(m_totalExecutionCard);
    layout->addLayout(statsRow);

    // --- Process input card --------------------------------------------
    auto *processCard = makeCard();
    auto *processLayout = new QVBoxLayout(processCard);
    processLayout->setContentsMargins(20, 18, 20, 18);
    processLayout->setSpacing(12);

    auto *processHeaderRow = new QHBoxLayout();
    auto *processTitleBox = new QVBoxLayout();
    processTitleBox->setSpacing(2);
    auto *processTitle = new QLabel(QStringLiteral("Process Queue"));
    processTitle->setObjectName("sectionTitle");
    auto *processCaption = new QLabel(QStringLiteral("Define the processes to schedule"));
    processCaption->setObjectName("sectionCaption");
    processTitleBox->addWidget(processTitle);
    processTitleBox->addWidget(processCaption);
    processHeaderRow->addLayout(processTitleBox);
    processHeaderRow->addStretch(1);

    auto *addButton = new QPushButton(QStringLiteral("+  Add Process"));
    addButton->setObjectName("primaryButton");
    addButton->setCursor(Qt::PointingHandCursor);
    processHeaderRow->addWidget(addButton);
    processLayout->addLayout(processHeaderRow);

    m_processTable = new QTableWidget(0, 4);
    styleResultTable(m_processTable);
    m_processTable->setHorizontalHeaderLabels({QStringLiteral("PID"), QStringLiteral("Arrival Time"),
                                                QStringLiteral("Burst Time"), QStringLiteral("Actions")});
    m_processTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_processTable->setColumnWidth(3, 160);
    processLayout->addWidget(m_processTable);

    layout->addWidget(processCard);

    // --- Gantt chart ----------------------------------------------------
    m_ganttPanel = new GanttChartPanel();
    layout->addWidget(m_ganttPanel);

    // --- Results card -----------------------------------------------------
    auto *resultsCard = makeCard();
    auto *resultsLayout = new QVBoxLayout(resultsCard);
    resultsLayout->setContentsMargins(20, 18, 20, 18);
    resultsLayout->setSpacing(12);

    auto *resultsTitle = new QLabel(QStringLiteral("Simulation Results"));
    resultsTitle->setObjectName("sectionTitle");
    resultsLayout->addWidget(resultsTitle);

    m_resultsTable = new QTableWidget(0, 6);
    styleResultTable(m_resultsTable);
    m_resultsTable->setHorizontalHeaderLabels({QStringLiteral("PID"), QStringLiteral("Arrival Time"),
                                                QStringLiteral("Burst Time"), QStringLiteral("Completion Time"),
                                                QStringLiteral("Turnaround Time"), QStringLiteral("Waiting Time")});
    resultsLayout->addWidget(m_resultsTable);

    layout->addWidget(resultsCard);
    layout->addStretch(1);

    scrollArea->setWidget(content);

    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::onRunSimulation);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddProcessClicked);

    clearResults();
    return scrollArea;
}

QWidget *MainWindow::buildOverviewPage()
{
    auto *scrollArea = new QScrollArea();
    scrollArea->setObjectName("pageScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget();
    content->setObjectName("pageContent");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(18);

    auto *title = new QLabel(QStringLiteral("Overview"));
    title->setObjectName("pageTitle");
    auto *subtitle = new QLabel(QStringLiteral("A desktop dashboard for visualizing CPU scheduling algorithms."));
    subtitle->setObjectName("pageSubtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    auto *introCard = makeCard();
    auto *introLayout = new QVBoxLayout(introCard);
    introLayout->setContentsMargins(22, 20, 22, 20);
    introLayout->setSpacing(10);
    auto *introTitle = new QLabel(QStringLiteral("What this tool does"));
    introTitle->setObjectName("sectionTitle");
    auto *introBody = new QLabel(QStringLiteral(
        "Define a queue of processes with their arrival and burst times, choose between "
        "First-Come-First-Served, Shortest Job First, or Shortest Remaining Time First, and "
        "watch the resulting schedule animate on a proportional Gantt timeline — complete "
        "with completion, turnaround, and waiting time statistics computed from the real "
        "scheduling result, not placeholder numbers."));
    introBody->setWordWrap(true);
    introBody->setStyleSheet(QStringLiteral("color:#55546E; font-size:12px;"));
    introLayout->addWidget(introTitle);
    introLayout->addWidget(introBody);
    layout->addWidget(introCard);

    auto *chipsRow = new QHBoxLayout();
    chipsRow->setSpacing(14);

    struct Chip { QString name; QString desc; QColor color; };
    const std::vector<Chip> chips = {
        {QStringLiteral("FCFS"), QStringLiteral("Runs processes strictly in arrival order."), Theme::PrimaryPurple},
        {QStringLiteral("SJF"), QStringLiteral("Non-preemptive — always picks the shortest ready job."), Theme::PinkAccent},
        {QStringLiteral("SRTF"), QStringLiteral("Preemptive — switches to whichever job has the least time left."), Theme::CyanAccent},
    };
    for (const Chip &chip : chips) {
        auto *card = makeCard();
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 16, 18, 16);
        cardLayout->setSpacing(8);
        auto *bar = new QFrame();
        bar->setFixedSize(28, 4);
        bar->setStyleSheet(QStringLiteral("background:%1; border-radius:2px;").arg(chip.color.name()));
        auto *name = new QLabel(chip.name);
        name->setObjectName("sectionTitle");
        auto *desc = new QLabel(chip.desc);
        desc->setWordWrap(true);
        desc->setStyleSheet(QStringLiteral("color:#85849A; font-size:11px;"));
        cardLayout->addWidget(bar);
        cardLayout->addWidget(name);
        cardLayout->addWidget(desc);
        chipsRow->addWidget(card);
    }
    layout->addLayout(chipsRow);
    layout->addStretch(1);

    scrollArea->setWidget(content);
    return scrollArea;
}

QWidget *MainWindow::buildAlgorithmsPage()
{
    auto *scrollArea = new QScrollArea();
    scrollArea->setObjectName("pageScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget();
    content->setObjectName("pageContent");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(18);

    auto *title = new QLabel(QStringLiteral("Algorithms"));
    title->setObjectName("pageTitle");
    auto *subtitle = new QLabel(QStringLiteral("The three scheduling strategies implemented in this simulator."));
    subtitle->setObjectName("pageSubtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    struct AlgoInfo { QString name; QString tagline; QString detail; QColor color; };
    const std::vector<AlgoInfo> infos = {
        {QStringLiteral("First Come First Served (FCFS)"),
         QStringLiteral("Non-preemptive · ordered by arrival time"),
         QStringLiteral("Processes are sorted by arrival time and run to completion in that order. "
                         "Simple and fair in arrival order, but a long process can make every process "
                         "behind it wait — the classic convoy effect."),
         Theme::PrimaryPurple},
        {QStringLiteral("Shortest Job First (SJF)"),
         QStringLiteral("Non-preemptive · shortest burst time first"),
         QStringLiteral("Among all processes that have already arrived, the one with the smallest burst "
                         "time runs next, to completion. Ties are broken by earlier arrival time. Minimizes "
                         "average waiting time, but requires knowing burst times in advance and can starve "
                         "longer jobs."),
         Theme::PinkAccent},
        {QStringLiteral("Shortest Remaining Time First (SRTF)"),
         QStringLiteral("Preemptive · shortest remaining time first"),
         QStringLiteral("The preemptive version of SJF: at every time unit, the CPU switches to whichever "
                         "ready process has the least remaining work, interrupting the current process if a "
                         "newly arrived one is shorter. Produces the lowest possible average waiting time at "
                         "the cost of frequent context switches."),
         Theme::CyanAccent},
    };

    for (const AlgoInfo &info : infos) {
        auto *card = makeCard();
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(20, 18, 20, 18);
        cardLayout->setSpacing(8);

        auto *bar = new QFrame();
        bar->setFixedSize(32, 4);
        bar->setStyleSheet(QStringLiteral("background:%1; border-radius:2px;").arg(info.color.name()));

        auto *name = new QLabel(info.name);
        name->setObjectName("sectionTitle");
        auto *tagline = new QLabel(info.tagline);
        tagline->setStyleSheet(QStringLiteral("color:%1; font-size:11px; font-weight:700;").arg(info.color.name()));
        auto *detail = new QLabel(info.detail);
        detail->setWordWrap(true);
        detail->setStyleSheet(QStringLiteral("color:#55546E; font-size:12px;"));

        cardLayout->addWidget(bar);
        cardLayout->addWidget(name);
        cardLayout->addWidget(tagline);
        cardLayout->addWidget(detail);
        layout->addWidget(card);
    }

    layout->addStretch(1);
    scrollArea->setWidget(content);
    return scrollArea;
}

QWidget *MainWindow::buildAboutPage()
{
    auto *scrollArea = new QScrollArea();
    scrollArea->setObjectName("pageScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget();
    content->setObjectName("pageContent");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(18);

    auto *title = new QLabel(QStringLiteral("About"));
    title->setObjectName("pageTitle");
    auto *subtitle = new QLabel(QStringLiteral("Project details and architecture."));
    subtitle->setObjectName("pageSubtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    auto *card = makeCard();
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(22, 20, 22, 20);
    cardLayout->setSpacing(10);

    auto *name = new QLabel(QStringLiteral("CPU Scheduling Simulator"));
    name->setObjectName("sectionTitle");
    auto *body = new QLabel(QStringLiteral(
        "A C++ / Qt 6 Widgets desktop application that simulates and visualizes three CPU "
        "scheduling algorithms. The codebase is split into three layers: core (shared Process, "
        "GanttSegment, and ScheduleResult data structures), algorithms (the FCFS, SJF, and SRTF "
        "scheduler implementations behind a common SchedulingAlgorithm interface), and gui (this "
        "dashboard, which only collects input and renders results — it never reimplements "
        "scheduling logic). Built with CMake and the Qt 6 Widgets module."));
    body->setWordWrap(true);
    body->setStyleSheet(QStringLiteral("color:#55546E; font-size:12px;"));

    cardLayout->addWidget(name);
    cardLayout->addWidget(body);
    layout->addWidget(card);
    layout->addStretch(1);

    scrollArea->setWidget(content);
    return scrollArea;
}

void MainWindow::refreshProcessTable()
{
    m_processTable->setRowCount(static_cast<int>(m_processes.size()));
    for (int row = 0; row < static_cast<int>(m_processes.size()); ++row) {
        const Process &p = m_processes[static_cast<size_t>(row)];

        m_processTable->setItem(row, 0, makeCenteredItem(QStringLiteral("P%1").arg(p.pid)));
        m_processTable->setItem(row, 1, makeCenteredItem(QString::number(p.arrivalTime)));
        m_processTable->setItem(row, 2, makeCenteredItem(QString::number(p.burstTime)));

        auto *actionsWidget = new QWidget();
        auto *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(4, 2, 4, 2);
        actionsLayout->setSpacing(6);

        auto *editButton = new QPushButton(QStringLiteral("Edit"));
        editButton->setObjectName("iconButton");
        editButton->setCursor(Qt::PointingHandCursor);
        editButton->setFocusPolicy(Qt::NoFocus);

        auto *deleteButton = new QPushButton(QStringLiteral("Delete"));
        deleteButton->setObjectName("deleteIconButton");
        deleteButton->setCursor(Qt::PointingHandCursor);
        deleteButton->setFocusPolicy(Qt::NoFocus);

        const int pid = p.pid;
        connect(editButton, &QPushButton::clicked, this, [this, pid]() { editProcessByPid(pid); });
        connect(deleteButton, &QPushButton::clicked, this, [this, pid]() { deleteProcessByPid(pid); });

        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);
        actionsLayout->addStretch(1);

        m_processTable->setCellWidget(row, 3, actionsWidget);
    }

    // Deferred: at startup this runs before the window is shown, when the
    // table's header/row/frame metrics haven't been laid out or style-polished
    // yet, so measuring synchronously here would undercount the true rendered
    // row height. QTimer::singleShot(0, ...) runs this right after Qt
    // processes the pending layout/polish events instead.
    QTimer::singleShot(0, this, [this]() {
        resizeTableToContents(m_processTable);
        m_processTable->scrollToTop();
    });
}

void MainWindow::editProcessByPid(int pid)
{
    auto it = std::find_if(m_processes.begin(), m_processes.end(),
                            [pid](const Process &p) { return p.pid == pid; });
    if (it == m_processes.end())
        return;

    ProcessDialog dialog(m_processes, pid, this);
    dialog.setInitialValues(*it);
    if (dialog.exec() == QDialog::Accepted) {
        const Process updated = dialog.resultProcess();
        it->arrivalTime = updated.arrivalTime;
        it->burstTime = updated.burstTime;
        refreshProcessTable();
    }
}

void MainWindow::deleteProcessByPid(int pid)
{
    m_processes.erase(std::remove_if(m_processes.begin(), m_processes.end(),
                                      [pid](const Process &p) { return p.pid == pid; }),
                       m_processes.end());
    refreshProcessTable();
}

void MainWindow::onAddProcessClicked()
{
    ProcessDialog dialog(m_processes, -1, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_processes.push_back(dialog.resultProcess());
        refreshProcessTable();
    }
}

void MainWindow::onRunSimulation()
{
    if (m_processes.empty()) {
        QMessageBox::information(this, QStringLiteral("No Processes"),
                                  QStringLiteral("Add at least one process before running a simulation."));
        return;
    }

    std::unique_ptr<SchedulingAlgorithm> scheduler;
    switch (m_algorithmCombo->currentIndex()) {
    case 0: scheduler = std::make_unique<FCFSScheduler>(); break;
    case 1: scheduler = std::make_unique<SJFScheduler>(); break;
    case 2: scheduler = std::make_unique<SRTFScheduler>(); break;
    default: return;
    }

    try {
        const ScheduleResult result = scheduler->schedule(m_processes);
        applyResult(result);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, QStringLiteral("Simulation Error"), QString::fromStdString(e.what()));
    }
}

void MainWindow::onResetClicked()
{
    m_processes.clear();
    refreshProcessTable();
    clearResults();
}

void MainWindow::applyResult(const ScheduleResult &result)
{
    m_totalProcessesCard->setValue(QString::number(result.processes.size()));
    m_avgWaitingCard->setValue(QString::number(result.avgWaitingTime, 'f', 2));
    m_avgTurnaroundCard->setValue(QString::number(result.avgTurnaroundTime, 'f', 2));

    int totalExecutionTime = 0;
    for (const GanttSegment &seg : result.ganttChart)
        totalExecutionTime = std::max(totalExecutionTime, seg.endTime);
    m_totalExecutionCard->setValue(QString::number(totalExecutionTime));

    m_resultsTable->setRowCount(static_cast<int>(result.processes.size()));
    for (int row = 0; row < static_cast<int>(result.processes.size()); ++row) {
        const Process &p = result.processes[static_cast<size_t>(row)];
        m_resultsTable->setItem(row, 0, makeCenteredItem(QStringLiteral("P%1").arg(p.pid)));
        m_resultsTable->setItem(row, 1, makeCenteredItem(QString::number(p.arrivalTime)));
        m_resultsTable->setItem(row, 2, makeCenteredItem(QString::number(p.burstTime)));
        m_resultsTable->setItem(row, 3, makeCenteredItem(QString::number(p.completionTime)));
        m_resultsTable->setItem(row, 4, makeCenteredItem(QString::number(p.turnaroundTime)));
        m_resultsTable->setItem(row, 5, makeCenteredItem(QString::number(p.waitingTime)));
    }
    resizeTableToContents(m_resultsTable);
    m_resultsTable->scrollToTop();

    m_ganttPanel->setSegments(result.ganttChart, true);
}

void MainWindow::clearResults()
{
    m_totalProcessesCard->setValue(QStringLiteral("--"));
    m_avgWaitingCard->setValue(QStringLiteral("--"));
    m_avgTurnaroundCard->setValue(QStringLiteral("--"));
    m_totalExecutionCard->setValue(QStringLiteral("--"));
    m_resultsTable->setRowCount(0);
    resizeTableToContents(m_resultsTable);
    m_ganttPanel->clear();
}
