#include "logindialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    // Elegant, frameless modal box centered directly on screen
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setFixedSize(380, 420);
    setStyleSheet("QDialog { background-color: #2c001e; border: 2px solid #e95420; border-radius: 12px; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(15);

    // Profile Avatar Display Element
    QLabel *avatarLabel = new QLabel("👤", this);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("font-size: 72px; margin-bottom: 10px;");
    layout->addWidget(avatarLabel);

    QLabel *userLabel = new QLabel("EduCloud Administrator", this);
    userLabel->setAlignment(Qt::AlignCenter);
    userLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold; font-family: 'Segoe UI', Arial;");
    layout->addWidget(userLabel);

    // Masked Security Pin Field
    passwordInput = new QLineEdit(this);
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setPlaceholderText("Enter Custom Password...");
    passwordInput->setStyleSheet(
        "QLineEdit { background-color: #4f193f; color: white; border: 1px solid #aea79f; "
        "border-radius: 6px; padding: 10px; font-size: 14px; }"
        "QLineEdit:focus { border: 1px solid #e95420; background-color: #5e274e; }"
    );
    layout->addWidget(passwordInput);

    // Submission Trigger Action
    loginBtn = new QPushButton("Sign In", this);
    loginBtn->setStyleSheet(
        "QPushButton { background-color: #e95420; color: white; font-weight: bold; "
        "border: none; border-radius: 6px; padding: 12px; font-size: 14px; cursor: pointer; }"
        "QPushButton:hover { background-color: #ff6a36; }"
        "QPushButton:pressed { background-color: #c74316; }"
    );
    layout->addWidget(loginBtn);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::verifyPassword);
    connect(passwordInput, &QLineEdit::returnPressed, this, &LoginDialog::verifyPassword);
}

void LoginDialog::verifyPassword() {
    QSettings settings("EduCloudOS", "SecuritySettings");
    
    // Fetch the customized password set by the user; fallback to admin123 if null
    QString savedPassword = settings.value("systemPassword", "admin123").toString();

    if (passwordInput->text() == savedPassword) {
        accept(); // Core validation successful! Unlock the window loop.
    } else {
        QMessageBox::critical(this, "Authentication Failed", "Invalid administrative passphrase. Access Denied.");
        passwordInput->clear();
        passwordInput->setFocus();
    }
}