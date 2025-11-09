#include <gtest/gtest.h>
#include "gdal/gdalreader.h"
#include <QDebug>
// Тест-сьют для VectorFeature
TEST(VectorReaderTest, shpPointRead) {
    vrsa::gdalwrapper::GDALReader reader;
    GDALAllRegister();
    auto test_dS1=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/KostromskayaBoundary.shp");
    auto test_dS2=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/testMultiLayers.gpx");
    auto toVectorDs1 = static_cast<vrsa::vector::VectorDataset*>(test_dS1.get());
    auto toVectorDs2 = static_cast<vrsa::vector::VectorDataset*>(test_dS2.get());
    ASSERT_EQ(test_dS1->GetDatasetType(), vrsa::common::DatasetType::Vector);
    ASSERT_NE(test_dS1, nullptr);
    ASSERT_EQ(toVectorDs1->layersCount(), 1);

    ASSERT_EQ(test_dS2->GetDatasetType(), vrsa::common::DatasetType::Vector);
    ASSERT_NE(test_dS2, nullptr);
    ASSERT_EQ(toVectorDs2->layersCount(), 5);

    //EXPECT_TRUE(dS!=nullptr);
//    EXPECT_TRUE(feature.isVisible());
//    EXPECT_EQ(feature.geometry(), nullptr);
}
