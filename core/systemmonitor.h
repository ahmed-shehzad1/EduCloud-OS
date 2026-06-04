#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QtGlobal>

class SystemMonitor {
public:
    SystemMonitor();
    
    // Grabs actual RAM usage from the OS
    int getRealMemoryUsage();
    
    // Calculates CPU usage based on system state
    int getCpuUsage(bool isExecuting, bool isRunning);
};

#endif // SYSTEMMONITOR_H