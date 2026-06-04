#ifndef DESKTOP_H
#define DESKTOP_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class Desktop : public QMainWindow {
    Q_OBJECT

    
public:
    Desktop(QWidget *parent = nullptr);
    ~Desktop();

    void refreshSystem();

private slots:
    void updateClock();
    void shutdownOS();
    void launchSchedulerApp();
    void launchTaskManagerApp();
    void launchFileSystemApp();
    void launchCalculatorApp(); 
    void launchSnakeApp();      
    void toggleStartMenu(); // Start menu trigger

    void launchBrowserApp();
private:
    QMdiArea *workspace;
    QWidget *taskbar;
    QLabel *clockLabel;
    QPushButton *startBtn;
    QPushButton *shutdownBtn;
    QTimer *systemTimer;
    QFrame *startMenuWidget; // Floating panel

    void setupTaskbar();
    void setupDesktopEnvironment();
    void setupStartMenu();
    void createShortcut(QString iconText, QString name, int x, int y, void (Desktop::*slotFunction)());
    void applyGlobalStyle();
};

#endif // DESKTOP_H