#ifndef ABSTRACTVECTORREADER_H
#define ABSTRACTVECTORREADER_H

#include "gdal/ivectordatareader.h"
namespace vrsa
{
namespace gdalwrapper
{
class AbstractVectorReader: public IVectorDataReader
{
public:
    AbstractVectorReader();

protected:
    std::string mSource;
    BoundingBox mBounding_box_;
    int mFeatureCount;
    GeometryType mGeometryType;
    std::vector<VectorFeature> mFieldFefinitions;

    // IVectorDataReader interface
public:
    std::unique_ptr<VectorLayer> readLayer(const std::string &source);
    bool canRead(const std::string &source) const;
    std::string getFormatName() const;
    std::vector<std::string> getSupportedExtensions() const;
    GeometryType getSupportedGeometryType() const;
    int getFeatureCount() const;
    BoundingBox getBoundingBox() const;
    std::vector<std::unique_ptr<VectorFeature> > readFeatures(int start, int count);
};

std::unique_ptr<VectorLayer> AbstractVectorReader::readLayer(const std::string &source)
{
}

bool AbstractVectorReader::canRead(const std::string &source) const
{
}

std::string AbstractVectorReader::getFormatName() const
{
}

std::vector<std::string> AbstractVectorReader::getSupportedExtensions() const
{
}

GeometryType AbstractVectorReader::getSupportedGeometryType() const
{
}

int AbstractVectorReader::getFeatureCount() const
{
}

BoundingBox AbstractVectorReader::getBoundingBox() const
{
}

std::vector<std::unique_ptr<VectorFeature> > AbstractVectorReader::readFeatures(int start, int count)
{
}
}
}

#endif // ABSTRACTVECTORREADER_H
