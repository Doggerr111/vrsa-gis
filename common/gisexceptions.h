#ifndef GISEXCEPTIONS_H
#define GISEXCEPTIONS_H
#include <exception>
#include <string>

namespace vrsa{
namespace common{

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


class CoordinateException : public GISException {
public:
    explicit CoordinateException(const std::string& message)
        : GISException("Coordinate Error: " + message) {}
};

class InvalidCoordinateException : public CoordinateException {
public:
    InvalidCoordinateException(double lat, double lon)
        : CoordinateException("Invalid coordinates: lat=" +
                             std::to_string(lat) + ", lon=" +
                             std::to_string(lon)) {}
};

class OutOfBoundsException : public CoordinateException {
public:
    OutOfBoundsException(double lat, double lon, const std::string& bounds)
        : CoordinateException("Coordinates out of bounds: (" +
                             std::to_string(lat) + ", " +
                             std::to_string(lon) + ") beyond " + bounds) {}
};

// 3. Исключения для работы с данными
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

// 4. Исключения для пространственных операций
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

// 5. Исключения для анализа
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
}
}



#endif // GISEXCEPTIONS_H
