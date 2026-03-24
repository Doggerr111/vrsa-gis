#ifndef MAPCALCULATIONS_H
#define MAPCALCULATIONS_H
#include <QObject>
#include <QRectF>
#include "spatialref/spatialreference.h"
#include "common/logger.h"
#include <numeric>
namespace vrsa
{
namespace calculations
{
enum class MeasurementType {
    Cartesian,   //на плоскости (декартово)
    Geodesic     //на эллипсоиде (геодезическое)
};

class MapCalculator
{

public:
    explicit MapCalculator();
public:
    inline void SetDpi(double dpi) noexcept
    {
        mDpi = dpi;
    }
    inline double dpi() const noexcept
    {
        return mDpi;
    }
   /**
    * mapExtent - видимая область графической сцены
    * canvasWidth - ширина виджета MapHolder в пикселях
    */
    double calculate( const QRectF &mapExtent, double canvasWidth ) const;
   /**
    *  mapExtent видимая область графической сцены
    */
    double calculateGeographicDistance( const QRectF &mapExtent ) const;
    void setCRS(const spatialref::SpatialReference& CRS);
private:
    void calculateMetrics( const QRectF &mapExtent, double &delta) const;
private:
    double mDpi;
    bool isGeographicCRS;


public:
    static double calculateDistance(const QPointF& first, const QPointF& second,
                              MeasurementType type = MeasurementType::Cartesian,
                                    const std::string& ellipsoidName = "Unknown");

    static double calculateGeographicDistance(const QPointF& first, const QPointF& second, double r);

private:
    static void calculateMetrics(const QPointF& first, const QPointF& secon, double& delta, bool isGeographicCRS);



};
}
}

#endif // MAPCALCULATIONS_H
