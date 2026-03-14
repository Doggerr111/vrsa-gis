#include <gtest/gtest.h>
#include <QDir>
#include "vector/vectorfeature.h"
#include "vectorlayertools/vectorlayercreator.h"
#include "gdal/gdalreader.h"
#include "vector/vectordataset.h"
#include "GisDefines.h"

using namespace vrsa;
using namespace vrsa::vector;

class VectorFeatureTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        //создаем тестовую директорию
        testDir = QDir::currentPath() +
                QString::fromStdString(vrsa::common::TEST_VECTOR_DATA_FOLDER) + "/vector_feature_test";
        QDir dir;
        if (!dir.exists(testDir))
            dir.mkpath(testDir);

        common::LayerDefinition layerDef;
        layerDef.crs = "WGS 84";
        layerDef.filePath = (testDir + "/test_layer.shp").toStdString();
        layerDef.format = "ESRI Shapefile";
        layerDef.geometryType = static_cast<int>(common::GeometryType::Point);

        common::FieldDefinition def;
        def.name="int_field";
        def.typeIndex=static_cast<int>(common::FieldType::Integer64); //потому что все целые числа храним в int64_t

        common::FieldDefinition def2;
        def2.name="double_fie";
        def2.typeIndex=static_cast<int>(common::FieldType::Real);

        common::FieldDefinition def3;
        def3.name="string_fie";
        def3.typeIndex=static_cast<int>(common::FieldType::String);

        layerDef.fields = std::vector<common::FieldDefinition>{def, def2, def3};

        // Создаем слой
        VectorLayerCreator creator;
        auto ds = creator.createGDALDataset(layerDef);
        ASSERT_NE(ds, nullptr);

        // ПРИНУДИТЕЛЬНО СБРАСЫВАЕМ КЭШ НА ДИСК
        ds->FlushCache();  // гарантирует запись всех данных
        ds.reset();
        gdalwrapper::GDALReader reader;
        mDs = reader.readDataset(testDir.toStdString() + "/test_layer.shp");
        ASSERT_NE(mDs, nullptr);
        vectorDs = static_cast<vector::VectorDataset*>(mDs.get());
        ASSERT_NE(vectorDs, nullptr);
        m_layer = vectorDs->getLayer(0);
        ASSERT_NE(m_layer, nullptr);

        //сздаем тестовую фичу
        auto featUptr = VectorFeature::createFeature(m_layer);
        m_feature = featUptr.get();
        bool added = m_layer->addFeature(std::move(featUptr));

        ASSERT_TRUE(m_feature->setAttribute("int_field", 42));
        ASSERT_TRUE(m_feature->setAttribute("double_fie", 3.14159));
        ASSERT_TRUE(m_feature->setAttribute("string_fie", "test_value"));
        ASSERT_TRUE(m_feature->setGeometry(new OGRPoint(10,20)));
        ASSERT_TRUE(m_feature->setAttribute("string_fie", "test_value"));
    }

    void TearDown() override
    {
        //очищаем тестовые файлы
        QDir dir(testDir);
        dir.removeRecursively();
    }

    QString testDir;
    VectorLayer* m_layer = nullptr;
    VectorFeature * m_feature;
    std::unique_ptr<gdalwrapper::Dataset> mDs;
    VectorDataset* vectorDs;
};

// ==================== ТЕСТЫ АТРИБУТОВ ====================


TEST_F(VectorFeatureTest, GetExistingAttribute)
{

    ASSERT_EQ(m_layer->getFeaturesCount(), 1);

    auto intVal = m_feature->getAttribute<int>("int_field");
    EXPECT_EQ(intVal, 42);

    auto doubleVal = m_feature->getAttribute<double>("double_fie");
    EXPECT_DOUBLE_EQ(doubleVal, 3.14159);

    auto stringVal = m_feature->getAttribute<std::string>("string_fie");
    EXPECT_EQ(stringVal, "test_value");


}

