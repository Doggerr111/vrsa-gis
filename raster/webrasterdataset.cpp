#include "webrasterdataset.h"


vrsa::raster::WebRasterDataset::WebRasterDataset(gdalwrapper::GdalDatasetPtr dataset)
    : gdalwrapper::Dataset(std::move(dataset))
{

}

vrsa::raster::WebRasterDataset::~WebRasterDataset() = default;

QImage vrsa::raster::WebRasterDataset::readExtent(const QRectF &extent, int width, int height)
{

    if (!mDs) return QImage();
    //получаем геопривязку и вычисляем пиксельные координаты в виртуальном растре
    double gt[6];
    //        gt[0] = X координата левого верхнего угла левого верхнего пикселя
    //        gt[1] = разрешение по X (ширина пикселя в единицах карты)
    //        gt[2] = поворот по X (должна быть 0)
    //        gt[3] = Y координата левого верхнего угла левого верхнего пикселя
    //        gt[4] = поворот по Y (обычно 0)
    //        gt[5] = разрешение по Y (высота пикселя, ОТРИЦАТЕЛЬНАЯ для "север-вверх")
    mDs->GetGeoTransform(gt);
    int xoff = floor((extent.left() - gt[0]) / gt[1]); //находим с какого пикселя x начинается видимая область
    int yoff = floor((extent.top() - gt[3]) / gt[5]); //для y
    int xsize = ceil(extent.width() / abs(gt[1])); //сколько пикселей читаем по ширине
    int ysize = ceil(extent.height() / abs(gt[5])); //сколько по высоте

    //защита от выхода за границы
    int rasterXSize = mDs->GetRasterXSize();
    int rasterYSize = mDs->GetRasterYSize();
    xoff = std::max(0, std::min(xoff, rasterXSize - 1));
    yoff = std::max(0, std::min(yoff, rasterYSize - 1));
    xsize = std::min(xsize, rasterXSize - xoff);
    ysize = std::min(ysize, rasterYSize - yoff);

    //читаем данные со всех каналов через единый RasterIO
    std::vector<unsigned char> buffer(width * height * 3);

    CPLErr err = mDs->RasterIO(
                GF_Read,
                xoff, yoff, xsize, ysize,           // исходная область
                buffer.data(), width, height,       // выходной буфер 0-255
                GDT_Byte,                           // тип данных (всегда байт для RGB (0-255))
                3,                                  // количество каналов (RGB)
                nullptr,                            // nullptr - читай по порядку с 1
                0, 0, 0, nullptr
                );

    if (err != CE_None)
    {
        VRSA_LOG_GDAL_ERROR("RASTER", "Failed to read raster data");
        return QImage();
    }
    //нормализация не требуется, данные уже в RGB
    return QImage(buffer.data(), width, height,
                  width * 3, QImage::Format_RGB888).copy();

}
