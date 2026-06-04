#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), isSystemExecuting(false) {
    setWindowTitle("EduCloud OS - Orchestration Dashboard");
    resize(1050, 700); 

    // --- PREMIUM DARK MODE STYLING ---
    this->setStyleSheet(R"(
        QMainWindow { background-color: #1e1e2e; }
        QLabel { color: #cdd6f4; font-size: 14px; font-weight: bold; }
        QSpinBox, QComboBox {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 6px;
            padding: 6px; font-size: 13px;
        }
        QPushButton {
            background-color: #89b4fa; color: #11111b;
            border: none; border-radius: 6px;
            padding: 8px 16px; font-weight: bold; font-size: 13px;
        }
        QPushButton:hover { background-color: #b4befe; }
        QPushButton:pressed { background-color: #74c7ec; }
        QTextEdit {
            background-color: #11111b; border: 1px solid #45475a;
            border-radius: 8px; padding: 10px;
        }
        QTabWidget::pane { border: 1px solid #45475a; border-radius: 8px; background: #181825; }
        QTabBar::tab {
            background: #313244; color: #a6adc8; padding: 10px 24px;
            border-top-left-radius: 6px; border-top-right-radius: 6px; margin-right: 2px;
        }
        QTabBar::tab:selected { background: #89b4fa; color: #11111b; font-weight: bold; }
        QProgressBar {
            border: 1px solid #45475a; border-radius: 6px;
            text-align: center; color: white; font-weight: bold; background-color: #313244;
        }
        QScrollBar:vertical { border: none; background: #1e1e2e; width: 12px; margin: 0px; }
        QScrollBar::handle:vertical { background: #45475a; min-height: 20px; border-radius: 6px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { border: none; background: none; }
    )");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // --- TOP CONTROL PANEL ---
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    controlLayout->addWidget(new QLabel("Algorithm:"));
    policySelector = new QComboBox();
    policySelector->addItems({"First Come First Serve (FCFS)", "Priority Scheduling", "Shortest Job First (SJF)", "Round Robin (RR)"});
    controlLayout->addWidget(policySelector);

    controlLayout->addWidget(new QLabel("  Pri (1-10):"));
    priorityInput = new QSpinBox(); priorityInput->setRange(1, 10); priorityInput->setEnabled(false); 
    controlLayout->addWidget(priorityInput);

    controlLayout->addWidget(new QLabel("  Burst (s):"));
    burstInput = new QSpinBox(); burstInput->setRange(1, 20);
    controlLayout->addWidget(burstInput);

    controlLayout->addWidget(new QLabel("  Time Slice:"));
    quantumInput = new QSpinBox(); quantumInput->setRange(1, 10); quantumInput->setValue(2); quantumInput->setEnabled(false);
    controlLayout->addWidget(quantumInput);

    submitBtn = new QPushButton("➕ Add Task");
    controlLayout->addWidget(submitBtn);

    executionBtn = new QPushButton("▶️ START EXECUTION");
    executionBtn->setStyleSheet("background-color: #a6e3a1; color: #11111b; font-weight: bold;"); 
    controlLayout->addWidget(executionBtn);

    // --- SYSTEM RESOURCE MONITORING ---
    QHBoxLayout *monitorLayout = new QHBoxLayout();
    monitorLayout->addWidget(new QLabel("<b>Hardware Load:</b>"));
    
    monitorLayout->addWidget(new QLabel("CPU Allocation:"));
    cpuUsageBar = new QProgressBar(); cpuUsageBar->setRange(0, 100); cpuUsageBar->setValue(0);
    cpuUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #fab387; border-radius: 5px; }"); 
    monitorLayout->addWidget(cpuUsageBar);

    monitorLayout->addWidget(new QLabel("Memory Usage:"));
    memoryUsageBar = new QProgressBar(); memoryUsageBar->setRange(0, 100); memoryUsageBar->setValue(0);
    memoryUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #89b4fa; border-radius: 5px; }"); 
    monitorLayout->addWidget(memoryUsageBar);

    monitorTimer = new QTimer(this);
    connect(monitorTimer, &QTimer::timeout, this, &MainWindow::updateSystemMetrics);
    monitorTimer->start(1000); 

    // --- BOTTOM DISPLAY SCREENS ---
    QHBoxLayout *displayLayout = new QHBoxLayout();
    
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(new QLabel("<b>Live System Logs</b>"));
    logDisplay = new QTextEdit(); logDisplay->setReadOnly(true);
    logDisplay->setStyleSheet("color: #cdd6f4; font-family: 'Courier New', monospace; font-size: 13px;");
    logLayout->addWidget(logDisplay);

    QVBoxLayout *tabsLayout = new QVBoxLayout();
    systemTabs = new QTabWidget();

    queueDisplay = new QTextEdit(); queueDisplay->setReadOnly(true);
    queueDisplay->setStyleSheet("color: #a6e3a1; font-family: 'Courier New', monospace; font-size: 14px;"); 
    systemTabs->addTab(queueDisplay, "⚙️ Pipeline");

    processTableDisplay = new QTextEdit(); processTableDisplay->setReadOnly(true);
    processTableDisplay->setStyleSheet("color: #f9e2af; font-family: 'Courier New', monospace; font-size: 14px;"); 
    systemTabs->addTab(processTableDisplay, "🧟 Process Table");

    memoryMapDisplay = new QTextEdit(); memoryMapDisplay->setReadOnly(true);
    memoryMapDisplay->setStyleSheet("color: #89dceb; font-family: 'Courier New', monospace; font-size: 14px;"); 
    systemTabs->addTab(memoryMapDisplay, "💾 Memory Map");

    tabsLayout->addWidget(systemTabs);
    displayLayout->addLayout(logLayout, 2); 
    displayLayout->addLayout(tabsLayout, 3); 

    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(monitorLayout);
    mainLayout->addLayout(displayLayout);
    setCentralWidget(centralWidget);

    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::handleSubmit);
    connect(executionBtn, &QPushButton::clicked, this, &MainWindow::toggleExecution);
    connect(policySelector, &QComboBox::currentIndexChanged, this, &MainWindow::handlePolicyChange);
    connect(quantumInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::handleQuantumChange);
    
    connect(&osScheduler, &Scheduler::systemLog, this, &MainWindow::updateLog);
    connect(&osScheduler, &Scheduler::queueUpdate, this, &MainWindow::updateQueue);

    osScheduler.start(); 
}

MainWindow::~MainWindow() { osScheduler.stopScheduler(); }

void MainWindow::handleSubmit() { osScheduler.addJob(priorityInput->value(), burstInput->value()); }

void MainWindow::toggleExecution() {
    isSystemExecuting = !isSystemExecuting;
    if (isSystemExecuting) {
        executionBtn->setText("⏸️ PAUSE EXECUTION");
        executionBtn->setStyleSheet("background-color: #f38ba8; color: #11111b; font-weight: bold;"); 
        osScheduler.toggleExecution(true);
    } else {
        executionBtn->setText("▶️ START EXECUTION");
        executionBtn->setStyleSheet("background-color: #a6e3a1; color: #11111b; font-weight: bold;"); 
        osScheduler.toggleExecution(false);
    }
}

void MainWindow::handlePolicyChange(int index) {
    priorityInput->setEnabled(index == 1); 
    quantumInput->setEnabled(index == 3);  
    if (index == 0) osScheduler.setAlgorithm(Algorithm::FCFS);
    else if (index == 1) osScheduler.setAlgorithm(Algorithm::PRIORITY);
    else if (index == 2) osScheduler.setAlgorithm(Algorithm::SJF);
    else if (index == 3) osScheduler.setAlgorithm(Algorithm::RR);
}

void MainWindow::handleQuantumChange(int val) { osScheduler.setQuantum(val); }

void MainWindow::updateLog(QString msg) { logDisplay->append(msg); }

void MainWindow::updateQueue(QString qData) {
    QStringList parts = qData.split("--- SYSTEM PROCESS TABLE ---");
    if (parts.size() >= 2) {
        QString queueText = parts[0].trimmed();
        QStringList subParts = parts[1].split("--- PHYSICAL MEMORY PAGES ---");
        if (subParts.size() >= 2) {
            queueDisplay->setText(queueText);
            processTableDisplay->setText("--- SYSTEM PROCESS TABLE ---\n" + subParts[0].trimmed());
            memoryMapDisplay->setText("--- PHYSICAL MEMORY PAGES ---\n" + subParts[1].trimmed());
            return;
        }
    }
    queueDisplay->setText(qData); 
}

void MainWindow::updateSystemMetrics() {
    memoryUsageBar->setValue(sysMonitor.getRealMemoryUsage());
    QString currentQueueText = queueDisplay->toPlainText();
    cpuUsageBar->setValue(sysMonitor.getCpuUsage(isSystemExecuting, currentQueueText.contains("RUNNING")));
}