TEST_F(VectorFeatureTest, GetAttributeWithDefault)
{

    ASSERT_EQ(m_layer->getFeaturesCount(), 1);
    ASSERT_TRUE(m_feature->setAttribute("int_field", 42));
    ASSERT_TRUE(m_feature->setAttribute("double_fie", 3.14159));
    ASSERT_TRUE(m_feature->setAttribute("string_fie", "test_value"));

    auto intVal = m_feature->getAttribute<int>("int_field", 100);
    EXPECT_EQ(intVal, 42);

    auto missingVal = m_feature->getAttribute<int>("missing_field", 999);
    EXPECT_EQ(missingVal, 999);

    auto wrongType = m_feature->getAttribute<std::string>("int_field", "default");
    EXPECT_EQ(wrongType, "default");
}

TEST_F(VectorFeatureTest, GetAttributeThrowsForMissing)
{
    ASSERT_EQ(m_layer->getFeaturesCount(), 1);
    ASSERT_TRUE(m_feature->setAttribute("int_field", 42));
    ASSERT_TRUE(m_feature->setAttribute("double_fie", 3.14159));
    ASSERT_TRUE(m_feature->setAttribute("string_fie", "test_value"));

    EXPECT_ANY_THROW(m_feature->getAttribute<int>("missing_field"));
}

TEST_F(VectorFeatureTest, HasAttribute)
{
    EXPECT_TRUE(m_feature->hasAttribute("int_field"));
    EXPECT_FALSE(m_feature->hasAttribute("missing_field"));
}

TEST_F(VectorFeatureTest, GetAttributeNames)
{
    auto names = m_feature->getFieldNames();
    EXPECT_GE(names.size(), 3);
    // Проверяем наличие всех полей
    std::vector<std::string> expected = {"int_field", "double_fie", "string_fie"};
    for (const auto& exp : expected) {
        EXPECT_NE(std::find(names.begin(), names.end(), exp), names.end());
    }
}

TEST_F(VectorFeatureTest, GetFieldType)
{
    EXPECT_EQ(m_feature->getFieldType("int_field"), common::FieldType::Integer);
    EXPECT_EQ(m_feature->getFieldType("double_fie"), common::FieldType::Real);
    EXPECT_EQ(m_feature->getFieldType("string_fie"), common::FieldType::String);
    EXPECT_EQ(m_feature->getFieldType("missing_field"), common::FieldType::Unknown);
}

TEST_F(VectorFeatureTest, GetFieldCount)
{
    EXPECT_GE(m_feature->getFieldCount(), 3);
}

TEST_F(VectorFeatureTest, GetAttributeAsString)
{
    auto intf = m_feature->getAttributeAsString("int_field");
    auto doublef = m_feature->getAttributeAsString("double_fie");
    auto stringF = m_feature->getAttributeAsString("string_fie");
    auto unknF = m_feature->getAttributeAsString("missing_field");
    EXPECT_EQ(m_feature->getAttributeAsString("int_field"), "42");
    //EXPECT_EQ(m_feature->getAttributeAsString("double_fie"), "3.14159");
    EXPECT_EQ(m_feature->getAttributeAsString("string_fie"), "test_value");
    EXPECT_EQ(m_feature->getAttributeAsString("missing_field"), "NULL__"); //CHANGE IT TO NULL
}

TEST_F(VectorFeatureTest, GetAttributeAsQVariant)
{
    EXPECT_EQ(m_feature->getAttributeAsQVariant("int_field").toInt(), 42);
    EXPECT_DOUBLE_EQ(m_feature->getAttributeAsQVariant("double_fie").toDouble(), 3.14159);
    EXPECT_EQ(m_feature->getAttributeAsQVariant("string_fie").toString().toStdString(), "test_value");
    //EXPECT_EQ(m_feature->getAttributeAsQVariant("bool_field").toBool(), true);
    EXPECT_FALSE(m_feature->getAttributeAsQVariant("missing_field").isValid());
}

