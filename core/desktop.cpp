#include "desktop.h"
#include "mainwindow.h" 
#include "taskmanagerapp.h"
#include "filesystemapp.h"
#include "calculatorapp.h"
#include "snakegameapp.h"
#include "browserapp.h"
#include "contextmenumanager.h"

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QScreen>
#include <QInputDialog>
#include <QSettings>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QFileInfo>

Desktop::Desktop(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("EduCloud OS");
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        this->setGeometry(screen->geometry());
    }
    setWindowState(Qt::WindowFullScreen);
    
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground);

    QSettings displaySettings("EduCloudOS", "Display");
    m_darkMode = displaySettings.value("darkMode", true).toBool();

    workspace = new QMdiArea(this);
    workspace->setViewMode(QMdiArea::SubWindowView);
    
    // FIX 1: Stop the MDI workspace viewport from over-painting your custom desktop background
    workspace->setBackground(Qt::NoBrush);
    workspace->viewport()->setAutoFillBackground(false);
    workspace->setStyleSheet("QMdiArea { background: transparent; border: none; }");

    new ContextMenuManager(this, workspace);

    setupDesktopEnvironment();
    setupTaskbar();
    setupStartMenu();
    setupSystemTray();

    masterFileSystem = new QMap<QString, QString>();

    systemTimer = new QTimer(this);
    connect(systemTimer, &QTimer::timeout, this, &Desktop::updateClock);
    systemTimer->start(1000);
    updateClock();

    applyGlobalStyle();
}

Desktop::~Desktop() {
    QSettings displaySettings("EduCloudOS", "Display");
    displaySettings.setValue("darkMode", m_darkMode);
    
    if (masterFileSystem) {
        delete masterFileSystem;
    }
}

void Desktop::openSecuritySettings() {
    QSettings settings("EduCloudOS", "SecuritySettings");
    bool currentStatus = settings.value("loginEnabled", false).toBool();
    QString statusText = currentStatus ? "ENABLED" : "DISABLED";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("🔐 Security Configuration");
    msgBox.setText(QString("Lock Screen: <b>%1</b>\n\nWhat would you like to do?").arg(statusText));
    
    QPushButton *toggleBtn = msgBox.addButton(currentStatus ? "Disable" : "Enable", QMessageBox::ActionRole);
    msgBox.addButton(QMessageBox::Cancel);
    
    msgBox.exec();

    if (msgBox.clickedButton() == toggleBtn) {
        if (!currentStatus) {
            bool ok;
            QString newPassword = QInputDialog::getText(this, "Set Password", "Create password:", QLineEdit::Password, "", &ok);
            if (ok && !newPassword.isEmpty()) {
                settings.setValue("loginEnabled", true);
                settings.setValue("systemPassword", newPassword);
                QMessageBox::information(this, "Success", "Password enabled!");
            }
        } else {
            bool ok;
            QString checkPassword = QInputDialog::getText(this, "Disable", "Enter password:", QLineEdit::Password, "", &ok);
            if (ok && checkPassword == settings.value("systemPassword", "admin123").toString()) {
                settings.setValue("loginEnabled", false);
                QMessageBox::information(this, "Success", "Password disabled!");
            } else if (ok) {
                QMessageBox::critical(this, "Error", "Wrong password!");
            }
        }
    }
}

void Desktop::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw background gradient
    QLinearGradient gradient(0, 0, 0, this->height());
    
    if (m_darkMode) {
        gradient.setColorAt(0.0, QColor(44, 0, 30));
        gradient.setColorAt(0.5, QColor(79, 25, 63));
        gradient.setColorAt(1.0, QColor(30, 30, 46));
    } else {
        gradient.setColorAt(0.0, QColor(220, 200, 210));
        gradient.setColorAt(0.5, QColor(200, 160, 190));
        gradient.setColorAt(1.0, QColor(240, 220, 230));
    }
    
    painter.fillRect(this->rect(), gradient);
    
    // Draw large watermark/logo in background
    painter.setOpacity(0.12);
    painter.setFont(QFont("Arial", 200, QFont::Bold));
    painter.setPen(QPen(QColor(255, 255, 255)));
    painter.drawText(this->rect(), Qt::AlignCenter, "☁️");
    painter.setOpacity(1.0);
    
}

