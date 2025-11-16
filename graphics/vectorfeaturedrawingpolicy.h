#ifndef FEATUREDRAWINGPOLICY_H
#define FEATUREDRAWINGPOLICY_H
#include <QPainter>
#include <ogr_geometry.h>
#include <QStyleOptionGraphicsItem>
#include "common/GisDefines.h"
#include "graphics/drawingcontext.h"
#include "graphics/vectorfeaturestyle.h"
#include "calculations/unitconverter.h"
#include <QPainterPath>
namespace vrsa
{
namespace graphics
{


class VectorFeatureDrawingPolicy
{
public:
    VectorFeatureDrawingPolicy(VectorFeatureStyle &style);
    virtual void paint(const DrawingContext& context) = 0;
    virtual vrsa::common::GeometryType getType() const = 0;
    virtual ~VectorFeatureDrawingPolicy() = default;
    virtual QRectF boundingRect(const DrawingContext& context) const = 0;
    virtual void rebuildCache(const DrawingContext& context) = 0;
    virtual void cacheGeometry(OGRGeometry *geom) = 0;
    //virtual QPainterPath geometryToPath(const DrawingContext &context) const;
protected:
    VectorFeatureStyle mStyle;
    int mMapScale;
    struct CacheData
    { //TODO ADD SCALECASHE AND PAINTERPATH CASHE
        QPainterPath path{};
        QRectF boundingRect{};
        QPen pen{};
        QBrush brush{};
        double sceneScale = -1;
        bool isValid{false};
        bool isGeomValid{false};

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
};
}
}
#endif // FEATUREDRAWINGPOLICY_H
