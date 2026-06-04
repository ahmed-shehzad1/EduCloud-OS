#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QProgressBar> 
#include <QTimer>       
#include <QTabWidget>
#include "scheduler.h"
#include "systemmonitor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleSubmit(); 
    void handlePolicyChange(int index);
    void handleQuantumChange(int val);
    void toggleExecution(); 
    void updateLog(QString msg);
    void updateQueue(QString qData);
    void updateSystemMetrics(); 

private:
    Scheduler osScheduler; 
    SystemMonitor sysMonitor;
    bool isSystemExecuting; 
    
    QSpinBox *priorityInput;
    QSpinBox *burstInput;
    QSpinBox *quantumInput; 
    
    QPushButton *submitBtn;
    QPushButton *executionBtn; 
    QComboBox *policySelector;
    
    QTextEdit *logDisplay;
    QTabWidget *systemTabs;
    QTextEdit *queueDisplay;
    QTextEdit *processTableDisplay; 
    QTextEdit *memoryMapDisplay;    

    QProgressBar *cpuUsageBar;
    QProgressBar *memoryUsageBar;
    QTimer *monitorTimer;
};

#endif // MAINWINDOW_H