void Desktop::setupDesktopEnvironment() {
    QLabel *osLogo = new QLabel(workspace);
    osLogo->setText("☁️\nEduCloud OS");
    // Note: Adjusted opacity slightly from 0.08 to 0.15 so it's visible but subtle
    osLogo->setStyleSheet("QLabel { color: rgba(205, 214, 244, 0.15); font-size: 60px; font-weight: bold; border: none; background: transparent; }");
    osLogo->setAlignment(Qt::AlignCenter);
    
    QScreen *screen = QGuiApplication::primaryScreen();
    int screenW = screen->geometry().width();
    int screenH = screen->geometry().height();
    osLogo->setGeometry((screenW - 500) / 2, (screenH - 250) / 2 - 50, 500, 250);
    osLogo->lower();

    createShortcut("⚙️", "Task Orchestrator", 40, 40, &Desktop::launchSchedulerApp);
    createShortcut("📊", "Task Manager", 40, 160, &Desktop::launchTaskManagerApp);
    createShortcut("📁", "File System", 40, 280, &Desktop::launchFileSystemApp);
    createShortcut("🧮", "Calculator", 40, 400, &Desktop::launchCalculatorApp);
    createShortcut("🐍", "Snake Game", 40, 520, &Desktop::launchSnakeApp);
    createShortcut("🌐", "Web Browser", 40, 640, &Desktop::launchBrowserApp);
}

void Desktop::createShortcut(QString iconText, QString name, int x, int y, void (Desktop::*slotFunction)()) {
    QPushButton *btn = new QPushButton(iconText, workspace);
    btn->resize(70, 70);
    btn->move(x, y);
    btn->setStyleSheet("QPushButton { background-color: rgba(49, 50, 68, 0.8); color: white; font-size: 35px; border-radius: 12px; border: 1px solid #45475a; } QPushButton:hover { background-color: rgba(137, 180, 250, 0.8); }");
    connect(btn, &QPushButton::clicked, this, slotFunction);

    QLabel *label = new QLabel(name, workspace);
    label->setStyleSheet("QLabel { color: #cdd6f4; font-size: 10px; background: transparent; border: none; }");
    label->setFixedWidth(130);
    label->setAlignment(Qt::AlignCenter);
    label->move(x - 30, y + 75);
}

void Desktop::setupTaskbar() {
    taskbar = new QWidget(this);
    taskbar->setFixedHeight(50);
    taskbar->setStyleSheet("background-color: rgba(17, 17, 27, 0.95); border-top: 2px solid #89b4fa;");

    QHBoxLayout *taskbarLayout = new QHBoxLayout(taskbar);
    taskbarLayout->setContentsMargins(10, 0, 10, 0);
    taskbarLayout->setSpacing(5);

    startBtn = new QPushButton("☁️ Start", this);
    startBtn->setFixedSize(90, 35);
    startBtn->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; border-radius: 5px; border: none; } QPushButton:hover { background-color: #b4befe; }");
    connect(startBtn, &QPushButton::clicked, this, &Desktop::toggleStartMenu);
    
    clockLabel = new QLabel("00:00:00", this);
    clockLabel->setStyleSheet("QLabel { color: #cdd6f4; font-weight: bold; font-size: 13px; border: none; background: transparent; }");
    
    QPushButton *themeBtn = new QPushButton("🌙", this);
    themeBtn->setFixedSize(35, 35);
    themeBtn->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; border-radius: 5px; border: none; } QPushButton:hover { background-color: #b4befe; }");
    connect(themeBtn, &QPushButton::clicked, this, &Desktop::toggleTheme);
    
    shutdownBtn = new QPushButton("🛑 Shutdown", this);
    shutdownBtn->setFixedSize(90, 35);
    shutdownBtn->setStyleSheet("QPushButton { background-color: #f38ba8; color: #11111b; font-weight: bold; border-radius: 5px; border: none; } QPushButton:hover { background-color: #eba0ac; }");
    connect(shutdownBtn, &QPushButton::clicked, this, &Desktop::shutdownOS);

    taskbarLayout->addWidget(startBtn);
    taskbarLayout->addWidget(themeBtn);
    taskbarLayout->addStretch();
    taskbarLayout->addWidget(clockLabel);
    taskbarLayout->addWidget(shutdownBtn);

    QWidget *centralContainer = new QWidget(this);
    centralContainer->setStyleSheet("background: transparent;"); 
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    mainLayout->addWidget(workspace, 1);
    mainLayout->addWidget(taskbar, 0);
    
    setCentralWidget(centralContainer);
}

