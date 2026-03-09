#include "graphics/rubberbanditem.h"
#include "graphics/vertexhandle.h"
#include "graphics/featuregraphicsitem.h"
#include "gdal/ogrconverter.h"
#include "calculations/geomcalculations.h"
#include <QGraphicsScene>
vrsa::graphics::RubberBandItem::RubberBandItem(common::GeometryType geomType)
    : TemporaryGraphicsItem(geomType, TemporaryItemRole::RubberBand),
      mAnchorPoints{},
      mTargetFeatureItem{}
{
    setZValue(common::MAX_Z_VALUE+1);
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
    //mGeomType = mTargetFeatureItem->getFeatureGeometryType();//ffafasfafgsadga
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
        //connect(vertex.get(), &VertexHandle::released, this, &RubberBandItem::onVertexReleased);
        mAnchorPoints.push_back(std::move(vertex));
        break;
    }
    case common::GeometryType::LineString:
    case common::GeometryType::Polygon:
    {
        if (auto points = std::get_if<std::vector<QPointF>>(&geometry.variant))
        {
            qDebug()<<*points;
            if (!points->empty() && points->front() == points->back())
                points->pop_back();
            if (!scene())
            {
                VRSA_DEBUG("RubberBandItem", "Error while setting mTargetFeatureItem item:"
                                             "Can't add anchor points to rubber band, before setting mTargetFeatureItem item"
                                             "make sure rubber band item added to the scene");
                return;
            }
            if (points->empty())
                return;
            rebuildVertexes(points);

        }
        else if (auto pointsWithHoles = std::get_if<std::vector<std::vector<QPointF>>>(&geometry.variant))
        {
            if (pointsWithHoles)
                VRSA_DEBUG("RubberBandItem", "Currently creating rubber band for polygons with holes not supported");
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

void vrsa::graphics::RubberBandItem::addVertex(const QPointF &point, bool isMidPoint)
{
    //qDebug()<<"adding vertex for point " << point ;
    auto type = isMidPoint ? VertexType::MiddlePoint : VertexType::Vertex;
    auto vertex = std::make_unique<VertexHandle>(this, type);

    scene()->addItem(vertex.get());
    vertex->setPoint(point);

    qDebug() << "Scene after add:" << vertex->scene();
    qDebug() << "Item in scene?" << scene()->items().contains(vertex.get());

    vertex->setVisible(true);
    vertex->update();

    connect(vertex.get(), &VertexHandle::geometryChanged,
            this, &RubberBandItem::onVertexGeometryChanged);
    connect(vertex.get(), &VertexHandle::released,
            this, &RubberBandItem::onVertexReleased);
    connect(vertex.get(), qOverload<>(&VertexHandle::clicked),
            this, &RubberBandItem::onVertexClicked);

    mAnchorPoints.push_back(std::move(vertex));
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
            VRSA_ERROR("RubberBandItem", std::string("Exception while updating geometry for Point "
                                                     "Feature rubber band. Exception:") + e.what());
        }
    }
    else
    {
        std::vector<QPointF> points;
        points.reserve(mAnchorPoints.size()/2 + 1);
        for (const auto& vertex: mAnchorPoints)
        {
            if (vertex->getType() == VertexType::Vertex)
                points.push_back(vertex->getPoint());
        }
        //rebuildVertexes(&points);
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
            {
                vertex->disconnect();
                mapscene->removeItem(vertex.get());
            }
        }
    }
    mAnchorPoints.clear();
}

void vrsa::graphics::RubberBandItem::hideMiddleVertexes()
{
    for (const auto& vertex: mAnchorPoints)
    {
        if (vertex->getType() == VertexType::MiddlePoint)
            vertex->hide();
    }
}

void vrsa::graphics::RubberBandItem::rebuildVertexes(const std::vector<QPointF>* points, bool initialBuildDone )
{
    clearVertexes();
    //проверка замыкания
    bool isClosed = (points->front() == points->back());
    size_t vertexCount = isClosed ? points->size() - 1 : points->size(); //убираем

    for (size_t i = 0; i < vertexCount; ++i)
    {
        //qDebug()<<"adding vertex from loop. Point:" <<points->at(i);
        addVertex(points->at(i));
        // добавляем точку посередине сегмента
        if (i < vertexCount - 1)
        {
            QPointF mid = calculations::midPoint(points->at(i), points->at(i+1));
            //qDebug()<<"adding MIDDLE vertex from loop with point:" <<mid;
            addVertex(mid, true);  // true = середина
        }
    }
    setScale(mScale);
}

void vrsa::graphics::RubberBandItem::setScale(const double scale)
{
    TemporaryGraphicsItem::setScale(scale);
    for (const auto& vertex: mAnchorPoints)
        vertex->setScale(scale);
}

void vrsa::graphics::RubberBandItem::onVertexGeometryChanged(const QPointF &point)
{
    //qDebug()<<"RubberBandItem onVertexGeometryChanged() called. Point - " << point;
    updateGeometry();

}

void vrsa::graphics::RubberBandItem::onVertexReleased()
{
    //qDebug()<<"RubberBandItem onVertexReleased() called";
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
        {
            if (vertex->getType() != VertexType::MiddlePoint)
                points.push_back(vertex->getPoint());
        }
        rebuildVertexes(&points);
        geom.variant = std::move(points);
    }
    if (mTargetFeatureItem)
        vertexReleased(mTargetFeatureItem, geom);
}

void vrsa::graphics::RubberBandItem::onVertexClicked()
{
    hideMiddleVertexes();
}
