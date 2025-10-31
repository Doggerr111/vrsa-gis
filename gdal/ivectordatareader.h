#ifndef IVECTORDATAREADER_H
#define IVECTORDATAREADER_H

#include <memory>
#include <vector>
#include <string>
#include "vector/vectorlayer.h"
#include "common/GisDefines.h"
#include "common/GisTypes.h"
namespace vrsa
{
namespace gdalwrapper
{
using namespace vrsa::common;

class IVectorDataReader {
public:
    virtual ~IVectorDataReader() = default;

    // Основные методы чтения
    virtual std::unique_ptr<VectorLayer> readLayer(const std::string& source) = 0;
    virtual bool canRead(const std::string& source) const = 0;

    // Информационные методы
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
    virtual GeometryType getSupportedGeometryType() const = 0;

    // Метаданные
    virtual int getFeatureCount() const = 0;
    virtual BoundingBox getBoundingBox() const = 0;

    // Batch operations
    virtual std::vector<std::unique_ptr<VectorFeature>> readFeatures(int start = 0, int count = -1) = 0;
};
}
}
#endif // IVECTORDATAREADER_H
