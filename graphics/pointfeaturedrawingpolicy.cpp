#include "pointfeaturedrawingpolicy.h"

vrsa::graphics::PointFeatureDrawingPolicy::PointFeatureDrawingPolicy(VectorFeatureStyle &style)
    : VectorFeatureDrawingPolicy(style),
      mX{},
      mY{}
{

}

void vrsa::graphics::PointFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom)
{
    OGRPoint* point = static_cast<OGRPoint*>(geom);
    mX = point->getX();
    mY = point->getY();
    mCache.isGeomValid = true;
}

void vrsa::graphics::PointFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{

}

//QPainterPath vrsa::graphics::PointFeatureDrawingPolicy::geometryToPath(const DrawingContext &context) const
//{
//    OGRPoint* point = static_cast<OGRPoint*>(context.geom);
//    // Сохраняем данные для пересоздания пути
//    double x = point->getX();
//    double y = point->getY();
//    double pointSize = calculations::UnitConversion::mmToPixels(mStyle.getPointSize());

//    // Создаем QPainterPath
//    mCache.path = QPainterPath();
//    double radius = (pointSize / 2.0); // /context.sceneScale;
//    mCache.path.addEllipse(QPointF(x, y), radius, radius);
//    mCache.isValid=true;
//    return mCache.path;
//}

void vrsa::graphics::PointFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    //VRSA_DEBUG("PointFeatureDrawingPolicy", "paint called");
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);
    if (mCache.sceneScale != context.sceneScale) {
        double pointSize = calculations::UnitConversion::mmToPixels(mStyle.getPointSize());
        mCache.path = QPainterPath();
        double radius = (pointSize / 2.0) / context.sceneScale;  // Масштаб СРАЗУ в расчете!
        mCache.path.addEllipse(QPointF(mX, mY), radius, radius);
        mCache.sceneScale = context.sceneScale;
    }
    context.painter->setPen(mStyle.getPen());
    QBrush brush = mStyle.getBrush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);
    context.painter->drawPath(mCache.path);

//    OGRPoint* point = static_cast<OGRPoint*>(context.geom);
//    mX = point->getX();
//    mY = point->getY();
//    mMapScale = context.sceneScale;
//    mPointSize= calculations::UnitConversion::mmToPixels(mStyle.getPointSize());
//    context.painter->drawEllipse(QPointF(mX, mY), (mPointSize/2)/mMapScale,
//                                 (mPointSize/2)/mMapScale);



//    if (mIsSelected)
//    {
//        LIPVectorStyle newStyle = LIPVectorStyle::getSelectedStyle(mStyle);
//        QPen pen = newStyle.getPen();
//        pen.setWidthF(pen.widthF()/mSceneScale);
//        painter->setPen(pen);
//        QBrush brush = newStyle.getBrush();
//        brush.setTransform(QTransform(painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
//        painter->setBrush(brush);
//        mPointSize=LIPVectorStyle::MMToPixel(newStyle.getPointSize());;
//    }
//    else
//    {
//        QPen pen = mStyle->getPen();
//        pen.setWidthF(pen.widthF()/mSceneScale);
//        painter->setPen(pen);
//        QBrush brush = mStyle->getBrush();
//        brush.setTransform(QTransform(painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
//        painter->setBrush(brush);
//        mPointSize=LIPVectorStyle::MMToPixel(mStyle->getPointSize());;
//    }


//    switch (mStyle->getPointType())
//    {
//    case Circle:
//        painter->drawEllipse(QPointF(p->x(), p->y()), (mPointSize/2)/mSceneScale, (mPointSize/2)/mSceneScale);
//        break;
//    case Square:
//        painter->drawRect(QRectF(p->x()-(mPointSize/2)/mSceneScale, p->y()-(mPointSize/2)/mSceneScale,
//                                 (mPointSize)/mSceneScale, (mPointSize)/mSceneScale));
//        break;
//    case Triangle:
//    {
////        QPointF point1(p->x()+pointSize/2,p->y()-(pointSize)/mSceneScale);
////        QPointF point2(p->x()-(pointSize/2)/mSceneScale, p->y());
////        QPointF point3(p->x()-(pointSize)/mSceneScale,p->y()-(pointSize)/mSceneScale);
//        QPointF point1(p->x()-(mPointSize/2)/mSceneScale,p->y()-(mPointSize/2)/mSceneScale);
//        QPointF point2(p->x(), p->y()+(mPointSize/2)/mSceneScale);
//        QPointF point3(p->x()+(mPointSize/2)/mSceneScale,p->y()-(mPointSize/2)/mSceneScale);
//        QVector<QPointF> points{point1,point2,point3};
//        QPolygonF pol(std::move(points));
//        painter->drawPolygon(pol);
//        break;
//    }
//    case InvertedTriangle:
//    {
//        QPointF point1(p->x()+(mPointSize/2)/mSceneScale,p->y()+(mPointSize/2)/mSceneScale);
//        QPointF point2(p->x(), p->y()-(mPointSize/2)/mSceneScale);
//        QPointF point3(p->x()-(mPointSize/2)/mSceneScale,p->y()+(mPointSize/2)/mSceneScale);
//        QVector<QPointF> points{point1,point2,point3};
//        QPolygonF pol(std::move(points));
//        painter->drawPolygon(pol);
//        break;
//    }
//    case Rhombus:
//    {
//        QPointF point1(p->x()-(mPointSize/2)/mSceneScale,p->y());
//        QPointF point2(p->x(), p->y()+(mPointSize/2)/mSceneScale);
//        QPointF point3(p->x()+(mPointSize/2)/mSceneScale,p->y());
//        QPointF point4(p->x(), p->y()-(mPointSize/2)/mSceneScale);
//        QVector<QPointF> points{point1,point2,point3, point4};
//        QPolygonF pol(std::move(points));
//        painter->drawPolygon(pol);
//        break;
//    }
//    case CustomImage:
//    {

