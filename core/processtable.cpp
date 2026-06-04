#include "processtable.h"
#include <QMutexLocker>

ProcessTable::ProcessTable() {}

void ProcessTable::spawnProcess(int pid, int ppid) {
    QMutexLocker locker(&tableMutex);
    
    // Force root parent to init process (PID 1) if invalid
    if (ppid == 0 && pid != 1) ppid = 1; 

    SimProcess newProc = {pid, ppid, "RUNNING"};
    table.append(newProc);
}

void ProcessTable::completeProcess(int pid) {
    QMutexLocker locker(&tableMutex);

    int parentId = 1;
    bool parentIsRunning = false;

    // 1. Find the target process and its parent's status
    for (auto& proc : table) {
        if (proc.pid == pid) {
            parentId = proc.ppid;
            break;
        }
    }

    // Check if that parent is still running in the list
    for (const auto& proc : table) {
        if (proc.pid == parentId && proc.state == "RUNNING") {
            parentIsRunning = true;
            break;
        }
    }

    // 2. Set my execution state (ZOMBIE vs COMPLETED)
    for (auto& proc : table) {
        if (proc.pid == pid) {
            if (parentIsRunning) {
                proc.state = "ZOMBIE (Awaiting Parent " + QString::number(parentId) + ")";
            } else {
                proc.state = "COMPLETED";
            }
            break;
        }
    }

  // 3. Adopt any Orphans left behind by this completed process
    for (int i = 0; i < table.size(); ++i) {
        if (table[i].ppid == pid && table[i].state == "RUNNING") {
            table[i].ppid = 1; // Adopted by Init (PID 1)
            table[i].state = "ORPHAN (Adopted by Init PID 1)";
        }
    }
}

QString ProcessTable::getTableSnapshot() {
    QMutexLocker locker(&tableMutex);
    QString snapshot = "--- SYSTEM PROCESS TABLE ---\n";
    
    // Iterate through the QList using proper pointer syntax (it->)
    for (auto it = table.begin(); it != table.end(); ++it) {
        snapshot += QString("PID: %1 | PPID: %2 | STATE: %3\n")
                    .arg(it->pid)
                    .arg(it->ppid)
                    .arg(it->state);
    }
    return snapshot;
}