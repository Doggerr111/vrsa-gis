#include "mapcalculations.h"

vrsa::calculations::MapCalculator::MapCalculator(QObject *parent)
    : QObject(parent),
      isGeographicCRS{}
{

}

double vrsa::calculations::MapCalculator::calculateScaleFactor(double scale, const QRectF &mapExtent, double canvasWidth)
{
    if (scale<=0.0)
            return 1;
        double conversionFactor = 1/0.0254;
        QPointF rect_c= mapExtent.center();
        double xMin = rect_c.x()-mapExtent.width()/2.0;
        double xMax=xMin+mapExtent.width();
        double currentDist = xMax - xMin;
        if (isGeographicCRS) //если географическая ск вычисляем дистанцию по формуле Хаверсина
        {
            currentDist = calculateGeographicDistance(mapExtent);
            conversionFactor = 39.3700787;
        }

        //получаем расстояние в метрах от левого края холста карты до правого при заданном масштабе
        const double distance = scale * (canvasWidth / mDpi) / conversionFactor;
        if (distance<=0.0)
            return 1;

        //double currentDist = calculateGeographicDistance(mapExtent); //вычисляем расстояние для текущего экстента
        double scaleFactor = currentDist/distance; //коээфицент масштаба
        //qDebug()<<QString::number(currentDist/distance,'f',2);
        if (scaleFactor<=std::numeric_limits<double>::max())
            return scaleFactor;
        return 1;
}

double vrsa::calculations::MapCalculator::calculate(const QRectF &mapExtent, double canvasWidth) const
{

}

double vrsa::calculations::MapCalculator::calculateGeographicDistance(const QRectF &mapExtent) const
{

}

void vrsa::calculations::MapCalculator::onCRSChanged(OGRSpatialReference *ref)
{
    if (!ref)
    {
        VRSA_ERROR("MapCalculator", "nullptr OGRSpatialReference");
        return;
    }
    isGeographicCRS = ref->IsGeographic();
}

void vrsa::calculations::MapCalculator::calculateMetrics(const QRectF &mapExtent, double &delta, double &conversionFactor) const
{

}
