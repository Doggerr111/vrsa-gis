#ifndef REPROJECTPROCESSOR_H
#define REPROJECTPROCESSOR_H

#include "gdalrasterprocessor.h"

namespace vrsa
{
namespace raster
{

class ReprojectProcessor : public GDALRasterProcessor
{
public:
    ReprojectProcessor(common::RasterProcessingParams& params);

    // GDALRasterProcessor interface
public:
    bool execute() override;
    std::string name() const override { return "Перепроецирование (трансформация) растра"; };
    common::RasterOperationType type() const override { return common::RasterOperationType::Reproject; };

protected:
    std::string buildArgs() const override;
};

}
}
#endif // REPROJECTPROCESSOR_H
