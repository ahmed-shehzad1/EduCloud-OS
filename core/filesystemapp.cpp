#include "filesystemapp.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>

FileSystemApp::FileSystemApp(QMap<QString, QString> *masterStorage, QWidget *parent)
    : QWidget(parent), m_storage(masterStorage) {
    
    setWindowTitle("🛡️ EduCloud Persistent Storage Sandbox");
    resize(650, 400);

    // Dark Styling Sheet
    setStyleSheet(R"(
        QWidget { background-color: #1e1e2e; color: #cdd6f4; font-family: 'Segoe UI', Arial; }
        QListWidget { background-color: #181825; border: 1px solid #45475a; border-radius: 6px; padding: 5px; }
        QListWidget::item { padding: 6px; border-radius: 4px; }
        QListWidget::item:selected { background-color: #89b4fa; color: #11111b; }
        QLineEdit, QTextEdit, QComboBox { background-color: #313244; border: 1px solid #45475a; border-radius: 6px; padding: 6px; color: #cdd6f4; }
        QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; border-radius: 6px; padding: 8px; }
        QPushButton:hover { background-color: #b4befe; }
    )");

    // Master Layout Setup
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Left Panel: Dynamic File View
    leftLayout->addWidget(new QLabel("📂 Directory Structure:"));
    fileList = new QListWidget(this);
    leftLayout->addWidget(fileList);
    
    deleteBtn = new QPushButton("🗑️ Delete Selected File", this);
    deleteBtn->setStyleSheet("background-color: #f38ba8; color: #11111b;");
    leftLayout->addWidget(deleteBtn);

    // Right Panel: Text Editor & Format Attributes
    rightLayout->addWidget(new QLabel("📄 Configuration Node Matrix:"));
    
    QHBoxLayout *metaInputLayout = new QHBoxLayout();
    fileNameInput = new QLineEdit(this);
    fileNameInput->setPlaceholderText("Enter block system string name...");
    
    fileTypeDropdown = new QComboBox(this);
    fileTypeDropdown->addItems({".txt (Plain Text)", ".rtf (Rich Text)", ".log (System Log)", ".sh (Kernel Script)"});
    
    metaInputLayout->addWidget(fileNameInput, 3);
    metaInputLayout->addWidget(fileTypeDropdown, 1);
    rightLayout->addLayout(metaInputLayout);

    fileContentInput = new QTextEdit(this);
    fileContentInput->setPlaceholderText("Write binary stream buffers or document payload metadata here...");
    rightLayout->addWidget(fileContentInput);

    saveBtn = new QPushButton("💾 Commit & Write to Memory Map", this);
    rightLayout->addWidget(saveBtn);

    // Assembly
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 3);

    // Signal Triggers
    connect(saveBtn, &QPushButton::clicked, this, &FileSystemApp::saveFile);
    connect(deleteBtn, &QPushButton::clicked, this, &FileSystemApp::deleteFile);
    connect(fileList, &QListWidget::itemClicked, this, &FileSystemApp::loadFile);

    // Initial population from memory registry data matrix
    refreshFileList();
}

void FileSystemApp::saveFile() {
    QString rawName = fileNameInput->text().trimmed();
    if (rawName.isEmpty()) {
        QMessageBox::warning(this, "IO Error", "Target assignment file index cannot be completely blank!");
        return;
    }

    // Extract raw extension token
    QString selectedExt = fileTypeDropdown->currentText().split(" ").first();
    if (!rawName.endsWith(selectedExt)) {
        rawName += selectedExt;
    }

    // Write straight to the shared persistent pointer context allocation
    (*m_storage)[rawName] = fileContentInput->toPlainText();
    
    refreshFileList();
    QMessageBox::information(this, "RAM Allocation", "File written successfully into Virtual Flash Mapping Index!");
}

void FileSystemApp::loadFile(QListWidgetItem *item) {
    if (!item) return;
    QString targetFile = item->text();
    
    // Read directly out of persistent runtime memory pointers
    if (m_storage->contains(targetFile)) {
        // Strip off extensions to present clean inputs back to text labels
        int extIndex = targetFile.lastIndexOf('.');
        fileNameInput->setText(targetFile.left(extIndex));
        fileContentInput->setText((*m_storage)[targetFile]);
    }
}

void FileSystemApp::deleteFile() {
    QListWidgetItem *selected = fileList->currentItem();
    if (!selected) return;

    QString targetFile = selected->text();
    m_storage->remove(targetFile); // Wipe index out of the map layer
    
    fileNameInput->clear();
    fileContentInput->clear();
    refreshFileList();
}

void FileSystemApp::refreshFileList() {
    fileList->clear();
    // Re-index layout matrix entries directly out of the shared persistent container maps
    for (auto it = m_storage->begin(); it != m_storage->end(); ++it) {
        fileList->addItem(it.key());
    }
}