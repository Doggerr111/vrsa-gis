#include "reprojectprocessor.h"

vrsa::raster::ReprojectProcessor::ReprojectProcessor(common::RasterProcessingParams &params)
    : GDALRasterProcessor(params)
{

}

bool vrsa::raster::ReprojectProcessor::execute()
{
    auto args = buildArgs();
    return executeCommand(args);
}

std::string vrsa::raster::ReprojectProcessor::buildArgs() const
{
    std::string args;
    args += "gdalwarp -t_srs ";
    if (mParams.epsg.has_value())
        args += mParams.epsg.value();
    else if (mParams.wkt.has_value())
        args += mParams.wkt.value();
    else if (mParams.proj.has_value())
        args += mParams.proj.value();
    args += " -r ";
    args += mParams.resampleToString();
    args += " -co COMPRESS=";
    args += mParams.compressionToString();
    args += " ";
    if (mParams.overwrite)
        args += "-overwrite ";
    args += mParams.inputPath;
    args += " ";
    args += mParams.outputPath;
    return args;
}
