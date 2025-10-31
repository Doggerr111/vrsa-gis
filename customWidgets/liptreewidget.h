#ifndef LIPTREEWIDGET_H
#define LIPTREEWIDGET_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QObject>

class LIPTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    LIPTreeWidget(QWidget* parent);
signals:
    void itemDropped();
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dropEvent(QDropEvent *event);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);

    // QTreeWidget interface
protected:
    bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);

    // QWidget interface
protected:
    void dragLeaveEvent(QDragLeaveEvent *event);

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // LIPTREEWIDGET_H