TEST_F(VectorFeatureTest, GetAttributesAsString)
{
    auto attrs = m_feature->getAttributesAsString();
    EXPECT_EQ(attrs["int_field"], "42");
    double expected = 3.14159;
    double actual = std::stod(attrs["double_fie"]);
    EXPECT_DOUBLE_EQ(expected, actual);
    EXPECT_EQ(attrs["string_fie"], "test_value");
}

TEST_F(VectorFeatureTest, GetAttributesAsQString)
{
    auto attrs = m_feature->getAttributesAsQString();
    EXPECT_EQ(attrs["int_field"].toStdString(), "42");
    double expected = 3.14159;
    double actual = (attrs["double_fie"]).toDouble();
    EXPECT_DOUBLE_EQ(expected, actual);
    EXPECT_EQ(attrs["string_fie"].toStdString(), "test_value");
}

// ==================== ТЕСТЫ ИЗМЕНЕНИЯ АТРИБУТОВ ====================

TEST_F(VectorFeatureTest, SetIntAttribute)
{
    EXPECT_TRUE(m_feature->setAttribute("int_field", 100));
    EXPECT_EQ(m_feature->getAttribute<int>("int_field"), 100);
    //проверяем синхронизацис с OGR
    OGRFeature* ogrFeat = m_feature->getOGRFeature();
    EXPECT_EQ(ogrFeat->GetFieldAsInteger("int_field"), 100);
}

TEST_F(VectorFeatureTest, SetDoubleAttribute)
{
    EXPECT_TRUE(m_feature->setAttribute("double_fie", 2.71828));
    EXPECT_DOUBLE_EQ(m_feature->getAttribute<double>("double_fie"), 2.71828);
    //проверяем синхронизацис с OGR
    OGRFeature* ogrFeat = m_feature->getOGRFeature();
    EXPECT_DOUBLE_EQ(ogrFeat->GetFieldAsDouble("double_fie"), 2.71828);
}

TEST_F(VectorFeatureTest, SetStringAttribute)
{
    std::string newVal = "new_value";
    EXPECT_TRUE(m_feature->setAttribute("string_fie", newVal));
    EXPECT_EQ(m_feature->getAttribute<std::string>("string_fie"), newVal);
    OGRFeature* ogrFeat = m_feature->getOGRFeature();
    EXPECT_EQ(std::string(ogrFeat->GetFieldAsString("string_fie")), newVal);
}


TEST_F(VectorFeatureTest, SetAttributeWithQVariant)
{
    EXPECT_TRUE(m_feature->setAttribute("int_field", QVariant(999)));
    EXPECT_EQ(m_feature->getAttribute<int>("int_field"), 999);
    EXPECT_TRUE(m_feature->setAttribute("string_fie", QVariant("qvariant_test")));
    EXPECT_EQ(m_feature->getAttribute<std::string>("string_fie"), "qvariant_test");
}

TEST_F(VectorFeatureTest, SetAttributeWithConstChar)
{
    EXPECT_TRUE(m_feature->setAttribute("string_fie", "c_string_test"));
    EXPECT_EQ(m_feature->getAttribute<std::string>("string_fie"), "c_string_test");
}

TEST_F(VectorFeatureTest, SetAttributeWithAttributeValue)
{
    using AttributeValue = VectorFeature::AttributeValue;
    AttributeValue intVal(777);
    EXPECT_TRUE(m_feature->setAttribute("int_field", intVal));
    EXPECT_EQ(m_feature->getAttribute<int>("int_field"), 777);
    AttributeValue stringVal(std::string("attr_value_test"));
    EXPECT_TRUE(m_feature->setAttribute("string_fie", stringVal));
    EXPECT_EQ(m_feature->getAttribute<std::string>("string_fie"), "attr_value_test");
}

