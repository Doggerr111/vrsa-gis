#include "webrasterdataset.h"
#include <QThread>
#include <QDebug>
vrsa::raster::WebRasterDataset::WebRasterDataset(gdalwrapper::GdalDatasetPtr dataset)
    : gdalwrapper::Dataset(std::move(dataset))
{

}


vrsa::raster::WebRasterDataset::~WebRasterDataset()
{
    //пока так ........................
    int timeout = 5000;
    while (mActiveReads > 0 && timeout > 0)
    {
        QThread::msleep(10);
        timeout -= 10;
    }

    if (mActiveReads > 0) {
        VRSA_WARNING("RASTER", "Timeout waiting for reads to finish");
    }
};

QImage vrsa::raster::WebRasterDataset::readExtent(const QRectF &extent, int width, int height, QRectF &actualExtent)
{
    mActiveReads++;
    if (!mDs) return QImage();
    //получаем геопривязку и вычисляем пиксельные координаты в виртуальном растре
    double gt[6];
//  gt[0] = х координата левого верхнего угла левого верхнего пикселя
//  gt[1] = разрешение по х (ширина пикселя в единицах карты)
//  gt[2] = поворот по х (должна быть 0)
//  gt[3] = у координата левого верхнего угла левого верхнего пикселя
//  gt[4] = поворот по у (обычно 0)
//  gt[5] = разрешение по у (высота пикселя, ОТРИЦАТЕЛЬНАЯ для "север-вверх")
    mDs->GetGeoTransform(gt);
//    qDebug() << "=== GDAL GeoTransform ===";
//    qDebug() << "gt[0] (top-left X):" << gt[0];
//    qDebug() << "gt[1] (pixel width):" << gt[1];
//    qDebug() << "gt[2] (X rotation):" << gt[2];
//    qDebug() << "gt[3] (top-left Y):" << gt[3];
//    qDebug() << "gt[4] (Y rotation):" << gt[4];
//    qDebug() << "gt[5] (pixel height):" << gt[5];
//    qDebug()<<"MAP EXTENT" << extent;
//    qDebug()<< "WIDGET WIDTH" << width;

    int rasterXSize = mDs->GetRasterXSize();
    int rasterYSize = mDs->GetRasterYSize();

    double minWorldX = gt[0];
    double maxWorldX = gt[0] + rasterXSize * gt[1];
    double minWorldY = gt[3] + rasterYSize * gt[5];  // gt[5] отрицательный
    double maxWorldY = gt[3];

    //ограничиваем экстент
    QRectF safeExtent = extent;
    safeExtent.setLeft(std::max(extent.left(), minWorldX));
    safeExtent.setRight(std::min(extent.right(), maxWorldX));
    safeExtent.setTop(std::max(extent.top(), minWorldY));
    safeExtent.setBottom(std::min(extent.bottom(), maxWorldY));
    qDebug()<<"MAP EXTENT SAFE" << safeExtent;
    if (safeExtent.isNull() || safeExtent.width() <= 0)
        return QImage();

    //пиксельные координаты
    int xoff = (safeExtent.left() + abs(minWorldX)) / gt[1];
    int yoff = (maxWorldY - safeExtent.top()) / abs(gt[5]);
    int xsize = safeExtent.width() / gt[1];
    int ysize = abs(safeExtent.height()) / abs(gt[5]);

    //защита от выхода за границы
    xoff = std::max(0, std::min(xoff, rasterXSize - 1));
    yoff = std::max(0, std::min(yoff, rasterYSize - 1));
    xsize = std::min(xsize, rasterXSize - xoff);
    ysize = std::min(ysize, rasterYSize - yoff);

//    qDebug()<< "======test calc========";
//    qDebug() << "начинаем в х = " <<xoff;
//    qDebug() << "начинаем в y = " <<yoff;
//    qDebug() << "xsize" << xsize;
//    qDebug()<< "ysize" << ysize;


    if (xsize <= 0 || ysize <= 0)
        return QImage();

    std::vector<unsigned char> buffer(width * height * 3);
    int nPixelSpace = 3 * sizeof(unsigned char);  // 3 байта на пиксель
    int nLineSpace = width * nPixelSpace;          // сколько байт на строку
    int nBandSpace = sizeof(unsigned char);        // расстояние между каналами одного пикселя

    if (!mDs) return QImage();
    CPLErr err = mDs->RasterIO(
        GF_Read,
        xoff, yoff, xsize, ysize,
        buffer.data(), width, height,
        GDT_Byte,
        3,
        nullptr,
        nPixelSpace, nLineSpace, nBandSpace,
        nullptr
    );
    mActiveReads--;
    if (err != CE_None)
            return QImage();
    actualExtent = safeExtent;
    return QImage(buffer.data(), width, height,
                  width * 3, QImage::Format_RGB888).copy();

}
