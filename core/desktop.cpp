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
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QProgressBar>
#include <QSlider>
#include <QProcess>
#include <QSpinBox>
#include <QComboBox>

// ============================================================================
// 1. SECURITY CONFIGURATION PORTAL
// ============================================================================
void Desktop::openSecuritySettings() {
    QSettings settings("EduCloudOS", "SecuritySettings");
    bool currentStatus = settings.value("loginEnabled", false).toBool();
    QString statusText = currentStatus ? "ENABLED" : "DISABLED";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("🔐 EduCloud Security Configuration");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(QString("Current Lock Screen Status: <b>%1</b><br><br>What would you like to do?").arg(statusText));
    msgBox.setStyleSheet(R"(
        QMessageBox {
            background-color: #2c001e;
        }
        QLabel {
            color: #cdd6f4;
        }
        QPushButton {
            min-width: 80px;
            padding: 5px;
            background-color: #89b4fa;
            color: #11111b;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
    )");
    
    QPushButton *toggleBtn = msgBox.addButton(currentStatus ? "Disable Password Lock" : "Enable Password Lock", QMessageBox::ActionRole);
    QPushButton *cancelBtn = msgBox.addButton(QMessageBox::Cancel);
    
    msgBox.exec();

    if (msgBox.clickedButton() == toggleBtn) {
        if (!currentStatus) {
            bool ok;
            QString newPassword = QInputDialog::getText(this, "Set System Password",
                                                       "Create a new administrative password:", 
                                                       QLineEdit::Password, "", &ok);
            if (ok && !newPassword.isEmpty()) {
                settings.setValue("loginEnabled", true);
                settings.setValue("systemPassword", newPassword);
                QMessageBox::information(this, "✅ Success", "Password protection enabled! It will prompt on next boot.");
            } else {
                QMessageBox::warning(this, "❌ Cancelled", "Password cannot be empty. Setup aborted.");
            }
        } else {
            bool ok;
            QString checkPassword = QInputDialog::getText(this, "Disable Protection",
                                                       "Enter your current password to disable:", 
                                                       QLineEdit::Password, "", &ok);
            if (ok && checkPassword == settings.value("systemPassword", "admin123").toString()) {
                settings.setValue("loginEnabled", false);
                QMessageBox::information(this, "✅ Success", "Password lock disabled. App will boot directly to desktop.");
            } else if (ok) {
                QMessageBox::critical(this, "❌ Error", "Incorrect password. Settings unchanged.");
            }
        }
    }
}

// ============================================================================
// 2. DYNAMIC WALLPAPER RENDER ENGINE
// ============================================================================
void Desktop::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QImage bg(m_currentWallpaperPath);
    if (!bg.isNull()) {
        QImage scaledBg = bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        painter.drawImage(0, 0, scaledBg);
    } else {
        // Enhanced gradient background with brightness control
        QLinearGradient gradient(0, 0, 0, this->height());
        
        if (m_darkMode) {
            gradient.setColorAt(0.0, QColor(44, 0, 30));   
            gradient.setColorAt(0.5, QColor(79, 25, 63));  
            gradient.setColorAt(1.0, QColor(30, 30, 46));
        } else {
            gradient.setColorAt(0.0, QColor(200, 150, 180));   
            gradient.setColorAt(0.5, QColor(180, 120, 160));  
            gradient.setColorAt(1.0, QColor(220, 180, 200));
        }
        
        painter.fillRect(this->rect(), gradient);
        
        // Add subtle grid overlay
        painter.setOpacity(0.05);
        painter.setPen(QPen(QColor(137, 180, 250), 1));
        int gridSize = 50;
        for (int i = 0; i < this->width(); i += gridSize) {
            painter.drawLine(i, 0, i, this->height());
        }
        for (int i = 0; i < this->height(); i += gridSize) {
            painter.drawLine(0, i, this->width(), i);
        }
        painter.setOpacity(1.0);
    }

    QImage logo("system_logo.png"); 
    if (!logo.isNull()) {
        QImage scaledLogo = logo.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int logoX = this->width() - scaledLogo.width() - 35;
        int logoY = this->height() - scaledLogo.height() - 35;
        
        painter.setOpacity(0.60); 
        painter.drawImage(logoX, logoY, scaledLogo);
        painter.setOpacity(1.0);  
    }
}

