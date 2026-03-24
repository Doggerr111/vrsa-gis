#include "isolinesprocessor.h"

vrsa::raster::IsolinesProcessor::IsolinesProcessor(common::RasterProcessingParams &params)
    : GDALRasterProcessor(params)
{

}

bool vrsa::raster::IsolinesProcessor::execute()
{
    auto args = buildArgs();
    return executeCommand(args);
}

std::string vrsa::raster::IsolinesProcessor::buildArgs() const
{
    std::string args;
    args += "gdal_contour -a ";
    args += mParams.elevationFieldName.value();
    args += " -i ";
    args += std::to_string(mParams.interval.value());
    args += " ";
    args += mParams.inputPath;
    args += " ";
    args += mParams.outputPath;
    return args;
}
