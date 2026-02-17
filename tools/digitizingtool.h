#ifndef DIGITIZINGTOOL_H
#define DIGITIZINGTOOL_H
#include "maptool.h"

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
    std::vector<QPointF> mPoints;
};

}
}
#endif // DIGITIZINGTOOL_H
