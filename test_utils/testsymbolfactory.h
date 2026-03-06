#ifndef TESTSYMBOLFACTORY_H
#define TESTSYMBOLFACTORY_H

#ifdef VRSA_ENABLE_TEST_UTILS
#include <memory>
namespace vrsa{
namespace graphics{
class Symbol;
}
}

namespace vrsa
{
namespace test_utils
{

class TestSymbolFactory
{
public:
    TestSymbolFactory() = default;
    static std::unique_ptr<vrsa::graphics::Symbol> createTestPointSymbolWithOnePoint();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestPointSymbolWithThreePoints();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestPointSymbolWithFivePoints();

    static std::unique_ptr<vrsa::graphics::Symbol> createTestLineSymbolWithOneLine();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestLineSymbolWithThreeLines();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestLineSymbolWithFiveLines();

    static std::unique_ptr<vrsa::graphics::Symbol> createTestPolygonSymbolWithOnePolygon();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestPolygonSymbolWithThreePolygons();
    static std::unique_ptr<vrsa::graphics::Symbol> createTestPolygonSymbolWithFivePolygons();
};

}
}
#endif

#endif // TESTSYMBOLFACTORY_H