void Desktop::changeWallpaperDialog() {
    QString filePath = QFileDialog::getOpenFileName(this, 
        tr("Select Desktop Wallpaper Environment"), "", 
        tr("Images (*.png *.jpg *.jpeg *.bmp)"));
        
    if (!filePath.isEmpty()) {
        m_currentWallpaperPath = filePath;
        QSettings settings("EduCloudOS", "Display");
        settings.setValue("wallpaper", filePath);
        this->update(); 
    }
}

// ============================================================================
// 3. CORE CONSTRUCTOR & DESTRUCTOR
// ============================================================================
Desktop::Desktop(QWidget *parent) 
    : QMainWindow(parent), 
      m_currentSnakeWindow(nullptr),
      m_snakeSubWindow(nullptr),
      m_darkMode(true),
      m_brightness(100) {
    
    setWindowTitle("EduCloud OS - Desktop Environment");
    
    // Full-Screen System Lockdown
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Window);
    
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        this->setGeometry(screen->geometry());
    }
    setWindowState(Qt::WindowFullScreen);

    // Load saved wallpaper
    QSettings displaySettings("EduCloudOS", "Display");
    m_currentWallpaperPath = displaySettings.value("wallpaper", "wallpaper.jpg").toString();
    m_darkMode = displaySettings.value("darkMode", true).toBool();
    
    // Initialize file system storage (QMap)
    masterFileSystem = new QMap<QString, QString>();
    
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    workspace = new QMdiArea(this);
    workspace->setViewMode(QMdiArea::SubWindowView); 
    
    // Enhanced transparency setup
    workspace->setBackground(Qt::NoBrush);
    workspace->setAttribute(Qt::WA_TranslucentBackground, true);
    workspace->viewport()->setAttribute(Qt::WA_TranslucentBackground, true);
    workspace->setStyleSheet("QMdiArea { background: transparent; border: none; }");

    new ContextMenuManager(this, workspace);

    setupDesktopEnvironment();
    setupTaskbar();
    setupStartMenu();
    setupSystemTray();

    systemTimer = new QTimer(this);
    connect(systemTimer, &QTimer::timeout, this, &Desktop::updateClock);
    systemTimer->start(1000);
    
    // System stats timer (update every 2 seconds)
    statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, &Desktop::showSystemStats);
    statsTimer->start(2000);
    
    updateClock();
    showSystemStats();
    applyGlobalStyle();
}

Desktop::~Desktop() {
    QSettings displaySettings("EduCloudOS", "Display");
    displaySettings.setValue("wallpaper", m_currentWallpaperPath);
    displaySettings.setValue("darkMode", m_darkMode);
    
    if (masterFileSystem) {
        delete masterFileSystem;
        masterFileSystem = nullptr;
    }
}

void Desktop::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (windowState() & Qt::WindowMinimized) {
            hide();
        }
    }
    QMainWindow::changeEvent(event);
}

