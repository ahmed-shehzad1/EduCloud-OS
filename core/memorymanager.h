#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <QString>
#include <QList>
#include <QMutex>

// Representation of a physical memory frame
struct MemoryFrame {
    int frameId;
    int size;
    bool isAllocated;
    int allocatedPid;
};

class MemoryManager {
public:
    MemoryManager(int totalSize = 4096, int pageSize = 256);

    bool allocateMemory(int pid, int sizeNeeded);
    void deallocateMemory(int pid);
    QString getMemoryMap();

private:
    QList<MemoryFrame> frames; // Declaring 'frames' here
    QMutex memMutex;           // Declaring 'memMutex' here
};

#endif // MEMORYMANAGER_H