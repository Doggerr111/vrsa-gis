#ifndef ISOLINESPROCESSOR_H
#define ISOLINESPROCESSOR_H
#include "gdalrasterprocessor.h"

namespace vrsa
{
namespace raster
{

class IsolinesProcessor : public GDALRasterProcessor
{
public:
    IsolinesProcessor(common::RasterProcessingParams& params);

    // GDALRasterProcessor interface
public:
    bool execute() override;
    std::string name() const override { return "Создание изолиний"; };
    common::RasterOperationType type() const override { return common::RasterOperationType::Isolines; };

protected:
    std::string buildArgs() const override;
};

}
}

#endif // ISOLINESPROCESSOR_H
