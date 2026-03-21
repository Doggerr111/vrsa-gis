#ifndef RASTERDATASET_H
#define RASTERDATASET_H
#include "gdal/dataset.h"
#include "raster/rasterchannel.h"
namespace vrsa
{
namespace raster
{

/**
 * @class RasterDataset
 * @brief Базовый класс для растровых данных из файловых источников
 *
 * Этот класс предназначен ТОЛЬКО для работы с растрами из локальных файлов
 * (например, GeoTIFF, JPEG, и др.). Он управляет коллекцией каналов
 * (RasterChannel) и предоставляет доступ к метаданным растра.
 *
 * @note НЕ ИСПОЛЬЗУЕТСЯ для веб-сервисов. Для работы с тайловыми сервисами
 *       (WMS, TMS, XYZ) используется отдельный класс WebRasterDataset,
 *       который напрямую работает с GDALDataset и не создаёт каналы.
 *
 * @see RasterChannel, WebRasterDataset
 */

class RasterDataset: public QObject, public gdalwrapper::Dataset
{
    Q_OBJECT
    using channels = std::vector<std::unique_ptr<RasterChannel>>;
public:
    RasterDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, channels = channels());
    ~RasterDataset();
    std::unique_ptr<RasterChannel>& getChannel(size_t index);
    inline std::size_t channelsCount() const noexcept
    {
        return mRasterChannels.size();
    }
    inline channels& getChannels() noexcept
    {
        return mRasterChannels;
    }
    QTransform getGeoTransform();
    QRectF getBoundingBox();
    inline void setZValue(int zValue) noexcept
    {
        mZValue = zValue;
    }
signals:
    void ZValueChanged(int);
private:
    channels mRasterChannels;
    //double mGeoTransform;
    QTransform mGeoTransform;
    int mZValue;
public:
    RasterDataset();
};
}
}

#endif // RASTERDATASET_H
