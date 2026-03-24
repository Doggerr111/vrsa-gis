#include "treewidget.h"
#include <QDebug>

TreeWidget::TreeWidget(QWidget* parent)
    :   QTreeWidget(parent)
{

}

void TreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
}

void TreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTreeWidget::mouseMoveEvent(event);
}

void TreeWidget::dropEvent(QDropEvent *event)
{
    bool accepted = true;
    emit itemDropped(event, &accepted);
    expandAll();
}


void TreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeWidget::dragEnterEvent(event);
}


bool TreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{
    if (parent)
        return false;
    return QTreeWidget::dropMimeData(parent, index, data, action);
}


void TreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QTreeWidget::dragLeaveEvent(event);
    event->accept();
}

void TreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    bool accepted = true;
    itemDragRequested(event, &accepted);
    if (accepted)
        QTreeWidget::dragMoveEvent(event);
}


void TreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);
}