void Desktop::closeEvent(QCloseEvent *event) {
    if (trayIcon && trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

// ============================================================================
// 4. COMPONENT LAYOUT GENERATION
// ============================================================================
void Desktop::setupDesktopEnvironment() {
    QLabel *osLogo = new QLabel(workspace);
    osLogo->setText("☁️\nEduCloud OS");
    osLogo->setStyleSheet(R"(
        QLabel {
            color: rgba(205, 214, 244, 0.08);
            font-size: 75px;
            font-weight: bold;
            font-family: 'Segoe UI', Arial;
            border: none;
            background: transparent;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
        }
    )");
    osLogo->setAlignment(Qt::AlignCenter);
    
    QScreen *screen = QGuiApplication::primaryScreen();
    int screenW = screen->geometry().width();
    int screenH = screen->geometry().height();
    osLogo->setGeometry((screenW - 600) / 2, (screenH - 300) / 2 - 50, 600, 300);
    osLogo->lower(); 

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
            background-color: rgba(49, 50, 68, 0.9);
            color: white;
            font-size: 35px;
            border-radius: 15px;
            border: 2px solid #45475a;
            transition: all 0.3s ease;
        }
        QPushButton:hover {
            background-color: rgba(137, 180, 250, 0.9);
            border: 2px solid #89b4fa;
            transform: scale(1.05);
        }
        QPushButton:pressed {
            background-color: rgba(166, 227, 161, 0.9);
        }
    )");
    
    connect(btn, &QPushButton::clicked, this, slotFunction);

    QLabel *label = new QLabel(name, workspace);
    label->setStyleSheet(R"(
        QLabel {
            color: #cdd6f4;
            font-size: 11px;
            font-weight: bold;
            background: transparent;
            border: none;
        }
    )");
    label->setFixedWidth(130);
    label->setAlignment(Qt::AlignCenter);
    label->move(x - 30, y + 75);
}