void Desktop::setupStartMenu() {
    startMenuWidget = new QFrame(this);
    startMenuWidget->setFixedSize(220, 320);
    startMenuWidget->setStyleSheet("QFrame { background-color: rgba(30, 30, 46, 0.95); border: 1px solid #89b4fa; border-radius: 8px; }");
    startMenuWidget->hide();

    QVBoxLayout *menuLayout = new QVBoxLayout(startMenuWidget);
    menuLayout->setContentsMargins(10, 10, 10, 10);
    menuLayout->setSpacing(2);

    QLabel *userLabel = new QLabel("👤 Admin", startMenuWidget);
    userLabel->setStyleSheet("QLabel { color: #cdd6f4; font-size: 13px; font-weight: bold; border: none; background: transparent; }");
    menuLayout->addWidget(userLabel);
    menuLayout->addSpacing(5);

    auto addMenuBtn = [this, menuLayout](QString text, auto slotFunc) {
        QPushButton *btn = new QPushButton(text, startMenuWidget);
        btn->setStyleSheet("QPushButton { text-align: left; padding: 8px; background: transparent; color: #cdd6f4; font-size: 11px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #45475a; }");
        connect(btn, &QPushButton::clicked, this, slotFunc);
        connect(btn, &QPushButton::clicked, this, &Desktop::toggleStartMenu);
        menuLayout->addWidget(btn);
    };

    addMenuBtn("⚙️ Task Orchestrator", &Desktop::launchSchedulerApp);
    addMenuBtn("📊 Task Manager", &Desktop::launchTaskManagerApp);
    addMenuBtn("📁 File Explorer", &Desktop::launchFileSystemApp);
    addMenuBtn("🧮 Calculator", &Desktop::launchCalculatorApp);
    addMenuBtn("🐍 Snake Game", &Desktop::launchSnakeApp);
    addMenuBtn("🌐 Browser", &Desktop::launchBrowserApp);
    addMenuBtn("🔐 Security", &Desktop::openSecuritySettings);
    addMenuBtn("🔄 Refresh", &Desktop::refreshSystem);

    menuLayout->addStretch();
}

void Desktop::toggleStartMenu() {
    if (startMenuWidget->isHidden()) {
        QPoint globalPos = startBtn->mapToGlobal(QPoint(0, 0));
        QPoint localPos = this->mapFromGlobal(globalPos);
        startMenuWidget->move(localPos.x(), localPos.y() - startMenuWidget->height() - 5);
        startMenuWidget->raise();
        startMenuWidget->show();
    } else {
        startMenuWidget->hide();
    }
}

