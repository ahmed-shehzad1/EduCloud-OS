#include "desktop.h"
#include "mainwindow.h" 
#include "taskmanagerapp.h"
#include "filesystemapp.h"
#include "calculatorapp.h"
#include "snakegameapp.h"
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QScreen>
#include "browserapp.h"
#include "contextmenumanager.h"

Desktop::Desktop(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("EduCloud OS - Desktop Environment");
    showFullScreen(); 

    // 1. Setup the Workspace
    workspace = new QMdiArea(this);
    workspace->setBackground(QBrush(QColor("#1e1e2e"))); 
    workspace->setViewMode(QMdiArea::SubWindowView); 
    setCentralWidget(workspace);

    new ContextMenuManager(this, workspace);
    // 2. Setup UI Components
    setupDesktopEnvironment();
    setupTaskbar();
    setupStartMenu();

    // 3. Start System Clock
    systemTimer = new QTimer(this);
    connect(systemTimer, &QTimer::timeout, this, &Desktop::updateClock);
    systemTimer->start(1000);
    updateClock();

    // 4. Apply Global Styles
    applyGlobalStyle();
}

Desktop::~Desktop() {}

void Desktop::setupDesktopEnvironment() {
    // --- INTRODUCING THE FOOLPROOF CENTERING WALLPAPER LOGO ---
    QLabel *osLogo = new QLabel(workspace);
    osLogo->setText("☁️\nEduCloud OS");
    osLogo->setStyleSheet("color: rgba(205, 214, 244, 0.08); font-size: 75px; font-weight: bold; font-family: 'Segoe UI', Arial; border: none; background: transparent;");
    osLogo->setAlignment(Qt::AlignCenter);
    
    // Snaps the watermark perfectly using standard viewport ratios
    QScreen *screen = QGuiApplication::primaryScreen();
    int screenW = screen->geometry().width();
    int screenH = screen->geometry().height();
    osLogo->setGeometry((screenW - 600) / 2, (screenH - 300) / 2 - 50, 600, 300);
    osLogo->lower(); 

    // --- GRID ARRANGEMENT OF DESKTOP SHORTCUTS ---
    createShortcut("⚙️", "Task Orchestrator", 40, 40, &Desktop::launchSchedulerApp);
    createShortcut("📊", "Task Manager",    40, 160, &Desktop::launchTaskManagerApp);
    createShortcut("📁", "File System",     40, 280, &Desktop::launchFileSystemApp);
    createShortcut("🧮", "Calculator",      40, 400, &Desktop::launchCalculatorApp);
    createShortcut("🐍", "Snake Game",      40, 520, &Desktop::launchSnakeApp);
    createShortcut("🌐", "Web Browser",     40, 640, &Desktop::launchBrowserApp);
}

void Desktop::createShortcut(QString iconText, QString name, int x, int y, void (Desktop::*slotFunction)()) {
    QPushButton *btn = new QPushButton(iconText, workspace);
    btn->resize(70, 70);
    btn->move(x, y);
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(49, 50, 68, 0.8); color: white;
            font-size: 35px; border-radius: 15px; border: 1px solid #45475a;
        }
        QPushButton:hover { background-color: rgba(137, 180, 250, 0.8); }
    )");
    
    connect(btn, &QPushButton::clicked, this, slotFunction);

    QLabel *label = new QLabel(name, workspace);
    label->setStyleSheet("color: #cdd6f4; font-size: 11px; font-weight: bold; background: transparent; border: none;");
    label->setFixedWidth(130);
    label->setAlignment(Qt::AlignCenter);
    label->move(x - 30, y + 75);
}

void Desktop::setupTaskbar() {
    taskbar = new QWidget(this);
    taskbar->setFixedHeight(50);
    taskbar->setStyleSheet("background-color: #11111b; border-top: 2px solid #89b4fa;");

    QHBoxLayout *taskbarLayout = new QHBoxLayout(taskbar);
    taskbarLayout->setContentsMargins(10, 0, 10, 0);

    startBtn = new QPushButton("☁️ Start", this);
    startBtn->setFixedSize(100, 35);
    connect(startBtn, &QPushButton::clicked, this, &Desktop::toggleStartMenu);
    
    clockLabel = new QLabel("00:00:00", this);
    clockLabel->setStyleSheet("color: #cdd6f4; font-weight: bold; font-size: 14px; border: none; background: transparent;");
    
    shutdownBtn = new QPushButton("🛑 Shutdown", this);
    shutdownBtn->setFixedSize(100, 35);
    shutdownBtn->setStyleSheet("background-color: #f38ba8; color: #11111b; font-weight: bold; border-radius: 5px;");
    connect(shutdownBtn, &QPushButton::clicked, this, &Desktop::shutdownOS);

    taskbarLayout->addWidget(startBtn);
    taskbarLayout->addStretch(); 
    taskbarLayout->addWidget(clockLabel);
    taskbarLayout->addWidget(shutdownBtn);

    QWidget *centralContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(workspace, 1);
    mainLayout->addWidget(taskbar, 0);
    setCentralWidget(centralContainer);
}

