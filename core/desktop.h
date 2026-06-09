#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMap>

class SnakeGameApp;
class QMdiSubWindow;

class Desktop : public QMainWindow {
    Q_OBJECT

public:
    Desktop(QWidget *parent = nullptr);
    ~Desktop();
    void refreshSystem();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void openSecuritySettings();
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
    void restoreFromTray(QSystemTrayIcon::ActivationReason reason);

private:
    void setupDesktopEnvironment();
    void setupTaskbar();
    void setupStartMenu();
    void createShortcut(QString iconText, QString name, int x, int y, void (Desktop::*slotFunction)());
    void applyGlobalStyle();
    void setupSystemTray();

    QMdiArea *workspace;
    QWidget *taskbar;
    QFrame *startMenuWidget;
    QPushButton *startBtn;
    QPushButton *shutdownBtn;
    QLabel *clockLabel;
    QTimer *systemTimer;
    
    SnakeGameApp *m_currentSnakeWindow = nullptr;
    QMdiSubWindow *m_snakeSubWindow = nullptr;
    
    bool m_darkMode = true;
    QSystemTrayIcon *trayIcon = nullptr;
    QMap<QString, QString> *masterFileSystem = nullptr;
};