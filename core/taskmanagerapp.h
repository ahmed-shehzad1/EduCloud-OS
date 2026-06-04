#ifndef TASKMANAGERAPP_H
#define TASKMANAGERAPP_H

#include <QWidget>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "systemmonitor.h"

class TaskManagerApp : public QWidget {
    Q_OBJECT

public:
    explicit TaskManagerApp(QWidget *parent = nullptr);

private slots:
    void updateMetrics();

private:
    QProgressBar *cpuBar;
    QProgressBar *ramBar;
    QTextEdit *processTableMock;
    QTimer *refreshTimer;
    SystemMonitor monitor;
};

#endif // TASKMANAGERAPP_H