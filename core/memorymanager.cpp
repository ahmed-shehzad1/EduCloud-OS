#include "memorymanager.h"
#include <QMutexLocker>

MemoryManager::MemoryManager(int totalSize, int pageSize) {
    int numFrames = totalSize / pageSize;
    for (int i = 0; i < numFrames; ++i) {
        frames.append({i, pageSize, false, -1});
    }
}

bool MemoryManager::allocateMemory(int pid, int sizeNeeded) {
    QMutexLocker locker(&memMutex);
    
    if (frames.isEmpty()) return false;
    
    int framesNeeded = (sizeNeeded + frames[0].size - 1) / frames[0].size;
    int freeFrames = 0;
    
    for (const auto& frame : frames) {
        if (!frame.isAllocated) freeFrames++;
    }
    
    if (freeFrames < framesNeeded) return false;
    
    int allocated = 0;
    for (auto& frame : frames) {
        if (!frame.isAllocated && allocated < framesNeeded) {
            frame.isAllocated = true;
            frame.allocatedPid = pid;
            allocated++;
        }
    }
    return true;
}

void MemoryManager::deallocateMemory(int pid) {
    QMutexLocker locker(&memMutex);
    for (auto& frame : frames) {
        if (frame.isAllocated && frame.allocatedPid == pid) {
            frame.isAllocated = false;
            frame.allocatedPid = -1;
        }
    }
}

QString MemoryManager::getMemoryMap() {
    QMutexLocker locker(&memMutex);
    QString map = "=== PHYSICAL MEMORY (RAM) FRAMES ===\n\n";
    
    int cols = 4; // Display 4 frames per row for a clean UI grid
    int count = 0;

    for (const auto& frame : frames) {
        if (frame.isAllocated) {
            map += QString("[ PID: %1 | %2KB ]  ").arg(frame.allocatedPid, 4).arg(frame.size);
        } else {
            map += QString("[ --- FREE --- | %1KB ]  ").arg(frame.size);
        }
        
        count++;
        if (count % cols == 0) {
            map += "\n";
        }
    }
    
    return map + "\n";
}