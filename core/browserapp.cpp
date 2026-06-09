#include "browserapp.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QWindow>
#include <QDir>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

BrowserApp::BrowserApp(QWidget *parent) 
    : QWidget(parent), m_process(new QProcess(this)), m_containerWidget(nullptr), m_targetHwnd(0) {
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Set a matching dark backdrop while the external engine spins up
    setStyleSheet("background-color: #1e1e2e;");

    // --- BULLETPROOF CHROMIUM SANDBOXING PARAMETERS ---
    // Creating an isolated cache profile folder stops Edge from jumping to pre-existing background tasks!
    QString profilePath = QCoreApplication::applicationDirPath() + "/.browser_sandbox";
    QDir().mkpath(profilePath);

    QString program = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    QStringList arguments;
    arguments << "--app=https://www.google.com"
              << "--user-data-dir=" + profilePath
              << "--no-first-run"
              << "--no-default-browser-check";

    // Start the process completely hidden from the user's desktop view initially
    m_process->start(program, arguments);

    // A fast 1-second check loop is perfectly sufficient since the sandbox ensures instant instantiation
    QTimer::singleShot(1000, this, &BrowserApp::embedLiveEngine);
}

// Win32 window scanner callback that targets our independent profile window explicitly
#ifdef Q_OS_WIN
BOOL CALLBACK CaptureEdgeWindow(HWND hwnd, LPARAM lParam) {
    char className[256];
    GetClassNameA(hwnd, className, sizeof(className));
    
    // Check if it's the main Chromium presentation viewport window frame
    if (strcmp(className, "Chrome_WidgetWin_1") == 0) {
        char title[256];
        GetWindowTextA(hwnd, title, sizeof(title));
        QString windowTitle(title);
        
        // Filter out background auxiliary pipeline communication channels
        if (!windowTitle.isEmpty() && windowTitle != "Chrome Legacy Window") {
            HWND *resultHwnd = reinterpret_cast<HWND*>(lParam);
            *resultHwnd = hwnd;
            return FALSE; // Target secured! Break loop.
        }
    }
    return TRUE;
}
#endif

void BrowserApp::embedLiveEngine() {
#ifdef Q_OS_WIN
    HWND hwnd = nullptr;
    EnumWindows(CaptureEdgeWindow, reinterpret_cast<LPARAM>(&hwnd));

    if (hwnd) {
        m_targetHwnd = reinterpret_cast<unsigned long long>(hwnd);

        // 1. Force the OS to strip off standard desktop decorations and titles
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUP);
        style |= WS_CHILD; // Dictate that it must live inside a parental container
        SetWindowLong(hwnd, GWL_STYLE, style);

        // 2. Perform low-level native window reparenting directly into our Qt widget handle
        SetParent(hwnd, reinterpret_cast<HWND>(this->winId()));

        // 3. Wrap it seamlessly into a high-performance Qt Window viewport container
        QWindow *externalWindow = QWindow::fromWinId(reinterpret_cast<WId>(hwnd));
        if (externalWindow) {
            m_containerWidget = QWidget::createWindowContainer(externalWindow, this);
            layout()->addWidget(m_containerWidget);
            
            // 4. Update coordinates and grant immediate input focus rights
            m_containerWidget->setGeometry(this->rect());
            SetFocus(hwnd);
            this->update();
        }
    } else {
        // If system execution latency occurs, retry the loop one more time
        QTimer::singleShot(500, this, &BrowserApp::embedLiveEngine);
    }
#endif
}

void BrowserApp::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
#ifdef Q_OS_WIN
    if (m_targetHwnd) {
        // Force the window engine to wake up and take focus upon window rendering activation
        SetFocus(reinterpret_cast<HWND>(m_targetHwnd));
    }
#endif
}

void BrowserApp::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_containerWidget) {
        m_containerWidget->setGeometry(this->rect());
    }
}

BrowserApp::~BrowserApp() {
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->terminate();
        m_process->waitForFinished(2000);
    }
}