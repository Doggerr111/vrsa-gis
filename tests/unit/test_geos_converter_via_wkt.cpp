#include <gtest/gtest.h>
#include "geometry/geometryconverter.h"

using namespace vrsa::geometry;

class GeometryConverterTest_WKT : public ::testing::Test {
protected:
    void SetUp() override
    {
        //тестовая геометрия OGR
        ogrPoint = new OGRPoint(10.5, 20.7);

        ogrLine = new OGRLineString();
        ogrLine->addPoint(0.0, 0.0);
        ogrLine->addPoint(10.0, 10.0);
        ogrLine->addPoint(20.0, 5.0);

        ogrPolygon = new OGRPolygon();
        OGRLinearRing* ring = new OGRLinearRing();
        ring->addPoint(0.0, 0.0);
        ring->addPoint(10.0, 0.0);
        ring->addPoint(10.0, 10.0);
        ring->addPoint(0.0, 10.0);
        ring->addPoint(0.0, 0.0);
        ogrPolygon->addRing(ring);

        ogrMultiPoint = new OGRMultiPoint();
        ogrMultiPoint->addGeometry(new OGRPoint(1.0, 1.0));
        ogrMultiPoint->addGeometry(new OGRPoint(2.0, 2.0));
        ogrMultiPoint->addGeometry(new OGRPoint(3.0, 3.0));
    }

    void TearDown() override
    {
        delete ogrPoint;
        delete ogrLine;
        delete ogrPolygon;
        delete ogrMultiPoint;
    }

    OGRPoint* ogrPoint;
    OGRLineString* ogrLine;
    OGRPolygon* ogrPolygon;
    OGRMultiPoint* ogrMultiPoint;
};

// ============================================================================
// OGR -> GEOS (fromOGRViaWKT)
// ============================================================================

TEST_F(GeometryConverterTest_WKT, FromOGR_NullPtr)
{
    auto result = GeometryConverter::fromOGRViaWKT(nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST_F(GeometryConverterTest_WKT, FromOGR_Point)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_POINT);

    auto* point = dynamic_cast<geos::geom::Point*>(geosGeom.get());
    ASSERT_NE(point, nullptr);

    EXPECT_DOUBLE_EQ(point->getX(), 10.5);
    EXPECT_DOUBLE_EQ(point->getY(), 20.7);
}

TEST_F(GeometryConverterTest_WKT, FromOGR_Line)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_LINESTRING);

    auto* line = dynamic_cast<geos::geom::LineString*>(geosGeom.get());
    ASSERT_NE(line, nullptr);

    EXPECT_EQ(line->getNumPoints(), 3);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(0).x, 0.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(0).y, 0.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(2).x, 20.0);
    EXPECT_DOUBLE_EQ(line->getCoordinateN(2).y, 5.0);
}

TEST_F(GeometryConverterTest_WKT, FromOGR_Polygon)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrPolygon);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_POLYGON);

    auto* poly = dynamic_cast<geos::geom::Polygon*>(geosGeom.get());
    ASSERT_NE(poly, nullptr);

    EXPECT_FALSE(poly->isEmpty());
    EXPECT_GT(poly->getArea(), 0.0);
}

TEST_F(GeometryConverterTest_WKT, FromOGR_MultiPoint)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrMultiPoint);
    ASSERT_NE(geosGeom, nullptr);

    EXPECT_EQ(geosGeom->getGeometryTypeId(), geos::geom::GEOS_MULTIPOINT);

    auto* multiPoint = dynamic_cast<geos::geom::MultiPoint*>(geosGeom.get());
    ASSERT_NE(multiPoint, nullptr);

    EXPECT_EQ(multiPoint->getNumGeometries(), 3);
}

// ============================================================================
// EOS -> OGR (toOGRViaWKT)
// ============================================================================

