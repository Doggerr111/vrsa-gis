#include <gtest/gtest.h>
#include "gdal/gdalreader.h"
#include <QDebug>
#include "test_utils/vectorlayertest.h"
#include <QDir>
#include "GisDefines.h"
#include "gdal/dataset.h"
#include "vector/vectordataset.h"


struct TestLayerData {
    std::string layerName;
    OGRwkbGeometryType geomType;
    int numFeatures;
    int numFields;
};

class VectorReaderTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        //qDebug()<<QDir::currentPath();
        testDir = QDir::currentPath() +
                QString::fromStdString(vrsa::common::TEST_VECTOR_DATA_FOLDER) + "/vector_reader_test";
        //qDebug()<<testDir;
        QDir dir;
        if (!dir.exists(testDir))
            dir.mkpath(testDir);

        featureCount = 100;
        // Создаем хелпер
        helper = std::make_unique<vrsa::test_utils::TestLayerHelper>(testDir);
    }

    void TearDown() override
    {
        //очищаем тестовые файлы
        QDir dir(testDir);
        dir.removeRecursively();
        featureCount = 0;
    }

    QString testDir;
    int featureCount;
    std::unique_ptr<vrsa::test_utils::TestLayerHelper> helper;
};



// Тест 1: Создание shp слоев без мультигеометрий используя gdal api
TEST_F(VectorReaderTest, CreateSimpleLayers)
{
    OGRLayer* dsLayer;
    int layerFeatureCount = 0;
    //point layer
    auto* dsPoint = helper->createPointLayer("test_points", featureCount);
    ASSERT_NE(dsPoint, nullptr);
    EXPECT_TRUE(helper->layerExists("test_points", "shp"));
    dsLayer = dsPoint->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);
    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsPoint);
    //line layer
    auto* dsLine = helper->createLineLayer("test_lines", featureCount);
    ASSERT_NE(dsLine, nullptr);
    EXPECT_TRUE(helper->layerExists("test_lines", "shp"));
    dsLayer = dsLine->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);
    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsLine);
    //point layer
    auto* dsPoly = helper->createPolygonLayer("test_polygons", featureCount);
    ASSERT_NE(dsPoly, nullptr);
    EXPECT_TRUE(helper->layerExists("test_polygons", "shp"));
    dsLayer = dsPoly->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);
    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsPoly);
}

// Тест 2: Создание shp слоев с мультигеометриями используя gdal api
TEST_F(VectorReaderTest, CreateMultiGeometryLayer)
{
    OGRLayer* dsLayer;
    int layerFeatureCount = 0;
    //point layer
    auto* dsPoint = helper->createMultiPointLayer("test_MultiPoints", featureCount);
    ASSERT_NE(dsPoint, nullptr);
    EXPECT_TRUE(helper->layerExists("test_MultiPoints", "shp"));
    dsLayer = dsPoint->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);

    OGRFeatureDefn* poFDefn = dsLayer->GetLayerDefn();
    ASSERT_NE(poFDefn, nullptr);
    OGRwkbGeometryType layerGeomType = poFDefn->GetGeomType();
    EXPECT_EQ(wkbFlatten(layerGeomType), wkbMultiPoint);

    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsPoint);
    //line layer
    auto* dsLine = helper->createMultiLineLayer("test_MultiLines", featureCount);
    ASSERT_NE(dsLine, nullptr);
    EXPECT_TRUE(helper->layerExists("test_MultiLines", "shp"));
    dsLayer = dsLine->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);

    poFDefn = dsLayer->GetLayerDefn();
    ASSERT_NE(poFDefn, nullptr);
    layerGeomType = poFDefn->GetGeomType();
    EXPECT_EQ(wkbFlatten(layerGeomType), wkbMultiLineString);

    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsLine);
    //poly layer
    auto* dsPoly = helper->createMultiPolygonLayer("test_MultiPolygons", featureCount);
    ASSERT_NE(dsPoly, nullptr);
    EXPECT_TRUE(helper->layerExists("test_MultiPolygons", "shp"));
    dsLayer = dsPoly->GetLayer(0);
    ASSERT_NE(dsLayer, nullptr);
    poFDefn = dsLayer->GetLayerDefn();
    ASSERT_NE(poFDefn, nullptr);
    layerGeomType = poFDefn->GetGeomType();
    EXPECT_EQ(wkbFlatten(layerGeomType), wkbMultiPolygon);

    layerFeatureCount = dsLayer->GetFeatureCount();
    ASSERT_EQ(layerFeatureCount, featureCount);
    GDALClose(dsPoly);
}

//Тест 3: Проверяем GDALReader для работы с точечным слоем
TEST_F(VectorReaderTest, ReadSimplePointGeometryLayer)
{
    using namespace vrsa;
    auto* ds = helper->createPointLayer("test_point", 50);
    ASSERT_NE(ds, nullptr);
    GDALClose(ds);
    QString path = helper->getLayerPath("test_point", "shp");
    gdalwrapper::GDALReader reader;
    auto vrsaDS_uptr = reader.readDataset(path.toStdString());
    ASSERT_NE(vrsaDS_uptr, nullptr);
    auto vrsaDS_rawPtr = vrsaDS_uptr.get();
    ASSERT_EQ(vrsaDS_rawPtr->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDS_rawPtr);
    ASSERT_EQ(vectorDs->getLayers().size(), 1);
    ASSERT_EQ(vectorDs->getSource(), path.toStdString());
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 50);
}

//Тест 4: Проверяем GDALReader для работы с линейным слоем
TEST_F(VectorReaderTest, ReadSimpleLineGeometryLayer)
{
    using namespace vrsa;
    auto* ds = helper->createLineLayer("test_line", 50);
    ASSERT_NE(ds, nullptr);
    GDALClose(ds);
    QString path = helper->getLayerPath("test_line", "shp");
    gdalwrapper::GDALReader reader;
    auto vrsaDS_uptr = reader.readDataset(path.toStdString());
    ASSERT_NE(vrsaDS_uptr, nullptr);
    auto vrsaDS_rawPtr = vrsaDS_uptr.get();
    ASSERT_EQ(vrsaDS_rawPtr->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDS_rawPtr);
    ASSERT_EQ(vectorDs->getLayers().size(), 1);
    ASSERT_EQ(vectorDs->getSource(), path.toStdString());
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 50);
}


//Тест 5: Проверяем GDALReader для работы с полигональными слоем
TEST_F(VectorReaderTest, ReadSimplePolygonGeometryLayer)
{
    using namespace vrsa;
    auto* ds = helper->createLineLayer("test_poly", 50);
    ASSERT_NE(ds, nullptr);
    GDALClose(ds);
    QString path = helper->getLayerPath("test_poly", "shp");
    gdalwrapper::GDALReader reader;
    auto vrsaDS_uptr = reader.readDataset(path.toStdString());
    ASSERT_NE(vrsaDS_uptr, nullptr);
    auto vrsaDS_rawPtr = vrsaDS_uptr.get();
    ASSERT_EQ(vrsaDS_rawPtr->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDS_rawPtr);
    ASSERT_EQ(vectorDs->getLayers().size(), 1);
    ASSERT_EQ(vectorDs->getSource(), path.toStdString());
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 50);
}
//multigeometry tests soon tm
