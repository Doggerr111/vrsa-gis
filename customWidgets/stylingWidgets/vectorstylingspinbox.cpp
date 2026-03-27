#include "vectorstylingspinbox.h"
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"
#include "common/GisDefines.h"
VectorStylingSpinBox::VectorStylingSpinBox(vrsa::graphics::Symbol *s, StyleParam param, QWidget *parent)
    : QDoubleSpinBox{parent},
      mParam{param},
      mSymbol{s}
{
    updateFromSymbol();
}


void VectorStylingSpinBox::updateFromSymbol()
{
    setRangeForParam();
    setStepForParam();
    setValueFromSymbol();
}

void VectorStylingSpinBox::setRangeForParam()
{
    switch (mParam)
    {
    case StyleParam::PointSize:
    case StyleParam::PenWidth:
        setRange(0.0, 100.0);
        break;
    case StyleParam::XOffset:
    case StyleParam::YOffset:
        setRange(-100.0, 100.0);
        break;
    case StyleParam::Rotation:
        setRange(-360.0, 360.0);
        break;
    case StyleParam::Opacity:
        setRange(0.0, 1.0);
        break;
    default:
        break;
    }
}

void VectorStylingSpinBox::setStepForParam()
{
    switch (mParam)
    {
    case StyleParam::PointSize:
    case StyleParam::PenWidth:
    case StyleParam::XOffset:
    case StyleParam::YOffset:
        setSingleStep(0.1);
        break;
    case StyleParam::Rotation:
        setSingleStep(1.0);
        break;
    case StyleParam::Opacity:
        setSingleStep(0.01);
        break;
    default:
        break;
    }
}

void VectorStylingSpinBox::setValueFromSymbol()
{
    switch (mParam)
    {
    case StyleParam::PointSize:
        setValue(getPointSize());
        break;
    case StyleParam::PenWidth:
        setValue(getPenWidth());
        break;
    case StyleParam::XOffset:
        setValue(getXOffset());
        break;
    case StyleParam::YOffset:
        setValue(getYOffset());
        break;
    case StyleParam::Rotation:
        setValue(getRotation());
        break;
    case StyleParam::Opacity:
        setValue(getOpacity());
        break;
    default:
        break;
    }
}


double VectorStylingSpinBox::getPointSize() const
{
    using namespace vrsa::graphics;
    auto* s = dynamic_cast<SimplePointSymbol*>(mSymbol);
    return s ? s->pointSize :  0.0;
}

double VectorStylingSpinBox::getPenWidth() const
{
    using namespace vrsa::graphics;
    using namespace vrsa::common;
    switch (mSymbol->type())
    {
    case SymbolType::SimplePointSymbol:
        return static_cast<SimplePointSymbol*>(mSymbol)->borderWidth;
    case SymbolType::SimpleLineSymbol:
        return static_cast<SimpleLineSymbol*>(mSymbol)->borderWidth;
    case SymbolType::SimplePolygonSymbol:
        return static_cast<SimplePolygonSymbol*>(mSymbol)->borderWidth;
    default:
        return 0.0;
    }
}

double VectorStylingSpinBox::getXOffset() const
{
    using namespace vrsa::graphics;
    using namespace vrsa::common;
    switch (mSymbol->type())
    {
    case SymbolType::SimplePointSymbol:
        return static_cast<SimplePointSymbol*>(mSymbol)->xOffset;
    case SymbolType::SimpleLineSymbol:
        return static_cast<SimpleLineSymbol*>(mSymbol)->xOffset;
    case SymbolType::SimplePolygonSymbol:
        return static_cast<SimplePolygonSymbol*>(mSymbol)->xOffset;
    default:
        return 0.0;
    }
}

double VectorStylingSpinBox::getYOffset() const
{
    using namespace vrsa::graphics;
    using namespace vrsa::common;
    switch (mSymbol->type())
    {
    case SymbolType::SimplePointSymbol:
        return static_cast<SimplePointSymbol*>(mSymbol)->yOffset;
    case SymbolType::SimpleLineSymbol:
        return static_cast<SimpleLineSymbol*>(mSymbol)->yOffset;
    case SymbolType::SimplePolygonSymbol:
        return static_cast<SimplePolygonSymbol*>(mSymbol)->yOffset;
    default:
        return 0.0;
    }
}

double VectorStylingSpinBox::getRotation() const
{
    using namespace vrsa::graphics;
    auto* s = dynamic_cast<SimplePointSymbol*>(mSymbol);
    return s ? s->rotation :  0.0;
}

double VectorStylingSpinBox::getOpacity() const
{
    using namespace vrsa::graphics;
    using namespace vrsa::common;
    switch (mSymbol->type())
    {
    case SymbolType::SimplePointSymbol:
        return static_cast<SimplePointSymbol*>(mSymbol)->opacity;
    case SymbolType::SimpleLineSymbol:
        return static_cast<SimpleLineSymbol*>(mSymbol)->opacity;
    case SymbolType::SimplePolygonSymbol:
        return static_cast<SimplePolygonSymbol*>(mSymbol)->opacity;
    default:
        return 1.0;
    }
}

