#include "webrastergraphicsitem.h"
#include "raster/webrasterdataset.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
vrsa::graphics::WebRasterGraphicsItem::WebRasterGraphicsItem(raster::WebRasterDataset *webDs)
    : RasterGraphicsItem(webDs),
      mDataset{webDs}
{
    if (mDataset)
    {
        QObject::connect(mDataset, &raster::WebRasterDataset::ZValueChanged, this, &RasterGraphicsItem::onZValueChanged);
        QObject::connect(mDataset, &raster::WebRasterDataset::visibilityChanged, this, &RasterGraphicsItem::onVisibilityChanged);
    }
}

vrsa::graphics::WebRasterGraphicsItem::~WebRasterGraphicsItem()
{
    mCancelled = true;
}

void vrsa::graphics::WebRasterGraphicsItem::notifyExtentChanged(const QRectF &mapExtent, const QRect &widgetRect)
{
    if (!mDataset || mCancelled) return;

    QPointer<WebRasterGraphicsItem> safeThis = this;
    QGraphicsPixmapItem* itemPtr = mItem.get();
    auto extent = mapExtent;
    int width = widgetRect.width();
    int height = widgetRect.height();
    QRectF transformExtent;
    //читаем тайлы в отдельном потоке
    QtConcurrent::run([=]() mutable {
        if (!safeThis) return;
        if (safeThis->mCancelled) return;
        QImage tile = safeThis->mDataset->readExtent(extent, width, height, transformExtent);
        if (!safeThis) return;
        //возвращаемся в gui-поток
        QMetaObject::invokeMethod(this, [=]() {
            if (!safeThis) return;
            if (safeThis->mCancelled) return;
            if (tile.isNull()) return;

            QPixmap pixmap = QPixmap::fromImage(tile);
            if (itemPtr)
            {
                itemPtr->setPixmap(pixmap);
                itemPtr->setPos(transformExtent.left(), transformExtent.top());

                double scaleX = transformExtent.width() / pixmap.width();
                double scaleY = transformExtent.height() / pixmap.height();
                QTransform transform;
                transform.scale(scaleX, scaleY);
                itemPtr->setTransform(transform);
            }
            //mItem->update();
        });
    });
}


