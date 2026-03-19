#ifndef WEBRASTERDATASET_H
#define WEBRASTERDATASET_H
#include <dataset.h>
#include "gdal/dataset.h"
namespace vrsa
{
namespace raster
{

/**
 * @class WebRasterDataset
 * @brief Класс для работы с растровыми веб-сервисами (WMS, TMS, XYZ)
 *
 * В отличие от RasterDataset, этот класс НЕ создаёт каналы (RasterChannel) и
 * НЕ загружает все данные в память. Вместо этого он работает напрямую с GDALDataset,
 * используя механизм виртуальных растров и динамическую подгрузку тайлов.
 *
 * @see Dataset, RasterDataset
 */
class WebRasterDataset : public gdalwrapper::Dataset
{
public:
    WebRasterDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset);
    ~WebRasterDataset();
    /**
     * @brief Читает растровые данные по заданному экстенту
     * @param extent Экстент в координатах карты
     * @param width Ширина выходного изображения в пикселях (ширина viewport)
     * @param height Высота выходного изображения в пикселях (высота viewport)
     * @return QImage с прочитанными данными или пустой QImage при ошибке
    */
    QImage readExtent(const QRectF& extent, int width, int height);

};

}
}
#endif // WEBRASTERDATASET_H
