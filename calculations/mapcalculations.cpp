#include "mapcalculations.h"
#include "spatialref/spatialrefdatabase.h"
vrsa::calculations::MapCalculator::MapCalculator()
    : isGeographicCRS{true}
{

}



double vrsa::calculations::MapCalculator::calculate(const QRectF &mapExtent, double canvasWidth) const
{
    if (canvasWidth == 0 || mDpi <= 0) return 1.0;
    if (mapExtent.width() <= 0 || mapExtent.height() <= 0) return 1.0;
    double delta = 0;
    calculateMetrics(mapExtent, delta);
    if (delta <= 0) return 1.0;
    double metersPerPixel = delta / canvasWidth; //сколько метров в пикселе
    if (metersPerPixel <= 0) return 1.0;
    if (metersPerPixel > 1e9) return 1e9;  // слишком большой масштаб
    double pixelsPerCm = mDpi / 2.54; //сколько пикселей в 1 см (физически)
    //метров в 1 см на экране
    double metersPerCmOnScreen = metersPerPixel * pixelsPerCm;
    double scaleDenominator = metersPerCmOnScreen * 100.0;
    scaleDenominator = std::clamp(1.0, scaleDenominator, 1e9);
    if (!std::isfinite(scaleDenominator)) return 1.0;
    return scaleDenominator;
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
    //эксцентриситет sqrt(1.0 - rb*rb/(ra*ra)) c rb в 6357000 m.
    static const double E = 0.0810820288;
    const double radius = RA * ( 1.0 - E * E ) /
            std::pow( 1.0 - E * E * std::sin( lat * RADS ) * std::sin( lat * RADS ), 1.5 );
    const double meters = ( xMax - xMin ) / 180.0 * radius * c;
    return meters;
}

void vrsa::calculations::MapCalculator::setCRS(const spatialref::SpatialReference &ref)
{
    isGeographicCRS = ref.isGeographic();
}

double vrsa::calculations::MapCalculator::calculateDistance(const QPointF &first, const QPointF &second, MeasurementType type,
                                                            const std::string &ellipsoidName)
{
    switch (type)
    {
    case MeasurementType::Cartesian:
    {
        return std::sqrt((std::pow(second.x()-first.x(), 2)) + (std::pow(second.y()-first.y(), 2)));
    }
    case MeasurementType::Geodesic:
    {
        auto ellipsoids = spatialref::SpatialReferenceDatabase::instance().getPredefinedEllipsoids();
        auto it = std::find_if(ellipsoids.begin(), ellipsoids.end(), [ellipsoidName]
                     (const auto& ellips){
            return ellips.name == ellipsoidName;
        });
        if (it != ellipsoids.end())
            return calculateGeographicDistance(first, second, it->a);
        else
        {
            VRSA_ERROR("CORE", "Can't find ellipsoid:" + ellipsoidName + " . Using cartesian calculation");
            return std::sqrt((std::pow(second.x()-first.x(), 2)) + (std::pow(second.y()-first.y(), 2)));
        }
    }


    }

}

double vrsa::calculations::MapCalculator::calculateGeographicDistance(const QPointF &first,
                                                                      const QPointF &second, double r)
{
    //градусы в радианы
    double lat1 = first.y() * M_PI / 180.0;
    double lon1 = first.x() * M_PI / 180.0;
    double lat2 = second.y() * M_PI / 180.0;
    double lon2 = second.x() * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    //формула гаверсинусов
    double a = sin(dlat / 2) * sin(dlat / 2) +
            cos(lat1) * cos(lat2) *
            sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));


    return r * c;
}

void vrsa::calculations::MapCalculator::calculateMetrics(const QPointF &first,
                                          const QPointF &second, double &delta, bool isGeographicCRS)
{
//    if (isGeographicCRS) //если географическая ск вычисляем дистанцию по формуле Хаверсина
//    {
//        delta = calculateGeographicDistance(first, second);
//        return;
//    }
//    delta = std::sqrt((std::pow(second.x()-first.x(), 2)) + (std::pow(second.y()-first.y(), 2)));
}



void vrsa::calculations::MapCalculator::calculateMetrics(const QRectF &mapExtent, double &delta) const
{
    if (isGeographicCRS) //если географическая ск вычисляем дистанцию по формуле Хаверсина
    {
        delta = calculateGeographicDistance(mapExtent);
        return;
    }
    QPointF rect_c= mapExtent.center();
    double xMin = rect_c.x()-mapExtent.width()/2.0;
    double xMax=xMin+mapExtent.width();
    delta = xMax - xMin;
}


//double vrsa::calculations::MapCalculator::calculateScaleFactor(double scale, const QRectF &mapExtent, double canvasWidth)
//{
//    if (scale <= 0.0 || std::isnan(scale) || std::isinf(scale))
//            return 1.0;
//    if (!mapExtent.isValid() || mapExtent.width() <= 0 || mapExtent.height() <= 0)
//            return 1.0;
//    if (!std::isfinite(canvasWidth) || canvasWidth <= 0.0 || canvasWidth > 1e6)
//            return 1.0;
//    if (!mapExtent.isValid() || !std::isfinite(mapExtent.width()) ||  mapExtent.width() <= 0.0 ||
//                                          mapExtent.width() > 1e9 || mapExtent.height() <= 0)
//            return 1.0;

//    double conversionFactor = 1/0.0254; //1 дюйм = 0.0254 метра
//    QPointF rect_c= mapExtent.center();
//    double xMin = rect_c.x()-mapExtent.width()/2.0;
//    double xMax=xMin+mapExtent.width();
//    double currentDist = xMax - xMin;
//    if (isGeographicCRS) //если географическая ск вычисляем дистанцию по формуле Хаверсина
//    {
//        currentDist = calculateGeographicDistance(mapExtent);
//        conversionFactor = 39.3700787;
//        if (currentDist < 1.0 || currentDist > 1e9)
//        {
//            VRSA_DEBUG("CORE", "Расстояние слишком мало:" + std::to_string(currentDist));
//            currentDist = 1.0;
//        }
//    }

//    //получаем расстояние в метрах от левого края холста карты до правого при заданном масштабе
//    const double distance = scale * (canvasWidth / mDpi) / conversionFactor;
//    if (distance<=0.0)
//        return 1;

//    //double currentDist = calculateGeographicDistance(mapExtent); //вычисляем расстояние для текущего экстента
//    double scaleFactor = currentDist/distance; //коээфицент масштаба
//    //qDebug()<<QString::number(currentDist/distance,'f',2);
//    if (scaleFactor > 1e9)
//        return 1e9;

//    if (scaleFactor < 1e-9)
//        return 1e-9;

//    return scaleFactor;
//}
