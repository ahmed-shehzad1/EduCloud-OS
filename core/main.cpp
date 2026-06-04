#include <QApplication>
#include "desktop.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Booting the OS Desktop Environment
    Desktop osDesktop;
    osDesktop.show();
    
    return a.exec();
}