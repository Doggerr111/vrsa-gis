#ifndef CLIPPROCESSOR_H
#define CLIPPROCESSOR_H
#include "gdalrasterprocessor.h"

namespace vrsa
{
namespace raster
{

class ClipProcessor : public GDALRasterProcessor
{
public:
    ClipProcessor(common::RasterProcessingParams& params);

    // GDALRasterProcessor interface
public:
    bool execute() override;
    std::string name() const override { return "Обрезка растра по векторной маске"; };
    common::RasterOperationType type() const override { return common::RasterOperationType::CutByVectorMask; };

protected:
    std::string buildArgs() const override;
};

}
}

#endif // CLIPPROCESSOR_H
