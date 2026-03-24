#include "clipprocessor.h"

vrsa::raster::ClipProcessor::ClipProcessor(common::RasterProcessingParams &params)
    : GDALRasterProcessor(params)
{

}

bool vrsa::raster::ClipProcessor::execute()
{
    auto args = buildArgs();
    return executeCommand(args);
}

std::string vrsa::raster::ClipProcessor::buildArgs() const
{
    std::string args;
    args += "gdalwarp -cutline ";
    args += mParams.cutlinePath.value();
    if (mParams.cropToCutline)
        args += " -crop_to_cutline";
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