void Desktop::setupTaskbar() {
    taskbar = new QWidget(this);
    taskbar->setFixedHeight(50);
    taskbar->setStyleSheet(R"(
        background-color: rgba(17, 17, 27, 0.95);
        border-top: 3px solid #89b4fa;
    )");

    QHBoxLayout *taskbarLayout = new QHBoxLayout(taskbar);
    taskbarLayout->setContentsMargins(10, 0, 10, 0);
    taskbarLayout->setSpacing(5);

    startBtn = new QPushButton("☁️ Start", this);
    startBtn->setFixedSize(100, 35);
    startBtn->setCursor(Qt::PointingHandCursor);
    startBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            font-weight: bold;
            border-radius: 8px;
            border: none;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
        QPushButton:pressed {
            background-color: #a6e3a1;
        }
    )");
    connect(startBtn, &QPushButton::clicked, this, &Desktop::toggleStartMenu);
    
    clockLabel = new QLabel("00:00:00", this);
    clockLabel->setStyleSheet(R"(
        QLabel {
            color: #cdd6f4;
            font-weight: bold;
            font-size: 14px;
            border: none;
            background: transparent;
            padding: 0 10px;
        }
    )");
    
    systemStatsLabel = new QLabel("CPU: 0% | RAM: 0%", this);
    systemStatsLabel->setStyleSheet(R"(
        QLabel {
            color: #89b4fa;
            font-weight: bold;
            font-size: 12px;
            border: none;
            background: transparent;
            padding: 0 10px;
        }
    )");
    
    QPushButton *themeBtn = new QPushButton("🌙", this);
    themeBtn->setFixedSize(35, 35);
    themeBtn->setCursor(Qt::PointingHandCursor);
    themeBtn->setToolTip("Toggle Theme");
    themeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            border-radius: 5px;
            border: none;
        }
        QPushButton:hover { background-color: #b4befe; }
    )");
    connect(themeBtn, &QPushButton::clicked, this, &Desktop::toggleTheme);
    
    QPushButton *controlBtn = new QPushButton("⚙️", this);
    controlBtn->setFixedSize(35, 35);
    controlBtn->setCursor(Qt::PointingHandCursor);
    controlBtn->setToolTip("Control Panel");
    controlBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            border-radius: 5px;
            border: none;
        }
        QPushButton:hover { background-color: #b4befe; }
    )");
    connect(controlBtn, &QPushButton::clicked, this, &Desktop::openControlPanel);
    
    shutdownBtn = new QPushButton("🛑 Shutdown", this);
    shutdownBtn->setFixedSize(100, 35);
    shutdownBtn->setCursor(Qt::PointingHandCursor);
    shutdownBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f38ba8;
            color: #11111b;
            font-weight: bold;
            border-radius: 8px;
            border: none;
        }
        QPushButton:hover {
            background-color: #eba0ac;
        }
        QPushButton:pressed {
            background-color: #f28482;
        }
    )");
    connect(shutdownBtn, &QPushButton::clicked, this, &Desktop::shutdownOS);

    taskbarLayout->addWidget(startBtn);
    taskbarLayout->addWidget(themeBtn);
    taskbarLayout->addWidget(controlBtn);
    taskbarLayout->addStretch(); 
    taskbarLayout->addWidget(systemStatsLabel);
    taskbarLayout->addWidget(clockLabel);
    taskbarLayout->addWidget(shutdownBtn);

    QWidget *centralContainer = new QWidget(this);
    centralContainer->setAttribute(Qt::WA_TranslucentBackground, true);
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
    startMenuWidget->setFixedSize(250, 430); 
    startMenuWidget->setStyleSheet(R"(
        QFrame {
            background-color: rgba(30, 30, 46, 0.98);
            border: 2px solid #89b4fa;
            border-radius: 12px;
        }
    )");
    startMenuWidget->hide(); 

    QVBoxLayout *menuLayout = new QVBoxLayout(startMenuWidget);
    menuLayout->setContentsMargins(15, 15, 15, 15);
    menuLayout->setSpacing(5);

    QLabel *userLabel = new QLabel("👤 Admin User", startMenuWidget);
    userLabel->setStyleSheet(R"(
        QLabel {
            color: #cdd6f4;
            font-size: 16px;
            font-weight: bold;
            border: none;
            border-bottom: 2px solid #45475a;
            padding-bottom: 10px;
            background: transparent;
        }
    )");
    menuLayout->addWidget(userLabel);

    auto addMenuBtn = [this, menuLayout](QString text, auto slotFunc) {
        QPushButton *btn = new QPushButton(text, startMenuWidget);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(R"(
            QPushButton { 
                text-align: left;
                padding: 10px;
                background: transparent;
                color: #cdd6f4;
                font-size: 13px;
                border: none;
                border-radius: 6px;
                font-weight: 500;
            } 
            QPushButton:hover {
                background-color: #45475a;
                color: #b4befe;
            }
            QPushButton:pressed {
                background-color: #313244;
            }
        )");
        connect(btn, &QPushButton::clicked, this, slotFunc);
        connect(btn, &QPushButton::clicked, this, &Desktop::toggleStartMenu); 
        menuLayout->addWidget(btn);
    };

    menuLayout->addSpacing(5);
    addMenuBtn("⚙️ Task Orchestrator", &Desktop::launchSchedulerApp);
    addMenuBtn("📊 System Task Manager", &Desktop::launchTaskManagerApp);
    addMenuBtn("📁 EduCloud File Explorer", &Desktop::launchFileSystemApp);
    addMenuBtn("🧮 Calculator", &Desktop::launchCalculatorApp);
    addMenuBtn("🐍 Snake Game", &Desktop::launchSnakeApp);
    addMenuBtn("🌐 Internet Browser Explorer", &Desktop::launchBrowserApp);
    addMenuBtn("🖼️ Change Wallpaper", &Desktop::changeWallpaperDialog);
    addMenuBtn("🔐 Lock Screen Security", &Desktop::openSecuritySettings);
    addMenuBtn("🔄 Refresh System", &Desktop::refreshSystem);

    menuLayout->addStretch();
}

void Desktop::toggleStartMenu() {
    if (startMenuWidget->isHidden()) {
        QPoint globalPos = startBtn->mapToGlobal(QPoint(0, 0));
        QPoint localPos = this->mapFromGlobal(globalPos);
        
        startMenuWidget->move(localPos.x(), localPos.y() - startMenuWidget->height() - 10);
        
        // Fade-in animation
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
        startMenuWidget->setGraphicsEffect(effect);
        
        QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(200);
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        
        startMenuWidget->raise(); 
        startMenuWidget->show();
    } else {
        startMenuWidget->hide();
    }
}

// ============================================================================
// 5. APPLICATION SUB-WINDOW GENERATION ROUTINES
// ============================================================================
void Desktop::launchSchedulerApp() {
    MainWindow *app = new MainWindow();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose); 
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud - Task Orchestrator");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(1000, 700);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

