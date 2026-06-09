#include "desktop.h"
#include "logindialog.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Initialize the internal system registry
    QSettings settings("EduCloudOS", "SecuritySettings");
    
    // Default to 'false' (disabled) if the user hasn't set it yet
    bool isLoginEnabled = settings.value("loginEnabled", false).toBool();

    if (isLoginEnabled) {
        LoginDialog login;
        if (login.exec() == QDialog::Accepted) {
            Desktop w;
            w.showMaximized();
            return a.exec();
        }
        return 0; // Exit if they close or fail the login window
    } else {
        // Skip login screen entirely by default!
        Desktop w;
      w.showFullScreen();
        return a.exec();
    }
}