//        painter->drawImage(QRectF(p->x()-(mPointSize/2)/mSceneScale, p->y()-(mPointSize/2)/mSceneScale,
//                                  (mPointSize)/mSceneScale, (mPointSize)/mSceneScale), *mStyle->getCustomImage());
//    }

}

vrsa::common::GeometryType vrsa::graphics::PointFeatureDrawingPolicy::getType() const
{
    return vrsa::common::GeometryType::Point;
}


QRectF vrsa::graphics::PointFeatureDrawingPolicy::boundingRect(const DrawingContext& context) const
{
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
        return mCache.boundingRect;
    double scale = context.sceneScale;
    double size2 = mStyle.getPointSize()/scale;
    double size = (mStyle.getPointSize()/2) /scale;
    return  mCache.boundingRect = QRectF(mX-size, mY-size,
                  size2,size2);

}

vrsa::graphics::MultiPointFeatureDrawingPolicy::MultiPointFeatureDrawingPolicy(VectorFeatureStyle &style)
    : VectorFeatureDrawingPolicy(style)
{

}

void vrsa::graphics::MultiPointFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom)
{

    OGRMultiPoint* multiPoint = (OGRMultiPoint*)geom;
    int pointCount = multiPoint->getNumGeometries();
    mPoints.reserve(pointCount);
    for (int i = 0; i < pointCount; ++i)
    {
        OGRPoint* point = (OGRPoint*)multiPoint->getGeometryRef(i);
        mPoints.push_back(QPointF(point->getX(), point->getY()));
        mCache.isGeomValid = true;
    }

}
void vrsa::graphics::MultiPointFeatureDrawingPolicy::paint(const DrawingContext &context)
{

    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);
    if (mCache.sceneScale != context.sceneScale) {
        double pointSize = calculations::UnitConversion::mmToPixels(mStyle.getPointSize());
        mCache.path = QPainterPath();
        double radius = (pointSize / 2.0) / context.sceneScale;  // Масштаб СРАЗУ в расчете!
        for (const auto& point: mPoints)
            mCache.path.addEllipse(point, radius, radius);
        mCache.sceneScale = context.sceneScale;
    }
    context.painter->setPen(mStyle.getPen());
    QBrush brush = mStyle.getBrush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);
    context.painter->drawPath(mCache.path);

//    OGRGeometry* poGeometry = context.geom;

//            if (poGeometry != nullptr && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPoint) {
//                OGRMultiPoint* multiPoint = (OGRMultiPoint*)poGeometry;

//                // Работа с MultiPoint
//                int pointCount = multiPoint->getNumGeometries();
//                for (int i = 0; i < pointCount; i++) {
//                    OGRPoint* point = (OGRPoint*)multiPoint->getGeometryRef(i);
//                    double x = point->getX();
//                    double y = point->getY();
//                }
//            }
}

vrsa::common::GeometryType vrsa::graphics::MultiPointFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::MultiPoint;
}



QRectF vrsa::graphics::MultiPointFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
{
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
            return mCache.boundingRect;

        OGRMultiPoint* multiPoint = static_cast<OGRMultiPoint*>(context.geom);
        double scale = context.sceneScale;
        double pointSize = mStyle.getPointSize() / scale;
        double halfSize = pointSize / 2;

        // Если точек нет - возвращаем пустой rect
        if (multiPoint->getNumGeometries() == 0) {
            return mCache.boundingRect = QRectF();
        }

        // Находим min/max координаты всех точек
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double maxY = std::numeric_limits<double>::lowest();

        for (int i = 0; i < multiPoint->getNumGeometries(); i++) {
            OGRPoint* point = static_cast<OGRPoint*>(multiPoint->getGeometryRef(i));
            double x = point->getX();
            double y = point->getY();

            minX = std::min(minX, x - halfSize);
            minY = std::min(minY, y - halfSize);
            maxX = std::max(maxX, x + halfSize);
            maxY = std::max(maxY, y + halfSize);
        }

        return mCache.boundingRect = QRectF(minX, minY, maxX - minX, maxY - minY);
}

void vrsa::graphics::MultiPointFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{
}

