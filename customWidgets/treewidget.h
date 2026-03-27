#ifndef LIPTREEWIDGET_H
#define LIPTREEWIDGET_H
#include <QTreeWidget>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QObject>

/**
 * @brief Древовидный виджет для отображения слоёв проекта
 */
class TreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    TreeWidget(QWidget* parent);
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
