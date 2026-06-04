#include "filesystemapp.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>

FileSystemApp::FileSystemApp(QWidget *parent) : QWidget(parent), fileCounter(1) {
    this->setStyleSheet("background-color: #11111b; color: #cdd6f4;");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // Left Panel: File Explorer
    QVBoxLayout *leftPanel = new QVBoxLayout();
    fileTree = new QTreeWidget();
    fileTree->setHeaderLabel("💾 C: Drive (Virtual)");
    fileTree->setStyleSheet("background-color: #1e1e2e; border: 1px solid #45475a;");
    
    QTreeWidgetItem *root = new QTreeWidgetItem(fileTree);
    root->setText(0, "Documents");
    fileTree->addTopLevelItem(root);
    root->setExpanded(true);

    newFileBtn = new QPushButton("📄 New File");
    newFileBtn->setStyleSheet("background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 5px;");
    leftPanel->addWidget(fileTree);
    leftPanel->addWidget(newFileBtn);

    // Right Panel: Text Editor
    QVBoxLayout *rightPanel = new QVBoxLayout();
    textEditor = new QTextEdit();
    textEditor->setStyleSheet("background-color: #1e1e2e; border: 1px solid #45475a; font-family: 'Courier New';");
    
    saveBtn = new QPushButton("💾 Save to VFS");
    saveBtn->setStyleSheet("background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 5px;");
    
    rightPanel->addWidget(textEditor);
    rightPanel->addWidget(saveBtn);

    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addLayout(rightPanel, 2);

    // Connections
    connect(newFileBtn, &QPushButton::clicked, this, &FileSystemApp::createNewFile);
    connect(saveBtn, &QPushButton::clicked, this, &FileSystemApp::saveFileContent);
    connect(fileTree, &QTreeWidget::itemClicked, this, &FileSystemApp::loadFileContent);
}

void FileSystemApp::createNewFile() {
    QString fileName = "File_" + QString::number(fileCounter++) + ".txt";
    QTreeWidgetItem *item = new QTreeWidgetItem(fileTree->topLevelItem(0));
    item->setText(0, fileName);
    virtualHardDrive[fileName] = ""; // Initialize empty file in memory
    fileTree->setCurrentItem(item);
    textEditor->clear();
}

void FileSystemApp::saveFileContent() {
    QTreeWidgetItem *current = fileTree->currentItem();
    if (current && current->parent() != nullptr) { // Make sure it's a file, not the root folder
        QString fileName = current->text(0);
        virtualHardDrive[fileName] = textEditor->toPlainText();
        saveBtn->setText("✅ Saved!");
        QTimer::singleShot(1500, [this]() { saveBtn->setText("💾 Save to VFS"); });
    }
}

void FileSystemApp::loadFileContent(QTreeWidgetItem *item, int column) {
    if (item->parent() != nullptr) { // If it's a file
        QString fileName = item->text(0);
        textEditor->setText(virtualHardDrive[fileName]);
    }
}