void Desktop::setupStartMenu() {
    startMenuWidget = new QFrame(this);
    startMenuWidget->setFixedSize(250, 320);
    startMenuWidget->setStyleSheet("background-color: rgba(30, 30, 46, 0.98); border: 1px solid #89b4fa; border-radius: 10px;");
    startMenuWidget->hide(); 

    QVBoxLayout *menuLayout = new QVBoxLayout(startMenuWidget);
    menuLayout->setContentsMargins(15, 15, 15, 15);

    QLabel *userLabel = new QLabel("👤 Admin User", startMenuWidget);
    userLabel->setStyleSheet("color: #cdd6f4; font-size: 16px; font-weight: bold; border: none; border-bottom: 1px solid #45475a; padding-bottom: 10px; background: transparent;");
    menuLayout->addWidget(userLabel);

    auto addMenuBtn = [this, menuLayout](QString text, auto slotFunc) {
        QPushButton *btn = new QPushButton(text, startMenuWidget);
        btn->setStyleSheet(R"(
            QPushButton { 
                text-align: left; padding: 10px; background: transparent; 
                color: #cdd6f4; font-size: 14px; border: none; border-radius: 5px; 
            } 
            QPushButton:hover { background-color: #45475a; }
        )");
        connect(btn, &QPushButton::clicked, this, slotFunc);
        connect(btn, &QPushButton::clicked, this, &Desktop::toggleStartMenu); 
        menuLayout->addWidget(btn);
    };

    menuLayout->addSpacing(10);
    addMenuBtn("⚙️ Task Orchestrator", &Desktop::launchSchedulerApp);
    addMenuBtn("📊 System Task Manager", &Desktop::launchTaskManagerApp);
    addMenuBtn("📁 EduCloud File Explorer", &Desktop::launchFileSystemApp);
    addMenuBtn("🧮 Calculator", &Desktop::launchCalculatorApp);
    addMenuBtn("🐍 Snake Game", &Desktop::launchSnakeApp);
    addMenuBtn("🌐 Internet Browser Explorer", &Desktop::launchBrowserApp);

    menuLayout->addStretch();
}

void Desktop::toggleStartMenu() {
    if (startMenuWidget->isHidden()) {
        QPoint globalPos = startBtn->mapToGlobal(QPoint(0, 0));
        QPoint localPos = this->mapFromGlobal(globalPos);
        
        startMenuWidget->move(localPos.x(), localPos.y() - startMenuWidget->height() - 10);
        startMenuWidget->raise(); 
        startMenuWidget->show();
    } else {
        startMenuWidget->hide();
    }
}

void Desktop::launchSchedulerApp() {
    MainWindow *app = new MainWindow();
    app->setAttribute(Qt::WA_DeleteOnClose); 
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud - Task Orchestrator");
    window->resize(1000, 700);
    window->show();
}

void Desktop::launchTaskManagerApp() {
    TaskManagerApp *app = new TaskManagerApp();
    app->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("System Task Manager");
    window->resize(450, 400);
    window->show();
}

void Desktop::launchFileSystemApp() {
    FileSystemApp *app = new FileSystemApp();
    app->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud File Explorer");
    window->resize(650, 450);
    window->show();
}

void Desktop::launchCalculatorApp() {
    CalculatorApp *app = new CalculatorApp();
    app->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Calculator");
    window->resize(250, 320);
    window->show();
}

void Desktop::launchSnakeApp() {
    SnakeGameApp *app = new SnakeGameApp();
    app->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Snake Game");
    window->resize(420, 440);
    window->show();
}

void Desktop::launchBrowserApp() {
    BrowserApp *app = new BrowserApp();
    app->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud Virtual Web Browser");
    window->resize(700, 500);
    window->show();
}

void Desktop::updateClock() {
    clockLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss AP | ddd, MMM d"));
}

void Desktop::shutdownOS() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Shutdown OS", "Are you sure you want to power off the system?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) QApplication::quit();
}

void Desktop::applyGlobalStyle() {
    this->setStyleSheet(R"(
        QPushButton {
            background-color: #89b4fa; color: #11111b;
            border-radius: 5px; font-weight: bold; padding: 5px;
        }
        QPushButton:hover { background-color: #b4befe; }
        
        QSpinBox, QComboBox {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 4px; padding: 4px;
            padding-right: 20px;
        }
        QSpinBox::up-button {
            subcontrol-origin: border; subcontrol-position: top right;
            width: 18px; border-left: 1px solid #45475a; background: #45475a;
        }
        QSpinBox::down-button {
            subcontrol-origin: border; subcontrol-position: bottom right;
            width: 18px; border-left: 1px solid #45475a; background: #45475a;
        }
        QSpinBox::up-arrow {
            image: none; text-align: center; color: #cdd6f4; font-size: 8px;
        }
        QSpinBox::down-arrow {
            image: none; text-align: center; color: #cdd6f4; font-size: 8px;
        }
    )");

}
void Desktop::refreshSystem() {
    updateClock();
    workspace->update();
    QMessageBox::information(this, "System Refresh", "EduCloud System Cache & GUI Registry Refreshed Successfully!", QMessageBox::Ok);
}