TEST_F(GeometryConverterTest_WKT, ToOGR_NullPtr)
{
    OGRGeometry* result = GeometryConverter::toOGRViaWKT(nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST_F(GeometryConverterTest_WKT, ToOGR_Point)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbPoint);

    OGRPoint* point = dynamic_cast<OGRPoint*>(ogrResult);
    ASSERT_NE(point, nullptr);

    EXPECT_DOUBLE_EQ(point->getX(), 10.5);
    EXPECT_DOUBLE_EQ(point->getY(), 20.7);

    delete ogrResult;
}

TEST_F(GeometryConverterTest_WKT, ToOGR_Line)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbLineString);

    OGRLineString* line = dynamic_cast<OGRLineString*>(ogrResult);
    ASSERT_NE(line, nullptr);

    EXPECT_EQ(line->getNumPoints(), 3);
    EXPECT_DOUBLE_EQ(line->getX(0), 0.0);
    EXPECT_DOUBLE_EQ(line->getY(2), 5.0);

    delete ogrResult;
}

TEST_F(GeometryConverterTest_WKT, ToOGR_Polygon)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrPolygon);
    ASSERT_NE(geosGeom, nullptr);

    OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    EXPECT_EQ(wkbFlatten(ogrResult->getGeometryType()), wkbPolygon);

    OGRPolygon* poly = dynamic_cast<OGRPolygon*>(ogrResult);
    ASSERT_NE(poly, nullptr);

    OGRLinearRing* exterior = poly->getExteriorRing();
    ASSERT_NE(exterior, nullptr);
    EXPECT_GE(exterior->getNumPoints(), 4);

    delete ogrResult;
}

// ============================================================================
// КРУГОВОЕ ПРЕОБРАЗОВАНИЕ OGR -> GEOS -> OGR
// ============================================================================

TEST_F(GeometryConverterTest_WKT, RoundTrip_Point)
{
    // OGR → GEOS
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrPoint);
    ASSERT_NE(geosGeom, nullptr);

    // GEOS → OGR
    OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    // Сравниваем через WKT
    char* originalWkt = nullptr;
    char* resultWkt = nullptr;

    ogrPoint->exportToWkt(&originalWkt);
    ogrResult->exportToWkt(&resultWkt);

    EXPECT_STREQ(originalWkt, resultWkt);

    CPLFree(originalWkt);
    CPLFree(resultWkt);
    delete ogrResult;
}

TEST_F(GeometryConverterTest_WKT, RoundTrip_Line)
{
    auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrLine);
    ASSERT_NE(geosGeom, nullptr);

    OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
    ASSERT_NE(ogrResult, nullptr);

    char* originalWkt = nullptr;
    char* resultWkt = nullptr;

    ogrLine->exportToWkt(&originalWkt);
    ogrResult->exportToWkt(&resultWkt);

    EXPECT_STREQ(originalWkt, resultWkt);

    CPLFree(originalWkt);
    CPLFree(resultWkt);
    delete ogrResult;
}

// ============================================================================
// ОБРАБОТКА ОШИБОК
// ============================================================================

TEST_F(GeometryConverterTest_WKT, InvalidGeometry)
{
    //передаем nullptr
    auto geosGeom = GeometryConverter::fromOGRViaWKT(nullptr);
    EXPECT_EQ(geosGeom, nullptr);

    //создаем линию с 1 точкой (в GEOS линия должна иметь >=2 точек)
    OGRLineString invalidLine;
    invalidLine.addPoint(0.0, 0.0);  // только одна точка

    // В GDAL это валидно, но при конвертации в GEOS ожидаем исключение
    EXPECT_ANY_THROW(GeometryConverter::fromOGRViaWKT(&invalidLine));
}

// ============================================================================
// ПРОИЗВОДИТЕЛЬНОСТЬ
// ============================================================================

TEST_F(GeometryConverterTest_WKT, DISABLED_Performance_Test) {
    const int NUM_ITERATIONS = 1000;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        auto geosGeom = GeometryConverter::fromOGRViaWKT(ogrLine);
        ASSERT_NE(geosGeom, nullptr);

        OGRGeometry* ogrResult = GeometryConverter::toOGRViaWKT(geosGeom.get());
        ASSERT_NE(ogrResult, nullptr);

        delete ogrResult;
    }

    SUCCEED();
}


