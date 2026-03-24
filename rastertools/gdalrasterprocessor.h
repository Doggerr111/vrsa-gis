#ifndef GDALRASTERPROCESSOR_H
#define GDALRASTERPROCESSOR_H
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "gdal.h"
#include "cpl_string.h"
#include <QMetaType>
#include "common/rasterprocessingparams.h"
#include <QProcess>
#include <QDebug>
#include "logger.h"

namespace vrsa {
namespace raster {


/**
 * @brief Базовый класс для обработки растров через GDAL команды
 *
 * Позволяет выполнять операции (перепроецирование, обрезка и т.д.)
 * через единый интерфейс GDALGeneralCmdLineProcessor
 */
class GDALRasterProcessor {
public:
    GDALRasterProcessor(common::RasterProcessingParams& params)
        : mParams{params} {}
    virtual ~GDALRasterProcessor() = default;
    virtual bool execute() = 0;
    virtual std::string name()         const = 0;
    virtual common::RasterOperationType type() const = 0;

protected:
    /**
     * @brief Собрать аргументы команды
     * @return вектор аргументов
     */
    virtual std::string buildArgs() const = 0;

    /**
     * @brief Выполнить команду GDAL
     */
    bool executeCommand(const std::string& args) const
    {
        VRSA_INFO("GDAL", "Running: " + args);
        int result = system(args.c_str());
        if (result != 0)
        {
            VRSA_ERROR("GDAL", "Command failed with code:" + std::to_string(result));
            return false;
        }
        VRSA_INFO("GDAL", "Command executed successfully");
        return true;
    }
protected:
    common::RasterProcessingParams mParams;
};

}
}
#endif // GDALRASTERPROCESSOR_H
