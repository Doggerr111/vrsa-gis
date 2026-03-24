#ifndef RASTERPROCESSINGPARAMS_H
#define RASTERPROCESSINGPARAMS_H
#include <string>
#include <vector>
#include <optional>
#include <QMetaType>
#include <QString>
#include <QObject>
namespace vrsa
{
namespace common
{

enum class ResamplingMethod
{
    Unknown = 0,
    NearestNeighbour,
    Bilinear,
    Cubic,
    CubicSpline,
    Lanczos,
    Average
};

enum class GeoTiffCompression
{
    None = 0,
    LZW,
    Deflate,
    PackBits,
    JPEG,
    JPEG2000,
    CCITTRLE,
    CCITTFAX3,
    CCITTFAX4,
    LZMA,
    ZSTD,
    WEBP
};


enum class RasterOperationType
{
    Unknown = 0,
    Reproject,
    Isolines,
    CutByVectorMask

};

inline QString rasterOperationTypeToString(RasterOperationType type) {
    switch (type) {
        case RasterOperationType::Reproject:
            return QObject::tr("Перепроецирование");
        case RasterOperationType::Isolines:
            return QObject::tr("Изолинии");
        case RasterOperationType::CutByVectorMask:
            return QObject::tr("Обрезка по векторной маске");
        case RasterOperationType::Unknown:
        default:
            return QObject::tr("Неизвестная операция");
    }
}

/**
 * @brief Параметры обработки растра (DTO)
 */
struct RasterProcessingParams
{
    // обязательные параметры
    std::string inputPath;
    std::string outputPath;

    // перепроецирование
    std::optional<std::string> targetCRS; //name
    std::optional<std::string> epsg; //e.g. EPSG:4326
    std::optional<std::string> wkt; //wkt string
    std::optional<std::string> proj; //proj string

    //обрезка
    std::optional<std::string> cutlinePath; //путь к векторному файлу-маске
    bool cropToCutline = true;              //обрезать по контуру
    int cutlineLayer = 0;        //номер слоя в маске

    //обрезка по экстенту
    std::optional<double> xmin;
    std::optional<double> ymin;
    std::optional<double> xmax;
    std::optional<double> ymax;

    //изолинии
    std::optional<std::string> elevationFieldName;
    std::optional<double> interval;

    //опции обработки
    ResamplingMethod resample = ResamplingMethod::Bilinear;  //метод ресемплинга
    GeoTiffCompression compression = GeoTiffCompression::LZW;
    //дополнительные опции
    std::vector<std::string> extraOptions;      //дополнительные -co опции
    bool overwrite = true;
    bool progress = true;

    //выходной формат
    std::string outputFormat = "GTiff";         //формат выходного файла

    /**
     * @brief Преобразовать метод ресемплинга в строку для GDAL
     */
    std::string resampleToString() const
    {
        switch (resample)
        {
            case ResamplingMethod::NearestNeighbour: return "near";
            case ResamplingMethod::Bilinear: return "bilinear";
            case ResamplingMethod::Cubic: return "cubic";
            case ResamplingMethod::CubicSpline: return "cubicspline";
            case ResamplingMethod::Lanczos: return "lanczos";
            case ResamplingMethod::Average: return "average";
            default: return "bilinear";
        }
    }

    /**
     * @brief Преобразовать метод сжатия в строку для GDAL
     */
    std::string compressionToString() const
    {
        switch (compression)
        {
            case GeoTiffCompression::None:      return "NONE";
            case GeoTiffCompression::LZW:       return "LZW";
            case GeoTiffCompression::Deflate:   return "DEFLATE";
            case GeoTiffCompression::PackBits:  return "PACKBITS";
            case GeoTiffCompression::JPEG:      return "JPEG";
            case GeoTiffCompression::JPEG2000:  return "JPEG2000";
            case GeoTiffCompression::CCITTRLE:  return "CCITTRLE";
            case GeoTiffCompression::CCITTFAX3: return "CCITTFAX3";
            case GeoTiffCompression::CCITTFAX4: return "CCITTFAX4";
            case GeoTiffCompression::LZMA:      return "LZMA";
            case GeoTiffCompression::ZSTD:      return "ZSTD";
            case GeoTiffCompression::WEBP:      return "WEBP";
            default: return "LZW";
        }
    }


    /**
     * @brief Проверка валидности параметров
     */
    bool isValid() const {
        if (inputPath.empty() || outputPath.empty()) return false;
        if (inputPath == outputPath) return false;
        return true;
    }
};


}
}
Q_DECLARE_METATYPE(vrsa::common::ResamplingMethod);
Q_DECLARE_METATYPE(vrsa::common::RasterProcessingParams);
Q_DECLARE_METATYPE(vrsa::common::RasterOperationType);
Q_DECLARE_METATYPE(vrsa::common::GeoTiffCompression);

#endif // RASTERPROCESSINGPARAMS_H
