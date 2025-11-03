#include <gtest/gtest.h>
#include "gdal/gdalvectorreader.h"

// Тест-сьют для VectorFeature
TEST(VectorReaderTest, shpPointRead) {
    vrsa::gdalwrapper::GDALVectorReader reader;
    GDALAllRegister();
    auto test_dS1=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/KostromskayaBoundary.shp");
    auto test_dS2=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/testMultiLayers.gpx");
    ASSERT_NE(test_dS1, nullptr);
    ASSERT_EQ(test_dS1->layersCount(), 1);

    ASSERT_NE(test_dS2, nullptr);
    ASSERT_EQ(test_dS2->layersCount(), 5);

    //EXPECT_TRUE(dS!=nullptr);
//    EXPECT_TRUE(feature.isVisible());
//    EXPECT_EQ(feature.geometry(), nullptr);
}
