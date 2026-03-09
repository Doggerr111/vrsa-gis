// tests/unit/test_vector_feature.cpp
#include <gtest/gtest.h>
#include "vector/vectorfeature.h"
#include "test_utils/vectorlayertest.h"
#include <QDir>
// Тест-сьют для VectorFeature
TEST(VectorFeatureTest, DefaultConstruction)
{


    QString testDir = QDir::currentPath() + "/test_output";
       QDir dir;
       if (!dir.exists(testDir)) {
           dir.mkpath(testDir);
           qDebug() << "Created directory:" << testDir;
       }

       // Меняем рабочую директорию на тестовую папку
       QDir::setCurrent(testDir);
       qDebug() << "Working directory set to:" << QDir::currentPath();

    using namespace vrsa::test_utils;
    // 1. Shapefile (только один слой, короткие имена полей)
    auto* pointDS = createTestDataset("points", wkbPoint, 100, 2, "ESRI Shapefile");
    ASSERT_NE(pointDS, nullptr);
    if (pointDS) {
        saveTestDataset(pointDS, "test_points_wkbPoint.shp", "ESRI Shapefile");
        GDALClose(pointDS);
    }

    // 2. linestring
    auto* lineDS_shp = createTestDataset("lines", wkbLineString, 50, 2, "ESRI Shapefile");
    ASSERT_NE(lineDS_shp, nullptr);
    if (lineDS_shp) {
        saveTestDataset(lineDS_shp, "test_lines_wkbLineString.shp", "ESRI Shapefile");
        GDALClose(lineDS_shp);
    }


    // 2. GeoJSON (тоже один слой)
    auto* lineDS = createTestDataset("lines", wkbLineString, 50, 2, "GeoJSON");
    ASSERT_NE(lineDS, nullptr);
    if (lineDS) {
        saveTestDataset(lineDS, "test_lines_wkbLineString.geojson", "GeoJSON");
        GDALClose(lineDS);
    }

    // 3. GPKG (поддерживает много слоев)
    auto* completeDS = createCompleteTestDataset("GPKG");
    ASSERT_NE(completeDS, nullptr);
    if (completeDS) {
        saveTestDataset(completeDS, "test.gpkg", "GPKG");
        GDALClose(completeDS);
    }

    // 4. Memory (для тестов в памяти)
    auto* multiDS = createTestDataset("multipolygons", wkbMultiPolygon, 20, 3, "Memory");
    ASSERT_NE(multiDS, nullptr);
    if (multiDS) {
        // Работаем с датасетом в памяти
        // ...
        GDALClose(multiDS);
    }



}
