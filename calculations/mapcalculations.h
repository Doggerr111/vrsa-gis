#ifndef MAPCALCULATIONS_H
#define MAPCALCULATIONS_H
#include <QObject>
#include <QRectF>
#include <gdal/spatialreference.h>
#include "common/logger.h"
#include <numeric>
namespace vrsa
{
namespace calculations
{


class MapCalculator
{

public:
    explicit MapCalculator();
public:
    inline void SetDpi(double dpi)
    {
        mDpi = dpi;
    }
    inline double dpi() const
    {
        return mDpi;
    }
    double calculateScaleFactor(double scale, const QRectF &mapExtent, double canvasWidth);
    /**
   * mapExtent - видимая область графической сцены
   * canvasWidth - ширина виджета MapHolder в пикселях
   */
    double calculate( const QRectF &mapExtent, double canvasWidth ) const;
    /** mapExtent видимая область графической сцены
   */
    double calculateGeographicDistance( const QRectF &mapExtent ) const;

    void setCRS(const gdalwrapper::SpatialReference& CRS);

private:

    void calculateMetrics( const QRectF &mapExtent, double &delta, double &conversionFactor ) const;
    double mDpi;
    bool isGeographicCRS;


};
}
}

#endif // MAPCALCULATIONS_H
