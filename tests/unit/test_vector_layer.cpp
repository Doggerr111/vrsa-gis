#include <gtest/gtest.h>
#include "gdal/gdalreader.h"
#include <QDebug>
#include <QDir>
#include "GisDefines.h"
#include "gdal/dataset.h"
#include "vector/vectordataset.h"
#include "vectorlayertools/vectorlayercreator.h"


class VectorLayerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        testDir = QDir::currentPath() +
                QString::fromStdString(vrsa::common::TEST_VECTOR_DATA_FOLDER) + "/vector_creator_test";
        QDir dir;
        if (!dir.exists(testDir))
            dir.mkpath(testDir);
        layerDef.crs = "WGS 84";
        layerDef.filePath = testDir.toStdString()+".shp";
        layerDef.format = "ESRI Shapefile";
        layerDef.geometryType = static_cast<int>(vrsa::common::GeometryType::Point);
    }

    void TearDown() override
    {
        //очищаем тестовые файлы
        QDir dir(testDir);
        dir.removeRecursively();
    }

    QString testDir;
    vrsa::common::LayerDefinition layerDef;
};



// Тест 1: Тестирование добавления/удаления векторных объектов
TEST_F(VectorLayerTest, createFeaturesTest)
{
    using namespace vrsa;
    //===создание слоя===
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

    //===тестирование слоя===
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);
    ASSERT_NE(vectorLayer->getOGRLayer(), nullptr);
    ASSERT_EQ(vectorLayer->getGeomType(), common::GeometryType::Point);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getFieldCount(), 0);
    ASSERT_EQ(vectorLayer->getBoundingBox(), QRectF());
    ASSERT_FALSE(vectorLayer->getNameAsString().empty());
    auto feature1 = vector::VectorFeature::createFeature(vectorLayer);
    ASSERT_NE(feature1, nullptr);
    vectorLayer->addFeature(std::move(feature1));
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 1);
    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 1);
    auto featurefromlayer = vectorLayer->getFeatureAt(0);
    ASSERT_NE(featurefromlayer, nullptr);
    auto fid = featurefromlayer->getFID();
    ASSERT_EQ(fid, 0);
    bool fl = vectorLayer->deleteFeature(fid);
    ASSERT_TRUE(fl);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 0);
    auto feature2 = vector::VectorFeature::createFeature(vectorLayer);
    ASSERT_NE(feature2, nullptr);
    vectorLayer->addFeature(std::move(feature2));
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 1);
    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 1);
    auto featurefromlayer2 = vectorLayer->getFeatureAt(0);
    ASSERT_NE(featurefromlayer2, nullptr);
    bool fl2 = vectorLayer->deleteFeature(featurefromlayer2);
    ASSERT_TRUE(fl2);
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 0);
}


// Тест 2: Тестирование атрибутов
TEST_F(VectorLayerTest, layerAttributesFieldsTest)
{
    using namespace vrsa;
    //===создание слоя===
    //аттрибуты
    common::FieldDefinition def;
    def.name="attr1";
    def.typeIndex=static_cast<int>(common::FieldType::Integer);

    common::FieldDefinition def2;
    def2.name="attr2";
    def2.typeIndex=static_cast<int>(common::FieldType::Real);

    common::FieldDefinition def3;
    def3.name="attr3";
    def3.typeIndex=static_cast<int>(common::FieldType::String);

    common::FieldDefinition def4;
    def4.name="attr4";
    def4.typeIndex=static_cast<int>(common::FieldType::DateTime);

    layerDef.fields = std::vector<common::FieldDefinition>{def, def2, def3, def4};

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

    //===тестирование слоя===
    auto vectorLayer = vectorDs->getLayer(0);
    ASSERT_NE(vectorLayer, nullptr);

    ASSERT_EQ(vectorLayer->getFieldCount(), 4);
    ASSERT_FALSE(vectorLayer->getFieldNames().empty());
    ASSERT_TRUE(vectorLayer->hasField("attr1"));
    ASSERT_TRUE(vectorLayer->hasField("attr2"));
    ASSERT_TRUE(vectorLayer->hasField("attr3"));
    ASSERT_TRUE(vectorLayer->hasField("attr4"));

    ASSERT_EQ(vectorLayer->getFieldType("attr1"), common::FieldType::Integer);

    auto feature1 = vector::VectorFeature::createFeature(vectorLayer);
    ASSERT_NE(feature1, nullptr);
    vectorLayer->addFeature(std::move(feature1));
    ASSERT_EQ(vectorLayer->getFeaturesCount(), 1);
    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 1);





//    auto featurefromlayer = vectorLayer->getFeatureAt(0);
//    ASSERT_NE(featurefromlayer, nullptr);
//    auto fid = featurefromlayer->getFID();
//    ASSERT_EQ(fid, 0);
//    bool fl = vectorLayer->deleteFeature(fid);
//    ASSERT_TRUE(fl);
//    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
//    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 0);
//    auto feature2 = vector::VectorFeature::createFeature(vectorLayer);
//    ASSERT_NE(feature2, nullptr);
//    vectorLayer->addFeature(std::move(feature2));
//    ASSERT_EQ(vectorLayer->getFeaturesCount(), 1);
//    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 1);
//    auto featurefromlayer2 = vectorLayer->getFeatureAt(0);
//    ASSERT_NE(featurefromlayer2, nullptr);
//    bool fl2 = vectorLayer->deleteFeature(featurefromlayer2);
//    ASSERT_TRUE(fl2);
//    ASSERT_EQ(vectorLayer->getFeaturesCount(), 0);
//    ASSERT_EQ(vectorLayer->getOGRLayer()->GetFeatureCount(), 0);
}

