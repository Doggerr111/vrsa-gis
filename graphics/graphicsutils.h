#ifndef GRAPHICSUTILS_H
#define GRAPHICSUTILS_H
#include <QColor>
#include "calculations/unitconverter.h"
namespace vrsa
{
namespace graphics
{


class GraphicsUtils
{
public:
    GraphicsUtils() = delete;

    static QColor generateColor();
    static double convertToPixels(double value, common::StyleUnit units)
    {
        switch (units)
        {
        case common::StyleUnit::Millimeters:
        {
            return calculations::UnitConversion::mmToPixels(value);
        }
            return value;
        }
    }
};
}
}
#endif // GRAPHICSUTILS_H
