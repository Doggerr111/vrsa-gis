#include <gtest/gtest.h>
#include "gdal/gdalreader.h"
#include <QDebug>
#include <QDir>
#include "GisDefines.h"
#include "gdal/dataset.h"
#include "vector/vectordataset.h"
#include "vectorlayertools/vectorlayercreator.h"


class VectorCreatorTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        testDir = QDir::currentPath() +
                QString::fromStdString(vrsa::common::TEST_VECTOR_DATA_FOLDER) + "/vector_creator_test";
        QDir dir;
        if (!dir.exists(testDir))
            dir.mkpath(testDir);
    }

    void TearDown() override
    {
        //очищаем тестовые файлы
        QDir dir(testDir);
        dir.removeRecursively();
    }

    QString testDir;
};



// Тест 1: Создание точечного слоя в SHP без полей
TEST_F(VectorCreatorTest, CreateSimplePointLayerSHP)
{
    using namespace vrsa;
    common::LayerDefinition layerDef;
    layerDef.crs = "WGS 84";
    layerDef.filePath = testDir.toStdString()+".shp";
    layerDef.format = "ESRI Shapefile";
    layerDef.geometryType = static_cast<int>(common::GeometryType::Point);
    vector::VectorLayerCreator creator;
    auto gdalDs_Uptr = creator.createGDALDataset(layerDef);
    ASSERT_NE(gdalDs_Uptr, nullptr);
    auto pathToDs = creator.getVectorDatasetPath(gdalDs_Uptr.get());
    gdalDs_Uptr.reset();
    ASSERT_EQ(pathToDs, layerDef.filePath);
    gdalwrapper::GDALReader reader;
    auto vrsaDsUptr = reader.readDataset(pathToDs);
    ASSERT_NE(vrsaDsUptr, nullptr);
    auto vrsaDsRaw = vrsaDsUptr.get();
    ASSERT_EQ(pathToDs, vrsaDsRaw->getSource());
    ASSERT_EQ(vrsaDsRaw->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDsRaw);
    ASSERT_NE(vectorDs, nullptr);
    ASSERT_EQ(vectorDs->layersCount(), 1);
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getGeomType(), common::GeometryType::Point);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getFieldCount(), 0);
}
// Тест 2: Создание линейного слоя geoJSON
TEST_F(VectorCreatorTest, CreateSimpleLineLayerGeoJSON)
{
    using namespace vrsa;
    common::LayerDefinition layerDef;
    layerDef.crs = "WGS 84";
    layerDef.filePath = testDir.toStdString()+".geojson";
    layerDef.format = "GeoJSON";
    layerDef.geometryType = static_cast<int>(common::GeometryType::LineString);
    vector::VectorLayerCreator creator;
    auto gdalDs_Uptr = creator.createGDALDataset(layerDef);
    ASSERT_NE(gdalDs_Uptr, nullptr);
    auto pathToDs = creator.getVectorDatasetPath(gdalDs_Uptr.get());
    gdalDs_Uptr.reset();
    ASSERT_EQ(pathToDs, layerDef.filePath);
    gdalwrapper::GDALReader reader;
    auto vrsaDsUptr = reader.readDataset(pathToDs);
    ASSERT_NE(vrsaDsUptr, nullptr);
    auto vrsaDsRaw = vrsaDsUptr.get();
    ASSERT_EQ(pathToDs, vrsaDsRaw->getSource());
    ASSERT_EQ(vrsaDsRaw->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDsRaw);
    ASSERT_NE(vectorDs, nullptr);
    ASSERT_EQ(vectorDs->layersCount(), 1);
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getGeomType(), common::GeometryType::Unknown); //geojson has unknown geomtype
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getFieldCount(), 0);
}

//Teст 3. Создание полигонального слоя GPKG с полями
TEST_F(VectorCreatorTest, CreateSimplePolyLayerGPKGWithAttributes)
{
    using namespace vrsa;
    common::LayerDefinition layerDef;
    layerDef.crs = "WGS 84";
    layerDef.filePath = testDir.toStdString()+".gpkg";
    layerDef.format = "GPKG";
    layerDef.geometryType = static_cast<int>(common::GeometryType::Polygon);
    common::FieldDefinition def;
    def.name="attr1";
    def.typeIndex=2;
    layerDef.fields = std::vector<common::FieldDefinition>{def};
    vector::VectorLayerCreator creator;
    auto gdalDs_Uptr = creator.createGDALDataset(layerDef);
    ASSERT_NE(gdalDs_Uptr, nullptr);
    auto pathToDs = creator.getVectorDatasetPath(gdalDs_Uptr.get());
    gdalDs_Uptr.reset();
    ASSERT_EQ(pathToDs, layerDef.filePath);
    gdalwrapper::GDALReader reader;
    auto vrsaDsUptr = reader.readDataset(pathToDs);
    ASSERT_NE(vrsaDsUptr, nullptr);
    auto vrsaDsRaw = vrsaDsUptr.get();
    ASSERT_EQ(pathToDs, vrsaDsRaw->getSource());
    ASSERT_EQ(vrsaDsRaw->GetDatasetType(), common::DatasetType::Vector);
    auto vectorDs = static_cast<vector::VectorDataset*>(vrsaDsRaw);
    ASSERT_NE(vectorDs, nullptr);
    ASSERT_EQ(vectorDs->layersCount(), 1);
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_EQ(vectorLayer->getGeomType(), common::GeometryType::Polygon);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getFieldCount(), 1);
}
