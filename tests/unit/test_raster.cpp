// tests/unit/test_vector_feature.cpp
#include <gtest/gtest.h>
#include "raster/rasterdataset.h"
#include "gdal/gdalreader.h"
#include "graphics/rastergraphicsitemfactory.h"

void createTestMultiBandDataset(const std::string& filename);

// Тест-сьют для VectorFeature
TEST(RasterTest, DefaultConstruction) {
    //vrsa::vector::VectorFeature feature;
    createTestMultiBandDataset("tests.tiff");
    vrsa::gdalwrapper::GDALReader reader;
    auto ptr = reader.readDataset("tests.tiff");

    auto rDs = static_cast<vrsa::raster::RasterDataset*>(ptr.get());
    EXPECT_TRUE(rDs);
    EXPECT_EQ(rDs->getChannels().size(), 3);
    //QImage image = vrsa::graphics::createRGBImage(rDs->getChannels());
    //EXPECT_EQ(image.isNull(), false);
    //EXPECT_TRUE(true);
//    EXPECT_TRUE(feature.isVisible());
//    EXPECT_EQ(feature.geometry(), nullptr);
}

void createTestMultiBandDataset(const std::string& filename)
{
    // Регистрируем драйверы
    GDALAllRegister();

    // Создаем новый dataset
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (!driver) {
        driver = GetGDALDriverManager()->GetDriverByName("MEM"); // In-memory драйвер
    }

    const int width = 100;
    const int height = 100;
    const int numBands = 3; // RGB канала

    GDALDataset* dataset = driver->Create(
        filename.c_str(),
        width,
        height,
        numBands,
        GDT_Byte, // Тип данных
        nullptr
    );

    if (!dataset) {
        throw std::runtime_error("Failed to create dataset");
    }

    // Заполняем каналы тестовыми данными
    for (int bandIdx = 1; bandIdx <= numBands; ++bandIdx) {
        GDALRasterBand* band = dataset->GetRasterBand(bandIdx);

        // Создаем тестовые данные (градиент)
        std::vector<unsigned char> data(width * height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Разные паттерны для разных каналов
                unsigned char value = 0;
                switch (bandIdx) {
                    case 1: // Red
                        value = static_cast<unsigned char>(x * 255 / width);
                        break;
                    case 2: // Green
                        value = static_cast<unsigned char>(y * 255 / height);
                        break;
                    case 3: // Blue
                        value = static_cast<unsigned char>((x + y) * 255 / (width + height));
                        break;
                }
                data[y * width + x] = value;
            }
        }

        // Устанавливаем данные
        CPLErr err = band->RasterIO(
            GF_Write,
            0, 0,
            width, height,
            data.data(),
            width, height,
            GDT_Byte,
            0, 0
        );

        // Устанавливаем метаданные
        band->SetNoDataValue(0);
        band->SetMetadataItem("Description",
            bandIdx == 1 ? "Red channel" :
            bandIdx == 2 ? "Green channel" : "Blue channel");
    }

    // Устанавливаем геоинформацию (опционально)
    double geoTransform[6] = {
        441000.0, // top left x
        10.0,     // pixel width
        0.0,      // rotation (обычно 0)
        3750000.0, // top left y
        0.0,      // rotation (обычно 0)
        -10.0     // pixel height (отрицательный для north-up)
    };
    dataset->SetGeoTransform(geoTransform);

    // Устанавливаем проекцию (WGS84)
    dataset->SetProjection(
        "GEOGCS[\"WGS 84\","
        "DATUM[\"WGS_1984\","
        "SPHEROID[\"WGS 84\",6378137,298.257223563,"
        "AUTHORITY[\"EPSG\",\"7030\"]],"
        "AUTHORITY[\"EPSG\",\"6326\"]],"
        "PRIMEM[\"Greenwich\",0,"
        "AUTHORITY[\"EPSG\",\"8901\"]],"
        "UNIT[\"degree\",0.0174532925199433,"
        "AUTHORITY[\"EPSG\",\"9122\"]],"
        "AUTHORITY[\"EPSG\",\"4326\"]]"
    );

    // Сохраняем и закрываем
    GDALClose(dataset);
    std::cout << "Created test dataset: " << filename << std::endl;
}
