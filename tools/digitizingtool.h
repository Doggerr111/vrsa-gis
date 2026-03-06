#ifndef DIGITIZINGTOOL_H
#define DIGITIZINGTOOL_H
#include "maptool.h"
#include "graphics/temporarygraphicsitem.h"

namespace vrsa
{
namespace tools
{

class DigitizingTool : public MapTool
{
public:
    explicit DigitizingTool(graphics::MapScene* scene, QObject *parent = nullptr);
    virtual ~DigitizingTool() = default;
protected:
    vrsa::geometry::Geometry mGeom;
    graphics::TemporaryGraphicsItem* mItem;
    //std::vector<QPointF> mPoints;

};

}
}
#endif // DIGITIZINGTOOL_H
