#ifndef CONTEXTMENUMANAGER_H
#define CONTEXTMENUMANAGER_H

#include <QObject>
#include <QPoint>
#include <QMdiArea>

class Desktop; // Forward declaration to prevent circular dependency loops

class ContextMenuManager : public QObject {
    Q_OBJECT
public:
    explicit ContextMenuManager(Desktop *desktop, QMdiArea *workspace);

private slots:
    void handleContextMenuRequest(const QPoint &pos);
    void triggerRefresh();

private:
    Desktop *m_desktop;
    QMdiArea *m_workspace;
};

#endif // CONTEXTMENUMANAGER_H