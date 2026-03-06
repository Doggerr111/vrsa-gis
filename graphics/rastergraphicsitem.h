#ifndef RASTERGRAPHICSITEM_H
#define RASTERGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QObject>
namespace vrsa{
namespace graphics
{
class RasterGraphicsItem :public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    using QGraphicsPixmapItem::QGraphicsPixmapItem;  // наследуем конструкторы
public slots:
    void onZValueChanged(int zValue);
};
}
}

#endif // RASTERGRAPHICSITEM_H
