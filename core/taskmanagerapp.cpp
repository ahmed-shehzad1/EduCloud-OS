#include "taskmanagerapp.h"

TaskManagerApp::TaskManagerApp(QWidget *parent) : QWidget(parent) {
    this->setStyleSheet("background-color: #11111b; color: #cdd6f4; font-family: 'Courier New';");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    mainLayout->addWidget(new QLabel("<b>System Performance</b>"));

    // CPU Layout
    QHBoxLayout *cpuLayout = new QHBoxLayout();
    cpuLayout->addWidget(new QLabel("CPU Load:"));
    cpuBar = new QProgressBar();
    cpuBar->setRange(0, 100);
    cpuBar->setStyleSheet("QProgressBar::chunk { background-color: #fab387; border-radius: 4px; }");
    cpuLayout->addWidget(cpuBar);

    // RAM Layout
    QHBoxLayout *ramLayout = new QHBoxLayout();
    ramLayout->addWidget(new QLabel("RAM Load:"));
    ramBar = new QProgressBar();
    ramBar->setRange(0, 100);
    ramBar->setStyleSheet("QProgressBar::chunk { background-color: #89b4fa; border-radius: 4px; }");
    ramLayout->addWidget(ramBar);

    mainLayout->addLayout(cpuLayout);
    mainLayout->addLayout(ramLayout);

    mainLayout->addWidget(new QLabel("<b>Active Background Processes</b>"));
    processTableMock = new QTextEdit();
    processTableMock->setReadOnly(true);
    processTableMock->setStyleSheet("background-color: #1e1e2e; border: 1px solid #45475a; border-radius: 5px;");
    processTableMock->setText("PID 1 | Init System | RUNNING\nPID 2 | Desktop Environment | RUNNING\nPID 3 | Task Manager | RUNNING");
    mainLayout->addWidget(processTableMock);

    // Start live tracking
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &TaskManagerApp::updateMetrics);
    refreshTimer->start(1000);
}

void TaskManagerApp::updateMetrics() {
    ramBar->setValue(monitor.getRealMemoryUsage());
    // Simulate CPU usage fluctuating since the UI is active
    cpuBar->setValue(monitor.getCpuUsage(true, true)); 
}