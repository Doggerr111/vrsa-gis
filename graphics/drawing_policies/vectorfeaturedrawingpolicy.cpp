#include "vectorfeaturedrawingpolicy.h"
#include "geometry/geometryconverter.h"
#include "geos/simplify/DouglasPeuckerSimplifier.h"
#include "geos/simplify/TopologyPreservingSimplifier.h"
//FeatureDrawingPolicy::FeatureDrawingPolicy()
//{

//}

vrsa::graphics::VectorFeatureDrawingPolicy::VectorFeatureDrawingPolicy()
    :mLodCache{std::make_unique<LODGeometry>()}
{

}

void vrsa::graphics::VectorFeatureDrawingPolicy::precomputeLOD(OGRGeometry *geom) const
{
    if (mCache.isGeomValid)
        return;
    if (!geom) return;
    mLodCache->originalGeom = geom;
    mLodCache->originalPath = createPathFromOGRGeometry(geom);
    //уровни масштабирования !!!перенести отсюда!!!
    static struct { int zoom; double scale; double tol; } levels[] =
    {
        {0, 500000000, 1000000.0},   // zoom 0: планета → 1000 км
        {1, 250000000, 500000.0},    // zoom 1: континенты → 500 км
        {2, 150000000, 200000.0},    // zoom 2: страны → 200 км
        {3, 70000000,  100000.0},    // zoom 3: регионы → 100 км
        {4, 35000000,  50000.0},     // zoom 4: области → 50 км
        {5, 15000000,  20000.0},     // zoom 5: крупные города → 20 км
        {6, 10000000,  10000.0},     // zoom 6: города → 10 км
        {7, 4000000,   5000.0},      // zoom 7: районы → 5 км
        {8, 2000000,   2000.0},      // zoom 8: кварталы → 2 кмм
        {9, 1000000,   1000.0},      // zoom 9: детали → 1 км
        {10, 500000,   500.0},       // zoom 10: сверхдетально → 500 м
        {11, 250000,   250.0},       // zoom 11: здания → 250 м
        {12, 125000,   100.0},       // zoom 12: детали зданий → 100 м
        {13, 64000,    50.0},        // zoom 13: улицы → 50 м
        {14, 32000,    25.0},        // zoom 14: дома → 25 м
        {15, 16000,    12.0},        // zoom 15: подъезды → 12 м
        {16, 8000,     6.0},         // zoom 16: дорожки → 6 м
        {17, 4000,     3.0},         // zoom 17: малые формы → 3 м
        {18, 2000,     1.5},         // zoom 18: архитектура → 1.5 м
        {19, 1000,     0.75},        // zoom 19: сверхдетали → 0.75 м (если нужно)
        {20, 500,      0.37}         // zoom 20: план здания → 37 см (если нужно)
    };

    // Конвертируем в GEOS один раз
    auto geosGeom = geometry::GeometryConverter::createGEOSFromOGR(geom);
    if (!geosGeom) return;

    for (const auto& level : levels)
    {
        //DouglasPeuckerSimplifier - быстро, но не сохраняет топологию
        //TopologyPreservingSimplifier - с сохранением топологии
        auto simplified = geos::simplify::DouglasPeuckerSimplifier::simplify(
            geosGeom.get(), level.tol);
        //qDebug()<<"упрощенная" +QString::fromStdString(simplified->toString());
        if (simplified)
        {
            //обратно в OGR
            auto ogrSimplified = geometry::GeometryConverter::createOGRfromGeos(simplified.get());
            QPainterPath path = createPathFromOGRGeometry(ogrSimplified.get());
            //qDebug()<<path; //создается!
            mLodCache->addLevel(level.zoom, level.scale, std::move(ogrSimplified), path);
        }
        else
        {
            qDebug()<<"ERROR";
        }
    }
    mCache.isGeomValid = true;
}


//void vrsa::graphics::VectorFeatureDrawingPolicy::LODGeometry::precompute(OGRGeometry *originalGeom)
//{
//    if (!originalGeom) return;

//    // Сохраняем оригинал
//    m_original.reset(originalGeom->clone());
//    m_levels.clear();

//    // Конвертируем в GEOS один раз
//    auto geosGeom = geometry::GeometryConverter::createGEOSFromOGR(originalGeom);
//    if (!geosGeom) return;

//    // Уровни масштабирования (можно вынести в конфиг)
//    static const struct { int zoom; double scale; double tol; } LEVELS[] = {
//        {0, 500000000, 100000.0},   // zoom 0: планета → 100 км
//        {1, 250000000, 50000.0},    // zoom 1: континенты → 50 км
//        {2, 150000000, 20000.0},    // zoom 2: страны → 20 км
//        {3, 70000000,  10000.0},    // zoom 3: регионы → 10 км
//        {4, 35000000,  5000.0},     // zoom 4: области → 5 км
//        {5, 15000000,  2000.0},     // zoom 5: крупные города → 2 км
//        {6, 10000000,  1000.0},     // zoom 6: города → 1 км
//        {7, 4000000,   500.0},      // zoom 7: районы → 500 м
//        {8, 2000000,   200.0},      // zoom 8: кварталы → 200 м
//        {9, 1000000,   100.0},      // zoom 9: детали → 100 м
//        {10, 500000,   50.0},       // zoom 10: сверхдетально → 50 м
//        {11, 250000,   25.0},       // zoom 11: здания → 25 м
//        {12, 125000,   10.0},       // zoom 12: детали зданий → 10 м
//    };

//    for (const auto& level : LEVELS) {
//        // Упрощаем геометрию
//        auto simplified = geos::simplify::DouglasPeuckerSimplifier::simplify(
//            geosGeom.get(), level.tol);

//        if (simplified) {
//            // Конвертируем обратно в OGR
//            auto ogrSimplified = geometry::GeometryConverter::createOGRfromGeos(simplified.get());
//            // Добавляем уровень
//            addLevel(level.zoom, level.scale, std::move(ogrSimplified));
//        }
//    }

//    //qDebug() << "LOD precomputed:" << m_levels.size() << "levels for" << this;

//}