void Desktop::launchTaskManagerApp() {
    TaskManagerApp *app = new TaskManagerApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("System Task Manager");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(450, 400);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

void Desktop::launchFileSystemApp() {
    FileSystemApp *app = new FileSystemApp(masterFileSystem);
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud File Explorer");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(650, 450);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

void Desktop::launchCalculatorApp() {
    CalculatorApp *app = new CalculatorApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Calculator");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(250, 320);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

void Desktop::launchSnakeApp() {
    // If snake window already exists, bring it to front
    if (m_currentSnakeWindow != nullptr && m_snakeSubWindow != nullptr) {
        m_snakeSubWindow->raise();
        m_snakeSubWindow->setFocus();
        workspace->setActiveSubWindow(m_snakeSubWindow);
        return;
    }

    SnakeGameApp *app = new SnakeGameApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("Snake Game");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(420, 440);
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    // Store references for reuse
    m_currentSnakeWindow = app;
    m_snakeSubWindow = window;
    
    // Connect close signal to clear references
    connect(window, &QMdiSubWindow::destroyed, this, [this]() {
        m_currentSnakeWindow = nullptr;
        m_snakeSubWindow = nullptr;
    });
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

void Desktop::launchBrowserApp() {
    BrowserApp *app = new BrowserApp();
    app->setWindowFlags(Qt::Widget);
    app->setAttribute(Qt::WA_DeleteOnClose);
    
    QMdiSubWindow *window = workspace->addSubWindow(app);
    window->setWindowTitle("EduCloud Live Web Browser Engine");
    window->setStyleSheet(R"(
        QMdiSubWindow {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 8px;
        }
    )");
    app->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    window->resize(950, 600); 
    
    int x = (workspace->width() - window->width()) / 2;
    int y = (workspace->height() - window->height()) / 2;
    window->move(qMax(0, x), qMax(0, y));
    
    window->show();
    app->show();
    window->raise();
    workspace->setActiveSubWindow(window);
    app->setFocus(Qt::OtherFocusReason);
}

// ============================================================================
// 6. SYSTEM UTILITIES & REGISTRY MANAGEMENT
// ============================================================================
void Desktop::updateClock() {
    clockLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss AP | ddd, MMM d"));
}

void Desktop::showSystemStats() {
    // Simulated system stats (you can replace with actual system info)
    static int cpuUsage = 0;
    static int ramUsage = 0;
    
    cpuUsage = (cpuUsage + 5) % 100;
    ramUsage = (ramUsage + 3) % 100;
    
    systemStatsLabel->setText(QString("CPU: %1% | RAM: %2%").arg(cpuUsage).arg(ramUsage));
}

void Desktop::shutdownOS() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Shutdown OS", "Are you sure you want to power off the system?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void Desktop::toggleTheme() {
    m_darkMode = !m_darkMode;
    applyGlobalStyle();
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
    connect(trayIcon, QOverload<QSystemTrayIcon::ActivationReason>::of(&QSystemTrayIcon::activated),
            this, &Desktop::restoreFromTray);
    
    trayIcon->show();
}

void Desktop::restoreFromTray(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        this->showNormal();
        this->setWindowState(Qt::WindowFullScreen);
    }
}

void Desktop::openControlPanel() {
    createControlPanel();
}

void Desktop::createControlPanel() {
    if (controlPanelWidget && !controlPanelWidget->isHidden()) {
        controlPanelWidget->raise();
        controlPanelWidget->setFocus();
        return;
    }
    
    controlPanelWidget = new QFrame(this);
    controlPanelWidget->setFixedSize(400, 300);
    controlPanelWidget->setStyleSheet(R"(
        QFrame {
            background-color: rgba(30, 30, 46, 0.98);
            border: 2px solid #89b4fa;
            border-radius: 12px;
        }
    )");
    
    QVBoxLayout *layout = new QVBoxLayout(controlPanelWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    QLabel *titleLabel = new QLabel("⚙️ Control Panel", controlPanelWidget);
    titleLabel->setStyleSheet("color: #cdd6f4; font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);
    
    // Brightness control
    QHBoxLayout *brightnessLayout = new QHBoxLayout();
    QLabel *brightnessLabel = new QLabel("🔆 Brightness:", controlPanelWidget);
    brightnessLabel->setStyleSheet("color: #cdd6f4;");
    brightnessSlider = new QSlider(Qt::Horizontal, controlPanelWidget);
    brightnessSlider->setRange(50, 150);
    brightnessSlider->setValue(100);
    brightnessSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            background: #313244;
            height: 8px;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #89b4fa;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
        QSlider::handle:horizontal:hover {
            background: #b4befe;
        }
    )");
    connect(brightnessSlider, &QSlider::valueChanged, this, &Desktop::adjustBrightness);
    
    brightnessLayout->addWidget(brightnessLabel);
    brightnessLayout->addWidget(brightnessSlider);
    layout->addLayout(brightnessLayout);
    
    // Theme selector
    QHBoxLayout *themeLayout = new QHBoxLayout();
    QLabel *themeLabel = new QLabel("🎨 Theme:", controlPanelWidget);
    themeLabel->setStyleSheet("color: #cdd6f4;");
    QComboBox *themeCombo = new QComboBox(controlPanelWidget);
    themeCombo->addItems({"Dark Mode", "Light Mode"});
    themeCombo->setStyleSheet(R"(
        QComboBox {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 5px;
        }
    )");
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this](int index) {
                m_darkMode = (index == 0);
                this->update();
                applyGlobalStyle();
            });
    
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);
    layout->addLayout(themeLayout);
    
    layout->addSpacing(10);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *applyBtn = new QPushButton("Apply", controlPanelWidget);
    applyBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #a6e3a1;
            color: #11111b;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #b8e6b8; }
    )");
    connect(applyBtn, &QPushButton::clicked, [this]() {
        QSettings settings("EduCloudOS", "Display");
        settings.setValue("brightness", m_brightness);
        settings.setValue("darkMode", m_darkMode);
    });
    
    QPushButton *closeBtn = new QPushButton("Close", controlPanelWidget);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f38ba8;
            color: #11111b;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #eba0ac; }
    )");
    connect(closeBtn, &QPushButton::clicked, [this]() {
        if (controlPanelWidget) {
            controlPanelWidget->hide();
        }
    });
    
    buttonLayout->addWidget(applyBtn);
    buttonLayout->addWidget(closeBtn);
    layout->addLayout(buttonLayout);
    
    layout->addStretch();
    
    // Position in center
    int x = (this->width() - controlPanelWidget->width()) / 2;
    int y = (this->height() - controlPanelWidget->height()) / 2;
    controlPanelWidget->move(x, y);
    controlPanelWidget->raise();
    controlPanelWidget->show();
}

