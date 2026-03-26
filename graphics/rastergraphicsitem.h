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
    virtual ~RasterGraphicsItem();
    void setPixmap(const QPixmap& pixmap);
    void setTransform(const QTransform& transform);
    QGraphicsPixmapItem* getPixmapGraphicsItem() noexcept {return mItem.get(); };
    gdalwrapper::Dataset* getDataset() const noexcept { return mBaseDs; };
public slots:
    void onZValueChanged(int zValue);
    void onVisibilityChanged(bool visible);
    void onVisibleExtentChanged(const QRectF& mapExtent, const QRect& widgetRect);
protected:
    virtual void notifyExtentChanged(const QRectF& mapExtent, const QRect& widgetRect){};
    std::unique_ptr<QGraphicsPixmapItem> mItem;
private:
    gdalwrapper::Dataset* mBaseDs;
};
}
}

#endif // RASTERGRAPHICSITEM_H
