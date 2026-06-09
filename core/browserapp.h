#ifndef BROWSERAPP_H
#define BROWSERAPP_H

#include <QWidget>
#include <QProcess>

class BrowserApp : public QWidget {
    Q_OBJECT
public:
    explicit BrowserApp(QWidget *parent = nullptr);
    ~BrowserApp();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void embedLiveEngine();

private:
    QProcess *m_process;
    QWidget *m_containerWidget;
    unsigned long long m_targetHwnd; // Holds the native Win32 HWND handle safely
};

#endif // BROWSERAPP_H