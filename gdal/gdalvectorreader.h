#ifndef GDALVECTORREADER_H
#define GDALVECTORREADER_H


#include <memory>
#include <vector>
#include <string>
#include "vector/vectorlayer.h"
#include "common/GisDefines.h"
#include "common/GisTypes.h"
#include <gdal_priv.h>
#include "gdal/gdaldeleters.h"
#include "common/logger.h"
namespace vrsa
{
namespace gdalwrapper
{
using namespace vrsa::common;

class GDALVectorReader {
public:
    GDALVectorReader();
    ~GDALVectorReader() = default;


    // Основные методы чтения
    std::unique_ptr<VectorLayer> readLayer(const std::string& source);
    bool canRead(const std::string& source) const;

    // Информационные методы
    std::string getFormatName() const;
    std::vector<std::string> getSupportedExtensions() const;
    GeometryType getSupportedGeometryType() const;

    // Метаданные
    int getFeatureCount() const;
    BoundingBox getBoundingBox() const;

    // Batch operations
    std::vector<std::unique_ptr<VectorFeature>> readFeatures(int start = 0, int count = -1);



private:
    std::string mSource;
    BoundingBox mBounding_box_;
    int mFeatureCount;
    GeometryType mGeometryType;
    std::vector<VectorFeature> mFieldFefinitions;
};
}
}

#endif // GDALVECTORREADER_H
