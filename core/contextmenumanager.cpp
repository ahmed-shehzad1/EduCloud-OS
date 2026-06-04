#include "contextmenumanager.h"
#include "desktop.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>

ContextMenuManager::ContextMenuManager(Desktop *desktop, QMdiArea *workspace)
    : QObject(desktop), m_desktop(desktop), m_workspace(workspace) {
    
    // Configure the workspace component to listen to custom menu requests
    m_workspace->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_workspace, &QMdiArea::customContextMenuRequested, this, &ContextMenuManager::handleContextMenuRequest);
}

void ContextMenuManager::handleContextMenuRequest(const QPoint &pos) {
    QMenu contextMenu(m_desktop);
    
    // Modern Catppuccin Dark Theme Styling
    contextMenu.setStyleSheet(R"(
        QMenu { 
            background-color: #1e1e2e; color: #cdd6f4; 
            border: 1px solid #89b4fa; border-radius: 6px; padding: 5px; 
        }
        QMenu::item { padding: 6px 25px 6px 20px; background: transparent; }
        QMenu::item:selected { background-color: #45475a; border-radius: 4px; }
    )");

    QAction *refreshAction = new QAction("🔄 Refresh Desktop", m_desktop);
    connect(refreshAction, &QAction::triggered, this, &ContextMenuManager::triggerRefresh);
    contextMenu.addAction(refreshAction);

    // Render the layout right under the mouse cursor position bounds
    contextMenu.exec(m_workspace->mapToGlobal(pos));
}

void ContextMenuManager::triggerRefresh() {
    if (m_desktop) {
        m_desktop->refreshSystem();
    }
}