#ifndef PROCESSSIMAPP_H
#define PROCESSSIMAPP_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include <QVector>

struct SimulatedProcess {
    int pid;
    int ppid;
    QString name;
    QString state; 
    int timeLeft;  
};

class ProcessSimApp : public QWidget {
    Q_OBJECT
public:
    explicit ProcessSimApp(QWidget *parent = nullptr);
    ~ProcessSimApp();

private slots:
    void spawnZombieScenario();
    void spawnOrphanScenario();
    void onClockTick();

private:
    QTableWidget *processTable;
    QTextEdit *consoleLog;
    QTimer *systemClock;
    QVector<SimulatedProcess> activeProcesses;
    int nextPid;

    void logMessage(const QString &msg);
    void renderTable();
};

#endif // PROCESSSIMAPP_H