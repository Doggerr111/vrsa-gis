#include "testsymbolfactory.h"

#ifdef VRSA_ENABLE_TEST_UTILS

#include "graphics/symbols/layersymbol.h"
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithOnePoint()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPointSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithThreePoints()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPointSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithFivePoints()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPointSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePointSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithOneLine()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerLineSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithThreeLines()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerLineSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithFiveLines()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerLineSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimpleLineSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithOnePolygon()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPolygonSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithThreePolygons()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPolygonSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    return layerSymbol;
}

std::unique_ptr<vrsa::graphics::Symbol> vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithFivePolygons()
{
    auto layerSymbol = std::make_unique<vrsa::graphics::LayerPolygonSymbol>();
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    layerSymbol->addChild(vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol());
    return layerSymbol;
}
#endif
