#ifndef KERNELSIMULATORAPP_H
#define KERNELSIMULATORAPP_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMutex>
#include <QThread>
#include <QVector>

// Simple worker thread to simulate multi-threaded competition for the Mutex & Shared Memory
class WorkerThread : public QThread {
    Q_OBJECT
public:
    WorkerThread(int id, QMutex *mutex, int *sharedData, int *pipeBuffer, int *pipeSize, QObject *parent = nullptr);
    void stop();

signals:
    void logMessage(QString msg);
    void updateSharedMem(int value);
    void pipeUpdated();

protected:
    void run() override;

private:
    int m_id;
    QMutex *m_mutex;
    int *m_sharedData;
    int *m_pipeBuffer;
    int *m_pipeSize;
    bool m_running;
};

// Main Simulation Widget View
class KernelSimulatorApp : public QWidget {
    Q_OBJECT
public:
    explicit KernelSimulatorApp(QWidget *parent = nullptr);
    ~KernelSimulatorApp();

private slots:
    void runBankersCheck();
    void writeToPipe();
    void readFromPipe();
    void handlePipeUpdate();
    void handleSharedMemUpdate(int value);
    void appendLog(QString msg);

private:
    // UI Layout Components
    QTextEdit *consoleLog;
    QProgressBar *pipeProgressBar;
    QProgressBar *sharedMemBar;
    
    // Mutex & Shared Memory Infrastructure
    QMutex structuralMutex;
    int sharedMemoryBlock;
    WorkerThread *threadA;
    WorkerThread *threadB;

    // Pipe Infrastructure (IPC)
    int kernelPipeBuffer; 
    const int PIPE_MAX_CAPACITY = 20;

    // Banker's Algorithm Data Matrices
    int numProcesses = 3;
    int numResources = 3;
    QVector<int> available;
    QVector<QVector<int>> maxMatrix;
    QVector<QVector<int>> allocationMatrix;
    QVector<QVector<int>> needMatrix;

    void initializeBankersData();
};

#endif // KERNELSIMULATORAPP_H