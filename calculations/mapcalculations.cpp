#include "mapcalculations.h"

vrsa::calculations::MapCalculator::MapCalculator()
    : isGeographicCRS{}
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
    double conversionFactor = 0;
    double delta = 0;
    calculateMetrics( mapExtent, delta, conversionFactor );
    const double scale = ( delta * conversionFactor ) / ( static_cast< double >( canvasWidth ) / mDpi );
    return scale;
}

double vrsa::calculations::MapCalculator::calculateGeographicDistance(const QRectF &mapExtent) const
{
    QPointF rect_c= mapExtent.center();
    double xMin = rect_c.x()-mapExtent.width()/2.0;
    double xMax=xMin+mapExtent.width();
    double yMax = rect_c.y()-mapExtent.height()/2.0;
    double yMin = yMax - mapExtent.height();
    const double lat = ( yMax + yMin) * 0.5; //среднее значение
    static const double RADS = ( 4.0 * std::atan( 1.0 ) ) / 180.0;
    const double a = std::pow( std::cos( lat * RADS ), 2 );
    const double c = 2.0 * std::atan2( std::sqrt( a ), std::sqrt( 1.0 - a ) );
    static const double RA = 6378000; // [m]
    // Эксцентриситет sqrt(1.0 - rb*rb/(ra*ra)) c rb в 6357000 m.
    static const double E = 0.0810820288;
    const double radius = RA * ( 1.0 - E * E ) /
            std::pow( 1.0 - E * E * std::sin( lat * RADS ) * std::sin( lat * RADS ), 1.5 );
    const double meters = ( xMax - xMin ) / 180.0 * radius * c;
    return meters;
}

void vrsa::calculations::MapCalculator::setCRS(const gdalwrapper::SpatialReference &ref)
{
    isGeographicCRS = ref.GetOGRSpatialRef()->IsGeographic();
}



void vrsa::calculations::MapCalculator::calculateMetrics(const QRectF &mapExtent, double &delta, double &conversionFactor) const
{
    QPointF rect_c= mapExtent.center();
    double xMin = rect_c.x()-mapExtent.width()/2.0;
    double xMax=xMin+mapExtent.width();
    delta = xMax - xMin;
    conversionFactor=39.3700787; //(1/0.0254);
    if (isGeographicCRS) //если географическая ск вычисляем дистанцию по формуле Хаверсина
    {
        conversionFactor = 39.3700787; //конвертация из дюймов в метры (для dpi)
        delta = calculateGeographicDistance( mapExtent );
    }
}
