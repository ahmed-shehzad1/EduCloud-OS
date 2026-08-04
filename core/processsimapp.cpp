#include "processsimapp.h"

ProcessSimApp::ProcessSimApp(QWidget *parent) : QWidget(parent), nextPid(2000) {
    // Styling to cleanly match your active operating system workspace palette
    this->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4; font-family: 'Consolas', monospace;");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *leftColumn = new QVBoxLayout();
    QVBoxLayout *rightColumn = new QVBoxLayout();

    QLabel *titleLabel = new QLabel("🎮 Simulation Panel");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #89b4fa;");
    leftColumn->addWidget(titleLabel);

    QPushButton *btnZombie = new QPushButton("🧟 Spawn Zombie Process");
    QPushButton *btnOrphan = new QPushButton("🍂 Spawn Orphan Process");
    
    btnZombie->setStyleSheet("background-color: #313244; border: 1px solid #f38ba8; padding: 8px; border-radius: 6px; font-weight: bold; color: #f38ba8;");
    btnOrphan->setStyleSheet("background-color: #313244; border: 1px solid #f9e2af; padding: 8px; border-radius: 6px; font-weight: bold; color: #f9e2af;");
    
    leftColumn->addWidget(btnZombie);
    leftColumn->addWidget(btnOrphan);
    leftColumn->addSpacing(10);

    leftColumn->addWidget(new QLabel("📋 OS Interrupt Tracker Logs:"));
    consoleLog = new QTextEdit();
    consoleLog->setReadOnly(true);
    consoleLog->setStyleSheet("background-color: #11111b; border: 1px solid #45475a; color: #a6e3a1; font-size: 11px;");
    leftColumn->addWidget(consoleLog);

    QLabel *tableLabel = new QLabel("📊 Kernel Process State Table Entry");
    tableLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #89b4fa;");
    rightColumn->addWidget(tableLabel);

    processTable = new QTableWidget(0, 5);
    processTable->setHorizontalHeaderLabels({"PID", "PPID", "Process Name", "State", "TTL (s)"});
    processTable->setStyleSheet("background-color: #11111b; gridline-color: #313244; color: #cdd6f4;");
    processTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    processTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #313244; color: #cdd6f4; font-weight: bold; }");
    processTable->verticalHeader()->setVisible(false);
    rightColumn->addWidget(processTable);

    mainLayout->addLayout(leftColumn, 4);
    mainLayout->addLayout(rightColumn, 6);

    connect(btnZombie, &QPushButton::clicked, this, &ProcessSimApp::spawnZombieScenario);
    connect(btnOrphan, &QPushButton::clicked, this, &ProcessSimApp::spawnOrphanScenario);

    systemClock = new QTimer(this);
    connect(systemClock, &QTimer::timeout, this, &ProcessSimApp::onClockTick);
    systemClock->start(1000);

    logMessage("Process Lifecycle Subsystem initialized successfully.");
}

ProcessSimApp::~ProcessSimApp() {}

void ProcessSimApp::logMessage(const QString &msg) {
    consoleLog->append(msg);
}

void ProcessSimApp::spawnZombieScenario() {
    int parentPid = nextPid++;
    int childPid = nextPid++;

    SimulatedProcess parent = {parentPid, 1, "AppParent", "RUNNING", 12};
    SimulatedProcess child = {childPid, parentPid, "ZombieChild", "🧟 ZOMBIE", 6};

    activeProcesses.append(parent);
    activeProcesses.append(child);

    logMessage(QString("\n[FORK] Parent (PID: %1) created Child (PID: %2).").arg(parentPid).arg(childPid));
    logMessage(QString("🧟 [ZOMBIE] Child hit exit(). Parent hasn't called wait()."));
    renderTable();
}

void ProcessSimApp::spawnOrphanScenario() {
    int parentPid = nextPid++;
    int childPid = nextPid++;

    SimulatedProcess parent = {parentPid, 1, "ShortParent", "RUNNING", 3};
    SimulatedProcess child = {childPid, parentPid, "OrphanChild", "RUNNING", 10};

    activeProcesses.append(parent);
    activeProcesses.append(child);

    logMessage(QString("\n[FORK] Parent (PID: %1) spawned Long-Running Child (PID: %2).").arg(parentPid).arg(childPid));
    renderTable();
}

void ProcessSimApp::onClockTick() {
    for (int i = activeProcesses.size() - 1; i >= 0; --i) {
        activeProcesses[i].timeLeft--;

        if (activeProcesses[i].name == "OrphanChild" && activeProcesses[i].state == "RUNNING") {
            bool parentFound = false;
            for (const auto &proc : activeProcesses) {
                if (proc.pid == activeProcesses[i].ppid) { parentFound = true; break; }
            }
            if (!parentFound && activeProcesses[i].ppid != 1) {
                activeProcesses[i].ppid = 1;
                activeProcesses[i].state = "🍂 ORPHANED";
                logMessage(QString("🍂 [ADOPTION] Parent died. Process %1 adopted by System Init (PID 1).").arg(activeProcesses[i].pid));
            }
        }

        if (activeProcesses[i].timeLeft <= 0) {
            if (activeProcesses[i].state.contains("ZOMBIE")) {
                logMessage(QString("[REAP] Zombie %1 cleared from Process Table entries.").arg(activeProcesses[i].pid));
            }
            activeProcesses.removeAt(i);
        }
    }
    renderTable();
}

void ProcessSimApp::renderTable() {
    processTable->setRowCount(0);
    for (int i = 0; i < activeProcesses.size(); ++i) {
        processTable->insertRow(i);
        processTable->setItem(i, 0, new QTableWidgetItem(QString::number(activeProcesses[i].pid)));
        processTable->setItem(i, 1, new QTableWidgetItem(QString::number(activeProcesses[i].ppid)));
        processTable->setItem(i, 2, new QTableWidgetItem(activeProcesses[i].name));
        
        QTableWidgetItem *stateItem = new QTableWidgetItem(activeProcesses[i].state);
        if (activeProcesses[i].state.contains("ZOMBIE")) stateItem->setForeground(QColor("#f38ba8"));
        else if (activeProcesses[i].state.contains("ORPHAN")) stateItem->setForeground(QColor("#f9e2af"));
        else stateItem->setForeground(QColor("#a6e3a1"));
        
        processTable->setItem(i, 3, stateItem);
        processTable->setItem(i, 4, new QTableWidgetItem(QString::number(activeProcesses[i].timeLeft) + "s"));
    }
}