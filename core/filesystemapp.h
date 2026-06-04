#ifndef FILESYSTEMAPP_H
#define FILESYSTEMAPP_H

#include <QWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>

class FileSystemApp : public QWidget {
    Q_OBJECT

public:
    explicit FileSystemApp(QWidget *parent = nullptr);

private slots:
    void createNewFile();
    void loadFileContent(QTreeWidgetItem *item, int column);
    void saveFileContent();

private:
    QTreeWidget *fileTree;
    QTextEdit *textEditor;
    QPushButton *newFileBtn;
    QPushButton *saveBtn;
    
    int fileCounter;
    QMap<QString, QString> virtualHardDrive; // Maps Filename -> File Content
};

#endif // FILESYSTEMAPP_H