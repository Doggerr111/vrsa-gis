#ifndef LIPTREEWIDGET_H
#define LIPTREEWIDGET_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QObject>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    TreeWidget(QWidget* parent);
    void moveItem(QTreeWidgetItem* source, QTreeWidgetItem* target, bool insertAfter)
    {
        QTreeWidgetItem* parent = target->parent();
        int index = parent ? parent->indexOfChild(target) : indexOfTopLevelItem(target);

        if (insertAfter) index++;

        // Извлекаем
        QTreeWidgetItem* taken = source->parent() ?
            source->parent()->takeChild(source->parent()->indexOfChild(source)) :
            takeTopLevelItem(indexOfTopLevelItem(source));

        // Вставляем
        if (parent) {
            parent->insertChild(index, taken);
        } else {
            insertTopLevelItem(index, taken);
        }
    }
signals:
    void itemDragRequested(QDragMoveEvent*, bool*);
    void itemDropped(QDropEvent*, bool*);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;


};

#endif // LIPTREEWIDGET_H