void Desktop::mousePressEvent(QMouseEvent *event) {
    if (!startMenuWidget->isHidden()) {
        QRect menuRect = startMenuWidget->geometry();
        QRect btnRect = startBtn->geometry();
        
        if (!menuRect.contains(event->pos()) && !btnRect.contains(event->pos())) {
            startMenuWidget->hide();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void Desktop::launchSchedulerApp() {
    MainWindow *app = new MainWindow();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Task Orchestrator");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(1000, 700);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
}

void Desktop::launchTaskManagerApp() {
    TaskManagerApp *app = new TaskManagerApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Task Manager");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(450, 400);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
}

void Desktop::launchFileSystemApp() {
    FileSystemApp *app = new FileSystemApp(masterFileSystem);
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("File Explorer");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(650, 450);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
}

void Desktop::launchCalculatorApp() {
    CalculatorApp *app = new CalculatorApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Calculator");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(250, 320);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
}

void Desktop::launchSnakeApp() {
    // Bring window to focus if it's already active
    if (m_currentSnakeWindow != nullptr && m_snakeSubWindow != nullptr) {
        m_snakeSubWindow->raise();
        workspace->setActiveSubWindow(m_snakeSubWindow);
        return;
    }

    SnakeGameApp *app = new SnakeGameApp();
    app->setWindowFlags(Qt::Widget);
    
    QWidget *container = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);
    layout->setAlignment(Qt::AlignTop);
    
    layout->addWidget(app, 1);
    
    QPushButton *resetBtn = new QPushButton("🔄 Reset Game");
    resetBtn->setFixedHeight(35);
    resetBtn->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; border-radius: 5px; border: none; padding: 8px; } QPushButton:hover { background-color: #b8e6b8; }");
    layout->addWidget(resetBtn);
    
    container->setStyleSheet("background-color: #1e1e2e;");
    
    QMdiSubWindow *window = workspace->addSubWindow(container);
    window->setWindowTitle("Snake Game");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(420, 480);
    
    // FIX 2A: Set attribute explicitly so the window completely drops out of memory when closed
    window->setAttribute(Qt::WA_DeleteOnClose);
    
    m_currentSnakeWindow = app;
    m_snakeSubWindow = window;
    
    // FIX 2B: Clean lambda capturing the simple object. Closes the active window and safely loops back.
    connect(resetBtn, &QPushButton::clicked, this, [this, window]() {
        window->close();             
        this->launchSnakeApp();      
    });
    
    // FIX 2C: Safely empty tracking pointers upon actual deletion lifecycle hook
    connect(window, &QMdiSubWindow::destroyed, this, [this]() {
        m_currentSnakeWindow = nullptr;
        m_snakeSubWindow = nullptr;
    });
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    window->raise();
    workspace->setActiveSubWindow(window);
}

void Desktop::launchBrowserApp() {
    BrowserApp *app = new BrowserApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    app->setFocusPolicy(Qt::StrongFocus);
    
    QWidget *browserContainer = new QWidget();
    QVBoxLayout *browserLayout = new QVBoxLayout(browserContainer);
    browserLayout->setContentsMargins(0, 0, 0, 0);
    browserLayout->addWidget(app);
    browserContainer->setFocusPolicy(Qt::StrongFocus);
    browserContainer->setStyleSheet("background-color: #1e1e2e;");
    
    QMdiSubWindow *window = workspace->addSubWindow(browserContainer);
    window->setWindowTitle("Browser");
    window->setStyleSheet("QMdiSubWindow { background-color: #1e1e2e; border: 2px solid #89b4fa; }");
    window->resize(950, 600);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus();
}

void Desktop::updateClock() {
    clockLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void Desktop::shutdownOS() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Shutdown", "Power off?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) QApplication::quit();
}

void Desktop::toggleTheme() {
    m_darkMode = !m_darkMode;
    
    QPushButton *themeBtn = nullptr;
    for (QPushButton *btn : findChildren<QPushButton*>()) {
        if (btn->text() == "🌙" || btn->text() == "☀️") {
            themeBtn = btn;
            break;
        }
    }
    if (themeBtn) {
        themeBtn->setText(m_darkMode ? "🌙" : "☀️");
    }
    
    this->update();
}

void Desktop::setupSystemTray() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    
    QMenu *trayMenu = new QMenu(this);
    QAction *restoreAction = trayMenu->addAction("Restore");
    connect(restoreAction, &QAction::triggered, this, [this]() {
        this->showNormal();
        this->setWindowState(Qt::WindowFullScreen);
    });
    
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, QOverload<QSystemTrayIcon::ActivationReason>::of(&QSystemTrayIcon::activated), this, &Desktop::restoreFromTray);
    trayIcon->show();
}

void Desktop::restoreFromTray(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        this->showNormal();
        this->setWindowState(Qt::WindowFullScreen);
    }
}

void Desktop::applyGlobalStyle() {
    this->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; border-radius: 5px; font-weight: bold; padding: 5px; border: none; } QPushButton:hover { background-color: #b4befe; }");
}

void Desktop::refreshSystem() {
    updateClock();
    workspace->update();
}