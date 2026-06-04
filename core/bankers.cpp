#include "bankers.h"

BankersAlgorithm::BankersAlgorithm(int numResources) : numRes(numResources) {}

void BankersAlgorithm::setAvailable(QVector<int> available) {
    avail = available;
}

void BankersAlgorithm::addProcess(int pid, QVector<int> max, QVector<int> allocation) {
    pids.append(pid);
    maxMatrix.append(max);
    allocMatrix.append(allocation);
}

void BankersAlgorithm::calculateNeed() {
    needMatrix.clear();
    for (int i = 0; i < pids.size(); i++) {
        QVector<int> needRow;
        for (int j = 0; j < numRes; j++) {
            needRow.append(maxMatrix[i][j] - allocMatrix[i][j]);
        }
        needMatrix.append(needRow);
    }
}

bool BankersAlgorithm::isSafeState() {
    calculateNeed();
    safeSequence.clear();
    
    QVector<int> work = avail;
    QVector<bool> finish(pids.size(), false);
    
    int count = 0;
    while (count < pids.size()) {
        bool found = false;
        for (int p = 0; p < pids.size(); p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < numRes; j++) {
                    if (needMatrix[p][j] > work[j]) break;
                }
                
                if (j == numRes) { // Process can be allocated
                    for (int k = 0; k < numRes; k++) {
                        work[k] += allocMatrix[p][k];
                    }
                    safeSequence.append(pids[p]);
                    finish[p] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) return false; // System is in Deadlock!
    }
    return true; // System is Safe
}

QString BankersAlgorithm::getExecutionSequence() {
    if (safeSequence.isEmpty()) return "UNSAFE STATE DETECTED - DEADLOCK IMMINENT";
    
    QString seq = "SAFE SEQUENCE: ";
    for (int pid : safeSequence) {
        seq += QString("P%1 -> ").arg(pid);
    }
    return seq.left(seq.length() - 4); // Remove trailing arrow
}