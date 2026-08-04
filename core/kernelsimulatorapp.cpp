#include "kernelsimulatorapp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

// ================= WORKER THREAD (MUTEX & SHARED MEMORY SIMULATOR) =================
WorkerThread::WorkerThread(int id, QMutex *mutex, int *sharedData, int *pipeBuffer, int *pipeSize, QObject *parent)
    : QThread(parent), m_id(id), m_mutex(mutex), m_sharedData(sharedData), m_pipeBuffer(pipeBuffer), m_pipeSize(pipeSize), m_running(true) {}

void WorkerThread::run() {
    while (m_running) {
        QThread::msleep(1500 + (rand() % 1000)); // Sleep randomized interval
        
        emit logMessage(QString("⏱️ [Thread %1] Attempting to enter Critical Section...").arg(m_id));
        
        // MUTEX LOCK ACQUISITION
        m_mutex->lock(); 
        emit logMessage(QString("🔒 [Mutex LOCKED] Thread %1 gained exclusive access.").arg(m_id));
        
        // Modifying SHARED MEMORY safely
        *m_sharedData += (m_id == 1) ? 10 : -5;
        if (*m_sharedData > 100) *m_sharedData = 20;
        if (*m_sharedData < 0) *m_sharedData = 80;
        
        emit updateSharedMem(*m_sharedData);
        emit logMessage(QString("💾 [Shared Memory] Thread %1 modified value to: %2%").arg(m_id).arg(*m_sharedData));
        
        QThread::sleep(1); // Hold the lock for 1 second to make it visually obvious
        
        m_mutex->unlock(); // MUTEX RELEASE
        emit logMessage(QString("🔓 [Mutex UNLOCKED] Thread %1 exited Critical Section.").arg(m_id));
    }
}

void WorkerThread::stop() { m_running = false; }


