#include <gtest/gtest.h>
#include "geometry/geometryconverter.h"
#include "logger.h"
using namespace vrsa::geometry;

class GeometryConverterTest_Coords : public ::testing::Test {
protected:
    void SetUp() override
    {
        //тестовая геометрия OGR
        ogrPoint = new OGRPoint(10.5, 20.7);

        ogrLine = new OGRLineString();
        ogrLine->addPoint(0.0, 0.0);
        ogrLine->addPoint(10.0, 10.0);
        ogrLine->addPoint(20.0, 5.0);

        ogrPoly = new OGRPolygon();
        OGRLinearRing* ring = new OGRLinearRing();
        ring->addPoint(0.0, 0.0);
        ring->addPoint(10.0, 0.0);
        ring->addPoint(10.0, 10.0);
        ring->addPoint(0.0, 10.0);
        ring->addPoint(0.0, 0.0);
        ogrPoly->addRing(ring);
    }

    void TearDown() override
    {
        delete ogrPoint;
        delete ogrLine;
        delete ogrPoly;
        //delete ogrMultiPoint;
    }

    OGRPoint* ogrPoint;
    OGRLineString* ogrLine;
    OGRPolygon* ogrPoly;
    //OGRMultiPoint* ogrMultiPoint; //для мультигеометрий пока используется wkt
};


// ============================================================================
// createGEOSFromOGR (OGR -> GEOS)
// ============================================================================

TEST_F(GeometryConverterTest_Coords, GEOSFromOGR_NullPtr)
{
    auto result = GeometryConverter::createGEOSFromOGR(nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST_F(GeometryConverterTest_Coords, GEOSFromOGR_Point)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_POINT);

    auto* point = dynamic_cast<geos::geom::Point*>(geosGeom.get());
    ASSERT_NE(point, nullptr);

    EXPECT_DOUBLE_EQ(point->getX(), 10.5);
    EXPECT_DOUBLE_EQ(point->getY(), 20.7);
}

TEST_F(GeometryConverterTest_Coords, GEOSFromOGR_LineString)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_LINESTRING);

    auto* line = dynamic_cast<geos::geom::LineString*>(geosGeom.get());
    ASSERT_NE(line, nullptr);

    EXPECT_EQ(line->getNumPoints(), 3);

    EXPECT_DOUBLE_EQ(line->getCoordinateN(0).x, 0.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(0).y, 0.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(1).x, 10.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(1).y, 10.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(2).x, 20.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(2).y, 5.0);
}

TEST_F(GeometryConverterTest_Coords, GEOSFromOGR_Polygon)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoly);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_POLYGON);

    auto* poly = dynamic_cast<geos::geom::Polygon*>(geosGeom.get());
    ASSERT_NE(poly, nullptr);

    //площадь должна быть около 100
    EXPECT_NEAR(poly->getArea(), 100.0, 0.001);

    //внешнее кольцо
    const auto* shell = poly->getExteriorRing();
    ASSERT_NE(shell, nullptr);
    EXPECT_EQ(shell->getNumPoints(), 5);  // 4 точки + замыкание

    //внутренних колец нет
    EXPECT_EQ(poly->getNumInteriorRing(), 0);
}

// ============================================================================
// createOGRfromGeos (GEOS -> OGR)
// ============================================================================

TEST_F(GeometryConverterTest_Coords, OGRFromGEOS_NullPtr)
{
    auto result = GeometryConverter::createOGRfromGeos(nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST_F(GeometryConverterTest_Coords, OGRFromGEOS_Point)
{
    //создаем GEOS точку из OGR точки
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    //обратно в OGR
    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    //проверяем тип
    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbPoint);

    //приводим к OGRPoint
    OGRPoint* point = dynamic_cast<OGRPoint*>(ogrResult.get());
    ASSERT_NE(point, nullptr);

    EXPECT_DOUBLE_EQ(point->getX(), 10.5);
    EXPECT_DOUBLE_EQ(point->getY(), 20.7);
}

TEST_F(GeometryConverterTest_Coords, OGRFromGEOS_LineString)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbLineString);

    OGRLineString* line = dynamic_cast<OGRLineString*>(ogrResult.get());
    ASSERT_NE(line, nullptr);

    EXPECT_EQ(line->getNumPoints(), 3);
    EXPECT_DOUBLE_EQ(line->getX(0), 0.0);
    EXPECT_DOUBLE_EQ(line->getY(0), 0.0);
    EXPECT_DOUBLE_EQ(line->getX(1), 10.0);
    EXPECT_DOUBLE_EQ(line->getY(1), 10.0);
    EXPECT_DOUBLE_EQ(line->getX(2), 20.0);
    EXPECT_DOUBLE_EQ(line->getY(2), 5.0);
}

TEST_F(GeometryConverterTest_Coords, OGRFromGEOS_Polygon)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoly);
    ASSERT_NE(geosGeom, nullptr);

    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbPolygon);

    OGRPolygon* poly = dynamic_cast<OGRPolygon*>(ogrResult.get());
    ASSERT_NE(poly, nullptr);

    OGRLinearRing* shell = poly->getExteriorRing();
    ASSERT_NE(shell, nullptr);
    EXPECT_GE(shell->getNumPoints(), 4);

    EXPECT_EQ(poly->getNumInteriorRings(), 0);
}

// ============================================================================
// Круговое преобразование OGR -> GEOS -> OGR
// ============================================================================

TEST_F(GeometryConverterTest_Coords, RoundTrip_Point)
{
    // OGR -> GEOS
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    // GEOS -> OGR
    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    //cравниваем через WKT
    char* originalWkt = nullptr;
    char* resultWkt = nullptr;

    ogrPoint->exportToWkt(&originalWkt);
    ogrResult->exportToWkt(&resultWkt);

    EXPECT_STREQ(originalWkt, resultWkt);

    CPLFree(originalWkt);
    CPLFree(resultWkt);
}

TEST_F(GeometryConverterTest_Coords, RoundTrip_LineString)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    char* originalWkt = nullptr;
    char* resultWkt = nullptr;

    ogrLine->exportToWkt(&originalWkt);
    ogrResult->exportToWkt(&resultWkt);

    EXPECT_STREQ(originalWkt, resultWkt);

    CPLFree(originalWkt);
    CPLFree(resultWkt);
}

TEST_F(GeometryConverterTest_Coords, RoundTrip_Polygon)
{
    auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrPoly);
    ASSERT_NE(geosGeom, nullptr);

    auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    char* originalWkt = nullptr;
    char* resultWkt = nullptr;

    ogrPoly->exportToWkt(&originalWkt);
    ogrResult->exportToWkt(&resultWkt);

    EXPECT_STREQ(originalWkt, resultWkt);

    CPLFree(originalWkt);
    CPLFree(resultWkt);
}

// ============================================================================
// Gроизводительность
// ============================================================================

TEST_F(GeometryConverterTest_Coords, DISABLED_Performance)
{
    const int N = 10000;

    VRSA_START_TIMER("Performance test");

    for (int i = 0; i < N; ++i)
    {
        auto geosGeom = GeometryConverter::createGEOSFromOGR(ogrLine);
        ASSERT_NE(geosGeom, nullptr);

        auto ogrResult = GeometryConverter::createOGRfromGeos(geosGeom.get());
        ASSERT_NE(ogrResult, nullptr);
    }

    VRSA_STOP_TIMER("Performance test");

}
