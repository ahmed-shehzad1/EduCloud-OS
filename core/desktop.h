#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMap>

// Forward declarations
class SnakeGameApp;
class QMdiSubWindow;
class QProgressBar;
class QSlider;

class Desktop : public QMainWindow {
    Q_OBJECT

public:
    Desktop(QWidget *parent = nullptr);
    ~Desktop();

    // Public methods that can be called from outside
    void refreshSystem();

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openSecuritySettings();
    void changeWallpaperDialog();
    void updateClock();
    void shutdownOS();
    void toggleStartMenu();
    void launchSchedulerApp();
    void launchTaskManagerApp();
    void launchFileSystemApp();
    void launchCalculatorApp();
    void launchSnakeApp();
    void launchBrowserApp();
    void toggleTheme();
    void showSystemStats();
    void restoreFromTray(QSystemTrayIcon::ActivationReason reason);
    void adjustBrightness(int value);
    void openControlPanel();

private:
    void setupDesktopEnvironment();
    void setupTaskbar();
    void setupStartMenu();
    void createShortcut(QString iconText, QString name, int x, int y, void (Desktop::*slotFunction)());
    void applyGlobalStyle();
    void setupSystemTray();
    void createControlPanel();
    void loadTheme(const QString &themeName);

    // UI Components
    QMdiArea *workspace;
    QWidget *taskbar;
    QFrame *startMenuWidget;
    QPushButton *startBtn;
    QPushButton *shutdownBtn;
    QLabel *clockLabel;
    QLabel *systemStatsLabel;
    QTimer *systemTimer;
    QTimer *statsTimer;
    QString m_currentWallpaperPath;
    
    // Snake game window tracking
    SnakeGameApp *m_currentSnakeWindow = nullptr;
    QMdiSubWindow *m_snakeSubWindow = nullptr;
    
    // New features
    bool m_darkMode;
    QSystemTrayIcon *trayIcon;
    QFrame *controlPanelWidget;
    QSlider *brightnessSlider;
    int m_brightness;
    
    // File system storage (QMap instead of FileSystem)
    QMap<QString, QString> *masterFileSystem = nullptr;
};