// ================= MAIN KERNEL DIAGNOSTIC DASHBOARD =================
KernelSimulatorApp::KernelSimulatorApp(QWidget *parent) : QWidget(parent), sharedMemoryBlock(50), kernelPipeBuffer(0) {
    // Styling the Diagnostic Dashboard Terminal Look
    this->setStyleSheet("background-color: #0f141c; color: #e1e6f0; font-family: 'Consolas', monospace;");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *header = new QLabel("🚨 OS Subsystem Core Telemetry Simulator");
    header->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffb454; padding-bottom: 5px;");
    mainLayout->addWidget(header);

    // ---- ROW 1: IPC PIPES & MUTEX VISUALIZERS ----
    QHBoxLayout *row1 = new QHBoxLayout();
    
    // Pipe Box Layout
    QGroupBox *pipeBox = new QGroupBox("1. Bounded Buffer Kernel Pipe (IPC)");
    pipeBox->setStyleSheet("color: #59c2c9; font-weight: bold;");
    QVBoxLayout *pLayout = new QVBoxLayout(pipeBox);
    pipeProgressBar = new QProgressBar();
    pipeProgressBar->setRange(0, PIPE_MAX_CAPACITY);
    pipeProgressBar->setValue(0);
    pipeProgressBar->setFormat("%v / 20 Bytes");
    pipeProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #39bae6; }");
    QPushButton *btnProduce = new QPushButton("Write Byte to Pipe (Produce)");
    QPushButton *btnConsume = new QPushButton("Read Byte from Pipe (Consume)");
    btnProduce->setStyleSheet("background-color: #1c2331; border: 1px solid #39bae6; padding: 4px;");
    btnConsume->setStyleSheet("background-color: #1c2331; border: 1px solid #39bae6; padding: 4px;");
    pLayout->addWidget(new QLabel("Pipe Internal Buffer Allocation Size:"));
    pLayout->addWidget(pipeProgressBar);
    pLayout->addWidget(btnProduce);
    pLayout->addWidget(btnConsume);
    row1->addWidget(pipeBox);

    // Shared Memory & Mutex Layout Box
    QGroupBox *mutexBox = new QGroupBox("2. Thread Synchronization (Mutex & Shared RAM)");
    mutexBox->setStyleSheet("color: #ff7733; font-weight: bold;");
    QVBoxLayout *mLayout = new QVBoxLayout(mutexBox);
    sharedMemBar = new QProgressBar();
    sharedMemBar->setRange(0, 100);
    sharedMemBar->setValue(sharedMemoryBlock);
    sharedMemBar->setStyleSheet("QProgressBar::chunk { background-color: #ff7733; }");
    mLayout->addWidget(new QLabel("Atomic Shared Register State:"));
    mLayout->addWidget(sharedMemBar);
    mLayout->addWidget(new QLabel("Status: Thread 1 (+) & Thread 2 (-) racing under Mutex protection."));
    row1->addWidget(mutexBox);

    mainLayout->addLayout(row1);

    // ---- ROW 2: BANKER'S ALGORITHM DEADLOCK PREVENTION ----
    QGroupBox *bankersBox = new QGroupBox("3. Deadlock Avoidance Subsystem (Banker's Matrix Evaluation)");
    bankersBox->setStyleSheet("color: #aad94c; font-weight: bold;");
    QVBoxLayout *bLayout = new QVBoxLayout(bankersBox);
    QLabel *matrixDetails = new QLabel("Processes: P0, P1, P2 | Resources Types: [R0, R1, R2]\nAvailable Vector: [3, 3, 2]");
    matrixDetails->setStyleSheet("color: #b3b1ad; font-weight: normal;");
    QPushButton *btnCheckBankers = new QPushButton("Execute Kernel Deadlock Safety Check Scan");
    btnCheckBankers->setStyleSheet("background-color: #1c2331; border: 1px solid #aad94c; padding: 6px; font-weight: bold; color: #aad94c;");
    bLayout->addWidget(matrixDetails);
    bLayout->addWidget(btnCheckBankers);
    mainLayout->addWidget(bankersBox);

    // ---- ROW 3: SYSTEM CONSOLE LOG MONITOR ----
    mainLayout->addWidget(new QLabel("<b>Kernel Activity Event Log Output</b>"));
    consoleLog = new QTextEdit();
    consoleLog->setReadOnly(true);
    consoleLog->setStyleSheet("background-color: #161b24; border: 1px solid #232a36; color: #73d0ff; font-size: 11px;");
    mainLayout->addWidget(consoleLog);

    // Event Triggers Wiring Connections
    connect(btnProduce, &QPushButton::clicked, this, &KernelSimulatorApp::writeToPipe);
    connect(btnConsume, &QPushButton::clicked, this, &KernelSimulatorApp::readFromPipe);
    connect(btnCheckBankers, &QPushButton::clicked, this, &KernelSimulatorApp::runBankersCheck);

    // Initialize the Banker's Matrix Values
    initializeBankersData();

    // Spawn the background competitive thread processing units
    threadA = new WorkerThread(1, &structuralMutex, &sharedMemoryBlock, &kernelPipeBuffer, const_cast<int*>(&PIPE_MAX_CAPACITY), this);
    threadB = new WorkerThread(2, &structuralMutex, &sharedMemoryBlock, &kernelPipeBuffer, const_cast<int*>(&PIPE_MAX_CAPACITY), this);

    connect(threadA, &WorkerThread::logMessage, this, &KernelSimulatorApp::appendLog);
    connect(threadB, &WorkerThread::logMessage, this, &KernelSimulatorApp::appendLog);
    connect(threadA, &WorkerThread::updateSharedMem, this, &KernelSimulatorApp::handleSharedMemUpdate);
    connect(threadB, &WorkerThread::updateSharedMem, this, &KernelSimulatorApp::handleSharedMemUpdate);

    threadA->start();
    threadB->start();
}

KernelSimulatorApp::~KernelSimulatorApp() {
    threadA->stop();
    threadB->stop();
    threadA->wait();
    threadB->wait();
}