void Desktop::adjustBrightness(int value) {
    m_brightness = value;
    // Apply brightness to workspace
    workspace->setStyleSheet(QString("QMdiArea { background: transparent; opacity: %1; }").arg(value / 100.0));
}

void Desktop::applyGlobalStyle() {
    QString styleSheet = R"(
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            border-radius: 8px;
            font-weight: bold;
            padding: 8px;
            border: none;
            transition: all 0.3s ease;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
        QPushButton:pressed {
            background-color: #a6e3a1;
        }
        
        QSpinBox, QComboBox {
            background-color: #313244;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 6px;
            padding: 6px;
            padding-right: 20px;
            font-weight: 500;
        }
        QSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 18px;
            border-left: 1px solid #45475a;
            background: #45475a;
        }
        QSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 18px;
            border-left: 1px solid #45475a;
            background: #45475a;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: #585b70;
        }
        QSpinBox::up-arrow {
            image: none;
            text-align: center;
            color: #cdd6f4;
            font-size: 8px;
        }
        QSpinBox::down-arrow {
            image: none;
            text-align: center;
            color: #cdd6f4;
            font-size: 8px;
        }
    )";
    
    this->setStyleSheet(styleSheet);
}

void Desktop::refreshSystem() {
    updateClock();
    showSystemStats();
    workspace->update();
    // Silent refresh - no confirmation dialog
}