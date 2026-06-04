#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include <QString>
#include <QList>
#include <QMutex>

// Ensure the struct matches exactly what your project expects
struct SimProcess {
    int pid;
    int ppid;
    QString state;
};

class ProcessTable {
public:
    ProcessTable();

    void spawnProcess(int pid, int ppid);
    void completeProcess(int pid);
    QString getTableSnapshot();

private:
    QList<SimProcess> table; // Matching your QList container
    QMutex tableMutex;       // Keeps everything thread-safe
};

#endif // PROCESSTABLE_H