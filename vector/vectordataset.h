#ifndef VECTORDATASET_H
#define VECTORDATASET_H
#include "vector/vectorlayer.h"
#include "gdal/dataset.h"
namespace vrsa{
namespace vector{
/**
 * @english
 * @brief Represents a vector dataset containing multiple layers of geographic data
 *
 * The VectorDataset class encapsulates a complete vector dataset that may contain
 * multiple layers (point, line, polygon). Each layer stores homogeneous geographic
 * features. The dataset maintains connection to the original GDAL dataset and
 * provides access to individual layers and metadata.
 * @endenglish
 * @russian
 * @brief Представляет векторный набор данных, содержащий несколько слоев географических данных
 *
 * Класс VectorDataset инкапсулирует полный векторный набор данных, который может содержать
 * несколько слоев (точечные, линейные, полигональные). Каждый слой хранит однородные
 * географические объекты. Набор данных сохраняет соединение с исходным GDAL набором данных
 * и предоставляет доступ к отдельным слоям и метаданным.
 * @endrussian
 */
class VectorDataset: public vrsa::gdalwrapper::Dataset
{
    using vecLayers=std::vector<std::unique_ptr<vrsa::vector::VectorLayer>>;
public:
    VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, const std::string source, vecLayers = vecLayers());
    std::size_t layersCount();
    std::unique_ptr<VectorLayer>& getLayer(size_t index);
    std::string getSource();
private:
    vecLayers mLayers;
};
}
}
#endif // VECTORDATASET_H
