#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>
#include <QString>
#include "job.h"
#include "processtable.h"
#include "memorymanager.h"
#include "bankers.h"

enum class Algorithm { FCFS, PRIORITY, SJF, RR };

class Scheduler : public QThread {
    Q_OBJECT

public:
    Scheduler();
    ~Scheduler();

    void addJob(int priority, int burstTime);
    void setAlgorithm(Algorithm algo);
    void setQuantum(int q);              
    void toggleExecution(bool start);    
    void stopScheduler();
    void broadcastQueue(); 

signals:
    void systemLog(QString message);
    void queueUpdate(QString queueData);

protected:
    void run() override; 

private:
    QVector<Job> jobQueue;
    QMutex queueMutex;            
    QWaitCondition condition;     
    bool isRunning;
    bool isExecuting;             
    int timeQuantum;              
    int nextJobId;
    Algorithm activeAlgorithm;

    // OS Subsystem Modules
    ProcessTable processTable;
    MemoryManager memManager;
    BankersAlgorithm bankers;

    void sortQueue();             
};

#endif // SCHEDULER_H