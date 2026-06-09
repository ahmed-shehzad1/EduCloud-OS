#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

private slots:
    void verifyPassword();

private:
    QLineEdit *passwordInput;
    QPushButton *loginBtn;
};

#endif // LOGINDIALOG_H