#include "vectorstylingspinbox.h"
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"

VectorStylingSpinBox::VectorStylingSpinBox(vrsa::graphics::Symbol *s, StyleParam param, QWidget *parent)
    : QDoubleSpinBox{parent},
      mParam{param},
      mSymbol{s}
{
    setSpinBox();
}

//one day i will fix this ...
void VectorStylingSpinBox::setSpinBox()
{
    switch (mParam)
    {
    case StyleParam::PointSize:
    {
        setRange(0.0, 100.0);
        setSingleStep(0.1);
        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->pointSize);
            break;
        }
        default:
            break;

        }
        break;
    }
    case StyleParam::PenWidth:
    {
        setRange(0.0, 100.0);
        setSingleStep(0.1);
        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->borderWidth);
            break;
        }
        case vrsa::common::SymbolType::SimpleLineSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimpleLineSymbol*>(mSymbol);
            setValue(s->borderWidth);
            break;
        }
        case vrsa::common::SymbolType::SimplePolygonSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePolygonSymbol*>(mSymbol);
            setValue(s->borderWidth);
            break;
        }
        default:
            break;
        }
        break;
    }
    case StyleParam::XOffset:
    {
        setRange(-100.0, 100.0);
        setSingleStep(0.1);
        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->xOffset);
            break;
        }
        case vrsa::common::SymbolType::SimpleLineSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimpleLineSymbol*>(mSymbol);
            setValue(s->xOffset);
            break;
        }
        case vrsa::common::SymbolType::SimplePolygonSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePolygonSymbol*>(mSymbol);
            setValue(s->xOffset);
            break;
        }
        default:
            break;
        }

        break;
    }
    case StyleParam::YOffset:
    {
        setRange(-100.0, 100.0);
        setSingleStep(0.1);
        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->yOffset);
            break;
        }
        case vrsa::common::SymbolType::SimpleLineSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimpleLineSymbol*>(mSymbol);
            setValue(s->yOffset);
            break;
        }
        case vrsa::common::SymbolType::SimplePolygonSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePolygonSymbol*>(mSymbol);
            setValue(s->yOffset);
            break;
        }
        default:
            break;
        }

        break;
    }
    case StyleParam::Rotation:
    {
        setRange(-360.0, 360.0);
        setSingleStep(1);

        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->rotation);
            break;
        }
        default:
            break;
        }
        break;
    }
    case StyleParam::Opacity:
    {
        setRange(0.0, 1.0);
        setSingleStep(0.01);
        switch (mSymbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePointSymbol*>(mSymbol);
            setValue(s->opacity);
            break;
        }
        case vrsa::common::SymbolType::SimpleLineSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimpleLineSymbol*>(mSymbol);
            setValue(s->opacity);
            break;
        }
        case vrsa::common::SymbolType::SimplePolygonSymbol:
        {
            auto s = static_cast<vrsa::graphics::SimplePolygonSymbol*>(mSymbol);
            setValue(s->opacity);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}
