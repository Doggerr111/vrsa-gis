#include "rasterdataset.h"


vrsa::raster::RasterDataset::RasterDataset(gdalwrapper::GdalDatasetPtr dataset, const std::string source, channels channels)
    : Dataset{std::move(dataset), source},
      mRasterChannels{std::move(channels)}
{

}

std::unique_ptr<vrsa::raster::RasterChannel> &vrsa::raster::RasterDataset::getChannel(size_t index)
{
    return mRasterChannels.at(index);
}

QTransform vrsa::raster::RasterDataset::getGeoTransform()
{
    double geoTransform[6];
    if (mDs->GetGeoTransform(geoTransform) != CE_None)
        VRSA_ERROR("RasterDataset", "Raster Dataset without geotransform");

    QTransform transform;
    transform.translate(geoTransform[0], geoTransform[3]);
    transform.scale(geoTransform[1], geoTransform[5]);
    return transform;
}

QRectF vrsa::raster::RasterDataset::getBoundingBox()
{
    double geoTransform[6];
    mDs->GetGeoTransform(geoTransform);
    double minX = geoTransform[0];
    double maxY = geoTransform[3];
    double maxX = geoTransform[0] + geoTransform[1] * mDs->GetRasterXSize();
    double minY = geoTransform[3] + geoTransform[5] * mDs->GetRasterYSize();
    return QRectF(QPointF(minX, minY), QSizeF(maxX - minX, maxY - minY));
}
