#include "scheduler.h"
#include <QMutexLocker>
#include <algorithm>
#include <cstdlib>

Scheduler::Scheduler() : 
    isRunning(true), 
    isExecuting(false), 
    timeQuantum(2), 
    nextJobId(2), 
    activeAlgorithm(Algorithm::FCFS),
    memManager(4096, 256), // 4096KB total RAM, 256KB page size
    bankers(3)             // 3 simulated resource types for Banker's
{}

Scheduler::~Scheduler() { stopScheduler(); }

void Scheduler::toggleExecution(bool start) {
    QMutexLocker locker(&queueMutex);
    isExecuting = start;
    if (isExecuting) {
        emit systemLog("\n▶️ [System] EXECUTION STARTED.");
        condition.wakeAll(); // Wake up the sleeping CPU
    } else {
        emit systemLog("\n⏸️ [System] EXECUTION PAUSED.");
    }
}

void Scheduler::setQuantum(int q) {
    QMutexLocker locker(&queueMutex);
    timeQuantum = q;
    emit systemLog(QString("[System] Time Quantum set to %1s").arg(q));
}

void Scheduler::addJob(int priority, int burstTime) {
    QMutexLocker locker(&queueMutex); 
    
    // SIMULATE MEMORY ALLOCATION (Requests 256KB to 1024KB)
    int memNeeded = 256 * ((rand() % 4) + 1); 
    if (!memManager.allocateMemory(nextJobId, memNeeded)) {
        emit systemLog(QString("❌ [Memory ERROR] RAM full! Cannot allocate Job %1.").arg(nextJobId));
        return; 
    }

    Job newJob = { nextJobId++, priority, burstTime, burstTime, JobState::WAITING };
    jobQueue.append(newJob);
    
    emit systemLog(QString("[Staged] Job %1 added (Allocated %2KB).").arg(newJob.id).arg(memNeeded));
    
    condition.wakeAll(); // Wake the thread to check if it should run
    locker.unlock(); 
    broadcastQueue();
}

void Scheduler::setAlgorithm(Algorithm algo) {
    QMutexLocker locker(&queueMutex);
    activeAlgorithm = algo;
    
    QString algoName = "FCFS";
    if (algo == Algorithm::PRIORITY) algoName = "Priority";
    if (algo == Algorithm::SJF) algoName = "Shortest Job First (SJF)";
    if (algo == Algorithm::RR) algoName = "Round Robin (RR)";
    
    emit systemLog("[System] Switched policy to: " + algoName);
    locker.unlock();
    broadcastQueue();
}

void Scheduler::broadcastQueue() {
    QMutexLocker locker(&queueMutex);
    QString qData = "--- CURRENT JOB PIPELINE ---\n";
    
    if (jobQueue.isEmpty()) {
        qData += "Queue is empty.\n";
    } else {
        for (const auto& job : jobQueue) {
            qData += QString("Job ID: %1 | Pri: %2 | Burst: %3s | Rem: %4s | State: %5\n")
                     .arg(job.id).arg(job.priority).arg(job.burstTime)
                     .arg(job.remainingTime).arg(job.getStateString());
        }
    }
    
    // APPEND OS SUBSYSTEM STATES TO THE UI
    qData += "\n" + processTable.getTableSnapshot() + "\n";
    qData += memManager.getMemoryMap();
    
    emit queueUpdate(qData);
}

void Scheduler::sortQueue() {
    if (jobQueue.isEmpty()) return;
    
    if (activeAlgorithm == Algorithm::PRIORITY) {
        std::sort(jobQueue.begin(), jobQueue.end(), [](const Job& a, const Job& b) {
            if (a.state != JobState::WAITING) return true; 
            if (b.state != JobState::WAITING) return false;
            return a.priority > b.priority; 
        });
    } else if (activeAlgorithm == Algorithm::SJF) {
        std::sort(jobQueue.begin(), jobQueue.end(), [](const Job& a, const Job& b) {
            if (a.state != JobState::WAITING) return true; 
            if (b.state != JobState::WAITING) return false;
            return a.burstTime < b.burstTime; 
        });
    }
}

void Scheduler::run() {
    emit systemLog("[Engine Thread] Core Scheduler Engine online. Awaiting Start command...");

    while (isRunning) {
        Job* jobToExecute = nullptr;

        {
            QMutexLocker locker(&queueMutex);
            
            while (isRunning) {
                if (!isExecuting) {
                    condition.wait(&queueMutex);
                    continue; 
                }

                sortQueue();
                for (auto& job : jobQueue) {
                    if (job.state == JobState::WAITING) {
                        jobToExecute = &job;
                        break;
                    }
                }

                if (jobToExecute) break; 
                condition.wait(&queueMutex); 
            }
            if (!isRunning) return;
        }

        if (jobToExecute) {
            // FIRST TIME RUNNING: Spawn the process!
            if (jobToExecute->remainingTime == jobToExecute->burstTime) {
                // Simulate parent dynamics: Evens are children of 1. Odds are children of the previous ID.
                int parentPid = (jobToExecute->id % 2 == 0) ? 1 : std::max(1, jobToExecute->id - 1);
                processTable.spawnProcess(jobToExecute->id, parentPid);
            }

            jobToExecute->state = JobState::RUNNING;
            emit systemLog(QString(">>> [CPU Worker] Dispatching Job %1...").arg(jobToExecute->id));
            broadcastQueue();

            int maxRunTime = (activeAlgorithm == Algorithm::RR) ? timeQuantum : jobToExecute->remainingTime;
            int timeRun = 0;

            while (jobToExecute->remainingTime > 0 && timeRun < maxRunTime && isExecuting) {
                QThread::sleep(1); 
                
                QMutexLocker locker(&queueMutex);
                jobToExecute->remainingTime--;
                timeRun++;
                emit systemLog(QString(" -> Job %1 running. %2s remaining.").arg(jobToExecute->id).arg(jobToExecute->remainingTime));
                
                locker.unlock();
                broadcastQueue();
            }

            QMutexLocker locker(&queueMutex);
            if (jobToExecute->remainingTime == 0) {
                jobToExecute->state = JobState::COMPLETED;
                
                // TASK ENDED: Kill process and free RAM
                processTable.completeProcess(jobToExecute->id);
                memManager.deallocateMemory(jobToExecute->id);
                
                emit systemLog(QString("<<< [CPU Worker] Finished Job %1 Execution!\n").arg(jobToExecute->id));
            } else if (isExecuting) {
                // Only preempt if we ran out of time slice
                jobToExecute->state = JobState::WAITING;
                emit systemLog(QString("||| [CPU Worker] Job %1 Preempted.\n").arg(jobToExecute->id));
                
                int targetId = jobToExecute->id;
                for (int i = 0; i < jobQueue.size(); ++i) {
                    if (jobQueue[i].id == targetId) {
                        Job preemptedJob = jobQueue[i]; 
                        jobQueue.removeAt(i);           
                        jobQueue.append(preemptedJob);  
                        break;
                    }
                }
            } else {
                // If paused mid-execution, set back to waiting
                jobToExecute->state = JobState::WAITING;
            }
            locker.unlock();
            broadcastQueue();
        }
    }
}

void Scheduler::stopScheduler() {
    {
        QMutexLocker locker(&queueMutex);
        isRunning = false;
        condition.wakeAll();
    }
    wait(); 
}