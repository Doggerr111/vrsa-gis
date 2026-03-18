#include <gtest/gtest.h>
#include "graphics/featuregraphicsitem.h"
#include "graphics/drawing_policies/pointfeaturedrawingpolicy.h"
#include "graphics/drawing_policies/linefeaturedrawingpolicy.h"
#include "simplelinesymbol.h"
#include "QRandomGenerator"
//Оказывается мои рассчеты bounding rect были неверны.... поэтому в политиках теперь делегирую
//это qt классу, чтобы не возникало ошибок при смене типа соединений линий, типа оконечностей и т.д.
TEST(DrawingPolicyTest, LineFeaturePolicyTest)
{
    using namespace vrsa::graphics;
    auto style = VectorFeatureStyle::createDefaultVectorStyle(vrsa::common::GeometryType::LineString);


    DrawingContext linePolicyContext;
    OGRLineString lineGeom;
    lineGeom.addPoint(10,10);
    lineGeom.addPoint(20,20);
    lineGeom.addPoint(10,30);
    QPainterPath path;
    path.moveTo(10,10);
    path.lineTo(20,20);
    path.lineTo(10,30);
    linePolicyContext.geom = &lineGeom;
    auto symbol = SimpleLineSymbol::createDefaultSymbol();
    LineFeatureDrawingPolicy policy(symbol.get());
    int steps = 100;
    for (int i=1; i<steps; ++i)
    {
        double d = static_cast<double>(i) / steps;
        symbol->borderWidth = d;
        symbol->capStyle = static_cast<Qt::PenCapStyle>(QRandomGenerator::global()->bounded(3));

        linePolicyContext.sceneScale = d;
        auto policyRect = policy.boundingRect(linePolicyContext);

        auto penStroker = symbol->pen();
        auto penWidth = penStroker.widthF()/d;
        penStroker.setWidthF(penWidth);
        QPainterPathStroker stroker(penStroker);
        auto strokePath = stroker.createStroke(path);
        auto strokerRect = strokePath.boundingRect();

        qDebug()<<"orig" <<policyRect << "stroke:"<<strokerRect;
        EXPECT_NEAR(policyRect.x(), strokerRect.x(), 1e-6);
        EXPECT_NEAR(policyRect.y(), strokerRect.y(), 1e-6);
        EXPECT_NEAR(policyRect.width(), strokerRect.width(), 1e-6);
        EXPECT_NEAR(policyRect.height(), strokerRect.height(), 1e-6);

    }

}
