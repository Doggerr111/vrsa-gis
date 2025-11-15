#include "treewidget.h"
#include <QDebug>

TreeWidget::TreeWidget(QWidget* parent)
    :   QTreeWidget(parent)
{

}


void TreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
//    QTreeWidgetItem *item = itemAt(event->pos());
//            if (item != nullptr) {
//                item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
//            }
}

void TreeWidget::mouseMoveEvent(QMouseEvent *event)
{

    QTreeWidget::mouseMoveEvent(event);
//    if (event->buttons() == Qt::LeftButton) {
//               QTreeWidgetItem *item = currentItem();
//               if (item != nullptr && item->flags() & Qt::ItemIsDragEnabled) {
//                   QMimeData *mimeData = new QMimeData;
//                   mimeData->setText(item->text(0));
//                   QDrag *drag = new QDrag(this);
//                   drag->setMimeData(mimeData);
//                   drag->exec(Qt::MoveAction);
//               }
//           }
}

void TreeWidget::dropEvent(QDropEvent *event)
{





    QTreeWidget::dropEvent(event);
    emit itemDropped();


    //qDebug()<<event->mimeData()->imageData();
//    if (event->mimeData()->hasText()) {
//                //QTreeWidgetItem *item = new QTreeWidgetItem({event->mimeData()->imageData(), event->mimeData()->text(), ""});
//               // currentItem()->addChild(item);
//                event->accept();
//            } else {
//                event->ignore();
//            }
}


void TreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //emit itemDropped();
    QTreeWidget::dragEnterEvent(event);

}


bool TreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{

    qDebug()<<parent->columnCount();
    return QTreeWidget::dropMimeData(parent, index, data, action);
}


void TreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{

    QTreeWidget::dragLeaveEvent(event);
    event->accept();
    emit itemDropped();
}


void TreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit itemDropped();
    QTreeWidget::mouseReleaseEvent(event);

}
