#include "systemmonitor.h"
#include <algorithm>
#include <cstdlib>

// OS-specific headers live safely isolated in this file now
#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <QFile>
#include <QTextStream>
#endif

SystemMonitor::SystemMonitor() {}

int SystemMonitor::getRealMemoryUsage() {
    int realMemoryPercent = 0;

#ifdef Q_OS_WIN
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    realMemoryPercent = memInfo.dwMemoryLoad;
#elif defined(Q_OS_LINUX)
    QFile file("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        long totalMem = 0, availableMem = 0;
        while (in.readLineInto(&line)) {
            if (line.startsWith("MemTotal:")) totalMem = line.section(' ', -2, -2).toLong();
            if (line.startsWith("MemAvailable:")) availableMem = line.section(' ', -2, -2).toLong();
        }
        file.close();
        if (totalMem > 0) realMemoryPercent = 100 - ((availableMem * 100) / totalMem);
    }
#endif

    return realMemoryPercent;
}

int SystemMonitor::getCpuUsage(bool isExecuting, bool isRunning) {
    if (!isExecuting) return 0;
    
    // CPU Logic: 80-95% if a job is actively running, 5-10% if idle
    int cpuBase = isRunning ? 80 : 5;
    return std::min(100, cpuBase + (rand() % 15));
}