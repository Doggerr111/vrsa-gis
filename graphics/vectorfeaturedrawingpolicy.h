#ifndef FEATUREDRAWINGPOLICY_H
#define FEATUREDRAWINGPOLICY_H
#include <QPainter>
#include <ogr_geometry.h>
#include <QStyleOptionGraphicsItem>
#include "common/GisDefines.h"
#include "graphics/drawingcontext.h"
#include "graphics/symbols/symbol.h"
#include "calculations/unitconverter.h"
#include <QPainterPath>
namespace vrsa
{
namespace graphics
{


class VectorFeatureDrawingPolicy
{
public:
    VectorFeatureDrawingPolicy();
    virtual void paint(const DrawingContext& context) = 0;
    virtual vrsa::common::GeometryType getType() const = 0;
    virtual ~VectorFeatureDrawingPolicy() = default;
    virtual QRectF boundingRect(const DrawingContext& context) const = 0;
    virtual void rebuildCache() noexcept
    {
        mCache.isValid=false;
        mCache.isGeomValid=false;
        mCache.isBoundingRectValid=false;
    }
    virtual void cacheGeometry(OGRGeometry *geom) const = 0;
    //метод для изменения отрисовки при выборе графического объекта (вызывается из renderer, как правило)
    virtual void setSelection(bool selected) { mIsSelected = selected; };
    //virtual QPainterPath geometryToPath(const DrawingContext &context) const;
protected:
    int mMapScale;
    struct CacheData
    { //TODO ADD SCALECASHE AND PAINTERPATH CACHE
        QPainterPath path{};
        QRectF boundingRect{};
        QPen pen{};
        QBrush brush{};
        double sceneScale = -1;
        bool isValid{false};
        bool isGeomValid{false};
        bool isBoundingRectValid{false};

//        void update(const QPainterPath& newPath, const QRectF& newRect,
//                    const QPen& newPen, qreal newScale) {
//            path = newPath;
//            boundingRect = newRect;
//            pen = newPen;
//            sceneScale = newScale;
//            isValid = true;
//        }
    };

    mutable CacheData mCache;
    mutable bool mIsSelected{false};
};
}
}
#endif // FEATUREDRAWINGPOLICY_H