TEST_F(VectorFeatureTest, SetNullAttribute)
{
    using AttributeValue = VectorFeature::AttributeValue;
    AttributeValue nullVal(nullptr);
    EXPECT_TRUE(m_feature->setAttribute("string_fie", nullVal));
    //проверяем синхронизацис с OGR
    OGRFeature* ogrFeat = m_feature->getOGRFeature();
    int fieldIndex = ogrFeat->GetFieldIndex("string_fie");
    EXPECT_TRUE(ogrFeat->IsFieldNull(fieldIndex));
}

TEST_F(VectorFeatureTest, SetInvalidFieldName)
{
    EXPECT_FALSE(m_feature->setAttribute("nonexistent_field", 123));
    EXPECT_FALSE(m_feature->hasAttribute("nonexistent_field"));
}

// ==================== ТЕСТЫ ГЕОМЕТРИИ ====================

TEST_F(VectorFeatureTest, GetGeometryType)
{
    EXPECT_EQ(m_feature->getGeometryType(), common::GeometryType::Point);
    EXPECT_EQ(m_feature->getOGRGeometryType(), wkbPoint);
}

TEST_F(VectorFeatureTest, GetOGRGeometry)
{
    OGRGeometry* geom = m_feature->getOGRGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->getGeometryType(), wkbPoint);

    OGRPoint* point = dynamic_cast<OGRPoint*>(geom);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->getX(), 10.0);
    EXPECT_DOUBLE_EQ(point->getY(), 20.0);
}

TEST_F(VectorFeatureTest, CloneOGRGeometry)
{
    auto clonedGeom = m_feature->cloneOGRGeometry();
    ASSERT_NE(clonedGeom, nullptr);

    OGRPoint* point = dynamic_cast<OGRPoint*>(clonedGeom.get());
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->getX(), 10.0);
    EXPECT_DOUBLE_EQ(point->getY(), 20.0);
}

TEST_F(VectorFeatureTest, IsMultiGeometry)
{
    EXPECT_FALSE(m_feature->isMultiGeometry()); // точка не мульти
}

// ==================== ТЕСТЫ СОСТОЯНИЯ ====================

TEST_F(VectorFeatureTest, GetFID)
{
    EXPECT_GE(m_feature->getFID(), 0);
}

TEST_F(VectorFeatureTest, GetParentLayer)
{
    EXPECT_EQ(m_feature->getOGRLayer(), m_layer->getOGRLayer());
}

TEST_F(VectorFeatureTest, Visibility)
{
    EXPECT_TRUE(m_feature->isVisible()); // по умолчанию true

    m_feature->setVisible(false);
    EXPECT_FALSE(m_feature->isVisible());

    m_feature->setVisible(true);
    EXPECT_TRUE(m_feature->isVisible());
}

TEST_F(VectorFeatureTest, Selection)
{
    EXPECT_FALSE(m_feature->isSelected());

    m_feature->setSelected(true);
    EXPECT_TRUE(m_feature->isSelected());

    m_feature->setSelected(false);
    EXPECT_FALSE(m_feature->isSelected());
}

TEST_F(VectorFeatureTest, ZValue)
{
    EXPECT_EQ(m_feature->getZValue(), -1); // по умолчанию 0

    m_feature->setZValue(5);
    EXPECT_EQ(m_feature->getZValue(), 5);
}

// ==================== ТЕСТЫ КЛОНИРОВАНИЯ ====================

TEST_F(VectorFeatureTest, CloneFeature)
{
    auto clone = m_feature->clone();
    ASSERT_NE(clone, nullptr);

    // Проверяем атрибуты
    EXPECT_EQ(clone->getAttribute<int>("int_field"), 42);
    EXPECT_DOUBLE_EQ(clone->getAttribute<double>("double_fie"), 3.14159);

    // Проверяем геометрию
    EXPECT_EQ(clone->getGeometryType(), common::GeometryType::Point);

    // Проверяем состояние
    EXPECT_TRUE(clone->isVisible());
    EXPECT_FALSE(clone->isSelected());
}
