#ifndef JOB_H
#define JOB_H

#include <QString>

enum class JobState {
    WAITING,
    RUNNING,
    COMPLETED
};

struct Job {
    int id;
    int priority;      
    int burstTime;     
    int remainingTime; 
    JobState state;    

    QString getStateString() const {
        switch (state) {
            case JobState::WAITING:   return "WAITING";
            case JobState::RUNNING:   return "RUNNING";
            case JobState::COMPLETED: return "COMPLETED";
        }
        return "UNKNOWN";
    }
};

#endif // JOB_H