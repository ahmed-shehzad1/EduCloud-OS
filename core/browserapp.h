#ifndef BROWSERAPP_H
#define BROWSERAPP_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>

class BrowserApp : public QWidget {
    Q_OBJECT
public:
    explicit BrowserApp(QWidget *parent = nullptr);

private slots:
    void navigateToUrl();
    void goHome();

private:
    QLineEdit *urlBar;
    QPushButton *backBtn;
    QPushButton *forwardBtn;
    QPushButton *refreshBtn;
    QPushButton *homeBtn;
    QTextBrowser *webView;

    void loadMockPage(const QString &url);
};

#endif // BROWSERAPP_H