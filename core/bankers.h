#ifndef BANKERS_H
#define BANKERS_H

#include <QVector>
#include <QString>

class BankersAlgorithm {
public:
    BankersAlgorithm(int numResources);

    void setAvailable(QVector<int> available);
    void addProcess(int pid, QVector<int> max, QVector<int> allocation);
    
    bool isSafeState();
    QString getExecutionSequence();

private:
    int numRes;
    QVector<int> avail;
    QVector<int> pids;
    QVector<QVector<int>> maxMatrix;
    QVector<QVector<int>> allocMatrix;
    QVector<QVector<int>> needMatrix;
    
    QVector<int> safeSequence;
    
    void calculateNeed();
};

#endif // BANKERS_H