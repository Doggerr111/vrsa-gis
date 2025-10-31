#include "liptreewidget.h"
#include <QDebug>

LIPTreeWidget::LIPTreeWidget(QWidget* parent)
    :   QTreeWidget(parent)
{

}


void LIPTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
//    QTreeWidgetItem *item = itemAt(event->pos());
//            if (item != nullptr) {
//                item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
//            }
}

void LIPTreeWidget::mouseMoveEvent(QMouseEvent *event)
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

void LIPTreeWidget::dropEvent(QDropEvent *event)
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


void LIPTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //emit itemDropped();
    QTreeWidget::dragEnterEvent(event);

}


bool LIPTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{

    qDebug()<<parent->columnCount();
    return QTreeWidget::dropMimeData(parent, index, data, action);
}


void LIPTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{

    QTreeWidget::dragLeaveEvent(event);
    event->accept();
    emit itemDropped();
}


void LIPTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit itemDropped();
    QTreeWidget::mouseReleaseEvent(event);

}
