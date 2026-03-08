#include "graphics/rubberbanditem.h"
#include "graphics/vertexhandle.h"
#include "graphics/featuregraphicsitem.h"
#include "gdal/ogrconverter.h"
#include <QGraphicsScene>
vrsa::graphics::RubberBandItem::RubberBandItem(common::GeometryType geomType)
    : TemporaryGraphicsItem(geomType, TemporaryItemRole::RubberBand),
      mAnchorPoints{},
      mTargetFeatureItem{}
{
    setZValue(common::MAX_Z_VALUE+1);
    //    static_assert(std::is_polymorphic_v<TemporaryGraphicsItem>,
    //                  "Base must be polymorphic");
    //    static_assert(std::is_polymorphic_v<RubberBandItem>,
    //                  "we must be polymorphic...");
}

vrsa::graphics::RubberBandItem::~RubberBandItem()
{
    clearVertexes();
}

void vrsa::graphics::RubberBandItem::setTargetItem(FeatureGraphicsItem *target)
{
    if (!target)
        return;
    if (mTargetFeatureItem)
    {
        disconnect(this, &RubberBandItem::rubberBandGeometryChanged,
                   mTargetFeatureItem, &FeatureGraphicsItem::onGeometryChanged);
        clearVertexes();
    }
    mTargetFeatureItem = target;
    auto featureOGRGeom = mTargetFeatureItem->getFeatureGeometry();
    if (!featureOGRGeom)
        return;
    auto geometry = ogr_utils::OGRConverter::fromOGR(featureOGRGeom);
    mTargetOriginalGeometry = geometry;
    switch (geometry.type)
    {
    case common::GeometryType::Point:
    {
        auto* point = std::get_if<QPointF>(&geometry.variant);
        auto vertex = std::make_unique<VertexHandle>(this);
        auto mapScene = scene();
        qDebug()<< point;
        if (!point || !vertex)
            return;
        if (mapScene)
            mapScene->addItem(vertex.get());
        else
        {
            VRSA_DEBUG("RubberBandItem", "Error while setting mTargetFeatureItem item:"
                                         "Can't add anchor point to rubber band, before setting mTargetFeatureItem item"
                                         "make sure rubber band item added to the scene");
            return;
        }
        vertex->setPoint(*point);
        connect(vertex.get(), &VertexHandle::geometryChanged, this, &RubberBandItem::onVertexGeometryChanged);
        mAnchorPoints.push_back(std::move(vertex));
        break;
    }
    case common::GeometryType::LineString:
    case common::GeometryType::Polygon:
    {
        if (auto points = std::get_if<std::vector<QPointF>>(&geometry.variant))
        {
            //qDebug()<<*points;
            if (!points->empty() && points->front() == points->back())
                points->pop_back();
            for (const auto& point: *points)
            {
                //TODO добавить точки по центру  между набором из 2 координатых пар
                auto vertex = std::make_unique<VertexHandle>(this);
                if (!vertex)
                    return;
                //добавляем здесь стандартным методом, потому что их временем жизни будет управлять родитель (мы),
                //который добавляется методом класса MapScene и в любом случае будет очищен правильно
                if (scene())
                    scene()->addItem(vertex.get());
                else
                {
                    VRSA_DEBUG("RubberBandItem", "Error while setting mTargetFeatureItem item:"
                                                 "Can't add anchor points to rubber band, before setting mTargetFeatureItem item"
                                                 "make sure rubber band item added to the scene");
                    return;
                }
                vertex->setPoint(point);
                connect(vertex.get(), &VertexHandle::geometryChanged, this, &RubberBandItem::onVertexGeometryChanged);
                connect(vertex.get(), &VertexHandle::released, this, &RubberBandItem::onVertexReleased);
                mAnchorPoints.push_back(std::move(vertex));
            }
        }
        else if (auto pointsWithHoles = std::get_if<std::vector<std::vector<QPointF>>>(&geometry.variant))
        {
            if (pointsWithHoles)
            {
                VRSA_DEBUG("RubberBandItem", "Currently creating rubber band for polygons with holes not supported");
                //qDebug()<<*pointsWithHoles;
            }
        }
        break;
    }
    default:
        VRSA_DEBUG("RubberBandItem", "Currently creating rubber band for mTargetFeatureItem feature geometry type not supported");
        break;
    }
    connect(this, &RubberBandItem::rubberBandGeometryChanged, mTargetFeatureItem, &FeatureGraphicsItem::onGeometryChanged);
    setScale(mScale);
    setGeometry(geometry);
    update();
}


QRectF vrsa::graphics::RubberBandItem::boundingRect() const
{
    return TemporaryGraphicsItem::boundingRect();
}

void vrsa::graphics::RubberBandItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    TemporaryGraphicsItem::paint(painter, option, widget);
}

void vrsa::graphics::RubberBandItem::updateGeometry()
{
    geometry::Geometry geom;
    geom.type = mGeomType;
    if (mGeomType == common::GeometryType::Point)
    {
        try {
        geom.variant = mAnchorPoints.at(0)->getPoint();
        }
        catch (const std::out_of_range& e) {
            VRSA_DEBUG("RubberBandItem", std::string("Exception while updating geometry for Point "
                                                     "Feature rubber band. Exception:") + e.what());
        }
    }
    else
    {
        std::vector<QPointF> points;
        points.reserve(mAnchorPoints.size());
        for (const auto& vertex: mAnchorPoints)
            points.push_back(vertex->getPoint());
        geom.variant = std::move(points);
    }
    setGeometry(geom);
    emit rubberBandGeometryChanged(geom);



}

void vrsa::graphics::RubberBandItem::clearVertexes()
{

    if (auto mapscene = scene())
    {
        for (const auto& vertex: mAnchorPoints)
        {
            if (vertex)
                mapscene->removeItem(vertex.get());
        }
    }
    mAnchorPoints.clear();
}

void vrsa::graphics::RubberBandItem::setScale(const double scale)
{
    TemporaryGraphicsItem::setScale(scale);
    for (const auto& vertex: mAnchorPoints)
        vertex->setScale(scale);
}

void vrsa::graphics::RubberBandItem::onVertexGeometryChanged(const QPointF &point)
{
    qDebug()<<point;
    updateGeometry();

}

void vrsa::graphics::RubberBandItem::onVertexReleased()
{
    geometry::Geometry geom;
    geom.type = mGeomType;
    if (mGeomType == common::GeometryType::Point)
    {
        try {
        geom.variant = mAnchorPoints.at(0)->getPoint();
        }
        catch (const std::out_of_range& e) {
            VRSA_DEBUG("RubberBandItem", std::string("Exception while updating geometry for Point "
                                                     "Feature rubber band. Exception:") + e.what());
        }
    }
    else
    {
        std::vector<QPointF> points;
        points.reserve(mAnchorPoints.size());
        for (const auto& vertex: mAnchorPoints)
            points.push_back(vertex->getPoint());
        geom.variant = std::move(points);
    }
    qDebug()<<"RubberBandItem::onVertexReleased()";
    if (mTargetFeatureItem)
        vertexReleased(mTargetFeatureItem, geom);
}
