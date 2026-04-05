#ifndef GISEXCEPTIONS_H
#define GISEXCEPTIONS_H
#include <exception>
#include <string>
#include <gdal_priv.h>

namespace vrsa{
namespace common{
/**
 * @file GISExceptions.h
 *
 * @brief Иерархия классов исключений для ГИС системы
 *
 * Определяет иерархию исключений для обработки ошибок в ГИС системе.
 * Базовым классом является GISException, унаследованный от std::exception.
 * Все специфические исключения наследуются от него, образуя логические группы:
 * - CoordinateException — ошибки координат
 * - DataException — ошибки работы с данными
 * - GeometryException — ошибки геометрии
 * - AnalysisException — ошибки пространственного анализа * - GraphicsException — ошибки графики/отрисовки
 */

/**
 * @brief Базовый класс для всех исключений ГИС системы
 *
 * Наследуется от std::exception и предоставляет хранение текстового сообщения
 * об ошибке. Все специфические исключения наследуются от этого класса.
 */

class GISException : public std::exception {
private:
    std::string mMessage;

public:
    explicit GISException(const std::string& message)
        : mMessage(message) {}

    const char* what() const noexcept override
    {
        return mMessage.c_str();
    }
};

/**
 * @brief Базовый класс для исключений, связанных с координатами
 *
 * Добавляет префикс "Coordinate Error: " к сообщению об ошибке.
 * Используется как основа для более специфичных координатных исключений.
 */
class CoordinateException : public GISException {
public:
    explicit CoordinateException(const std::string& message)
        : GISException("Coordinate Error: " + message) {}
};

/**
 * @brief Исключение: некорректные координаты
 *
 * Генерируется при попытке создать или обработать координаты с недопустимыми
 * значениями (например, широта вне диапазона [-90, 90]).
 */
class InvalidCoordinateException : public CoordinateException {
public:
    InvalidCoordinateException(double lat, double lon)
        : CoordinateException("Invalid coordinates: lat=" +
                             std::to_string(lat) + ", lon=" +
                             std::to_string(lon)) {}
};


/**
 * @brief Исключение: выход координат за допустимые границы
 *
 * Генерируется, когда координаты выходят за пределы заданной области
 * (например, за границы тайла или кадра).
 */
class OutOfBoundsException : public CoordinateException {
public:
    OutOfBoundsException(double lat, double lon, const std::string& bounds)
        : CoordinateException("Coordinates out of bounds: (" +
                             std::to_string(lat) + ", " +
                             std::to_string(lon) + ") beyond " + bounds) {}
};

/**
 * @brief Базовый класс для исключений при работе с данными
 *
 * Добавляет префикс "Data Error: " к сообщению.
 * Используется для ошибок доступа к данным, форматам, CRS и т.д.
 */
class DataException : public GISException {
public:
    explicit DataException(const std::string& message)
        : GISException("Data Error: " + message) {}
};

class DataSetOpenException : public DataException {
public:
    explicit DataSetOpenException(const std::string& filename)
        : DataException("Failed to open data source: " + filename) {}
};


class DataSetNotFoundException : public DataException {
public:
    explicit DataSetNotFoundException(const std::string& filename)
        : DataException("Data set not found: " + filename) {}
};

class DataFormatException : public DataException {
public:
    DataFormatException(const std::string& format, const std::string& details = "")
        : DataException("Invalid data format: " + format +
                       (details.empty() ? "" : " - " + details)) {}
};

class CRSException : public DataException {
public:
    CRSException(const std::string& crs, const std::string& operation)
        : DataException("CRS operation failed: " + operation +
                       " for CRS: " + crs) {}
};

class GDALDataTypeException: public DataException {
public:
    GDALDataTypeException(GDALDataType type)
        : DataException("Unsupported GDAL data type:" + std::string(GDALGetDataTypeName(type))) {}
};

/**
 * @brief Базовый класс для исключений, связанных с геометрией
 *
 * Добавляет префикс "Geometry Error: " к сообщению.
 * Используется для ошибок создания, модификации и анализа геометрий.
 */
class GeometryException : public GISException {
public:
    explicit GeometryException(const std::string& message)
        : GISException("Geometry Error: " + message) {}
};

class InvalidGeometryException : public GeometryException {
public:
    explicit InvalidGeometryException(const std::string& type)
        : GeometryException("Invalid geometry: " + type) {}
};

class TopologyException : public GeometryException {
public:
    TopologyException(const std::string& operation, const std::string& reason = "")
        : GeometryException("Topology error in " + operation +
                           (reason.empty() ? "" : ": " + reason)) {}
};

/**
 * @brief Базовый класс для исключений при пространственном анализе
 *
 * Добавляет префикс "Analysis Error: " к сообщению.
 * Используется для ошибок в операциях буферизации, пересечения,
 * объединения и других аналитических функциях.
 */
class AnalysisException : public GISException {
public:
    explicit AnalysisException(const std::string& message)
        : GISException("Analysis Error: " + message) {}
};

class BufferException : public AnalysisException {
public:
    BufferException(double distance, const std::string& reason = "")
        : AnalysisException("Buffer operation failed for distance " +
                           std::to_string(distance) +
                           (reason.empty() ? "" : ": " + reason)) {}
};

class IntersectionException : public AnalysisException {
public:
    IntersectionException(const std::string& geom1, const std::string& geom2)
        : AnalysisException("Intersection failed between " + geom1 + " and " + geom2) {}
};


/**
 * @brief Базовый класс для исключений при отрисовке графики
 *
 * Добавляет префикс "Graphics Error: " к сообщению.
 * Используется для ошибок рендеринга, работы с символами и слоями.
 */
class GraphicsException : public GISException {
public:
    explicit GraphicsException(const std::string& message)
        : GISException("Graphics Error: " + message) {}
};

class SymbolLayerGeometryException : public GraphicsException {
public:
    explicit SymbolLayerGeometryException(const std::string& rootSymbolName, const std::string& childSymbolName)
        : GraphicsException("Can't add symbol:" + childSymbolName + " to layer symbol:" +
                            rootSymbolName+". Geometry missmatch") {}
};

}
}



#endif // GISEXCEPTIONS_H
