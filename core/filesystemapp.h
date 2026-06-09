#ifndef FILESYSTEMAPP_H
#define FILESYSTEMAPP_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMap>

class FileSystemApp : public QWidget {
    Q_OBJECT
public:
    // Constructor accepts a reference to the desktop's permanent map allocation
    explicit FileSystemApp(QMap<QString, QString> *masterStorage, QWidget *parent = nullptr);

private slots:
    void saveFile();
    void loadFile(QListWidgetItem *item);
    void deleteFile();

private:
    QMap<QString, QString> *m_storage; // Link pointer to master filesystem array
    
    QListWidget *fileList;
    QLineEdit *fileNameInput;
    QComboBox *fileTypeDropdown; // Dynamic formatting dropdown selector
    QTextEdit *fileContentInput;
    
    QPushButton *saveBtn;
    QPushButton *deleteBtn;

    void refreshFileList();
};

#endif // FILESYSTEMAPP_H