// ================= IPC PIPE SIMULATION LOGIC =================
void KernelSimulatorApp::writeToPipe() {
    QMutexLocker locker(&structuralMutex); // Prevent concurrent vector manipulation conflicts
    if (kernelPipeBuffer < PIPE_MAX_CAPACITY) {
        kernelPipeBuffer++;
        appendLog(QString("💾 [PIPE WRITE] Process 0 wrote 1 Byte to Kernel Pipe. Buffer: %1 Bytes.").arg(kernelPipeBuffer));
        handlePipeUpdate();
    } else {
        appendLog("⚠️ [PIPE FULL] Bounded Buffer full! Producer block-waiting on Semaphore empty count.");
    }
}

void KernelSimulatorApp::readFromPipe() {
    QMutexLocker locker(&structuralMutex);
    if (kernelPipeBuffer > 0) {
        kernelPipeBuffer--;
        appendLog(QString("📥 [PIPE READ] Process 1 read 1 Byte from Kernel Pipe. Buffer: %1 Bytes.").arg(kernelPipeBuffer));
        handlePipeUpdate();
    } else {
        appendLog("⚠️ [PIPE EMPTY] Bounded Buffer dry! Consumer block-waiting on Semaphore full count.");
    }
}

void KernelSimulatorApp::handlePipeUpdate() {
    pipeProgressBar->setValue(kernelPipeBuffer);
}

void KernelSimulatorApp::handleSharedMemUpdate(int value) {
    sharedMemBar->setValue(value);
}

void KernelSimulatorApp::appendLog(QString msg) {
    consoleLog->append(msg);
}

// ================= BANKER'S ALGORITHM PARADIGM =================


void KernelSimulatorApp::initializeBankersData() {
    available = {3, 3, 2}; // System Available Stock Vector

    maxMatrix = {
        {7, 5, 3}, // P0 Max
        {3, 2, 2}, // P1 Max
        {9, 0, 2}  // P2 Max
    };

    allocationMatrix = {
        {0, 1, 0}, // P0 currently holding
        {2, 0, 0}, // P1 currently holding
        {3, 0, 2}  // P2 currently holding
    };

    // Matrix Formula: Need[i][j] = Max[i][j] - Allocation[i][j]
    needMatrix.resize(numProcesses, QVector<int>(numResources));
    for (int i = 0; i < numProcesses; i++) {
        for (int j = 0; j < numResources; j++) {
            needMatrix[i][j] = maxMatrix[i][j] - allocationMatrix[i][j];
        }
    }
}

void KernelSimulatorApp::runBankersCheck() {
    appendLog("\n🔎 [Banker's Algorithm] Initiating Kernel Resource Safety Matrix Evaluation Scan...");
    
    QVector<int> work = available;
    QVector<bool> finish(numProcesses, false);
    QVector<int> safeSequence;

    int count = 0;
    while (count < numProcesses) {
        bool foundProcess = false;
        
        for (int p = 0; p < numProcesses; p++) {
            if (!finish[p]) {
                int j;
                // Verify if Need <= Work Vector
                for (j = 0; j < numResources; j++) {
                    if (needMatrix[p][j] > work[j]) break;
                }

                if (j == numResources) { // Condition met: Process resource requests can be satisfied safely
                    for (int k = 0; k < numResources; k++) {
                        work[k] += allocationMatrix[p][k]; // Deallocate resources to pool upon simulation finish
                    }
                    safeSequence.append(p);
                    finish[p] = true;
                    foundProcess = true;
                    count++;
                    appendLog(QString("  ✔ Process P%1 successfully parsed inside simulated execution chain.").arg(p));
                }
            }
        }

        if (!foundProcess) { // Deadlock Detected or Path Unsafe
            break;
        }
    }

    if (count == numProcesses) {
        QString seqStr = "";
        for(int i : safeSequence) seqStr += QString("P%1 -> ").arg(i);
        seqStr.chop(4);
        appendLog(QString("🟢 [SYSTEM STATE: SAFE] Deadlock avoided. Valid Scheduling Execution Path: <%1>").arg(seqStr));
    } else {
        appendLog("🔴 [SYSTEM STATE: UNSAFE] Resource Allocation Denied! Granting request would trigger cyclic resource blocking (Deadlock).");
    }
}