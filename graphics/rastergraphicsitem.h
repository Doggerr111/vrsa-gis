#ifndef RASTERGRAPHICSITEM_H
#define RASTERGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QObject>
namespace vrsa{
namespace gdalwrapper{
class Dataset;
}
namespace graphics
{
class RasterGraphicsItem :public QObject
{
    Q_OBJECT
public:
    RasterGraphicsItem(gdalwrapper::Dataset *dS = nullptr);
    void setPixmap(const QPixmap& pixmap);
    void setTransform(const QTransform& transform);
    QGraphicsPixmapItem* getPixmapGraphicsItem() noexcept {return mItem.get(); };
public slots:
    void onZValueChanged(int zValue);
protected:
    std::unique_ptr<QGraphicsPixmapItem> mItem;
private:
    gdalwrapper::Dataset* mBaseDs;
};
}
}

#endif // RASTERGRAPHICSITEM_H
