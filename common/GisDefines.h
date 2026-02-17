#ifndef LIPTYPES_H
#define LIPTYPES_H
#include <QMetaEnum>

namespace vrsa
{
namespace common
{
/**
 * @file GisDefines.h
 *
 * @english
 * @brief Enumerations and macros for GIS system
 *
 * Defines core enumerations, constants and macros used throughout the VRSA GIS.
 * These definitions provide type-safe identifiers for geometry types,
 * coordinate systems, attribute types, and common operations.
 * @endenglish
 *
 * @russian
 * @brief Перечисления и макросы для ГИС системы
 *
 * Определяет основные перечисления, константы и макросы, используемые в VRSA GIS.
 * Эти определения предоставляют типобезопасные идентификаторы для типов геометрии,
 * систем координат, типов атрибутов и общих операций.
 * @endrussian
 */
enum class DatasetType {
    Vector,
    Raster,
    Mixed,
    Unknown,
    Empty
};

enum class CrsFormat {
    WKT,
    Proj,
    EPSG
};

enum PointSymbolType {
    Square,
    Circle,
    Triangle,
    Rhombus
};

enum class BandRenderType {
    MultiBandColor,
    SingleBandGray //TODO PseudoColor, unique values
};

enum class GeometryType {
    Unknown = 0,          // Неизвестный тип
    Point = 1,            // Точка
    LineString = 2,       // Линия
    Polygon = 3,          // Полигон
    MultiPoint = 4,       // Мультиточка
    MultiLineString = 5,  // Мультилиния
    MultiPolygon = 6,     // Мультиполигон
    GeometryCollection = 7, // Коллекция геометрий
    CircularString = 8,   // Круговая строка (CircularString)
    CompoundCurve = 9,    // Составная кривая
    CurvePolygon = 10,    // Криволинейный полигон
    MultiCurve = 11,      // Мультикривая
    MultiSurface = 12,    // Мультиповерхность
    Curve = 13,           // Кривая
    Surface = 14,         // Поверхность
    PolyhedralSurface = 15, // Полиэдральная поверхность
    TIN = 16,             // TIN (триангуляционная сетка)
    Triangle = 17         // Треугольник
};


enum class StyleUnit{
    Millimeters,
    Inches,
    Pixels
};

enum class AttributeType{
    INT32,
    String,
    Real
};

//Роли для QTreeWidgetItem/QTableWidgetItem
enum CustomRoles {
    ItemTypeRole = Qt::UserRole + 1,
    DatasetPathRole,
    DatasetTypeRole,
    LayerTypeRole,
    LayerIDRole, //layer index frm dataset
    FullMetadataRole,    // для сложных структур
    AttributeTableFeatureRole,
    AttributeTableFeatureNameRole
};


enum TreeItemType {
    ItemType_Unknown = 0,
    // Уровень 1
    ItemType_Project,
    ItemType_ConnectionsRoot,

    // Уровень 2 - группы
    ItemType_RasterGroup,
    ItemType_VectorGroup,
    ItemType_DatabaseGroup,
    ItemType_WMSGroup,

    // Уровень 3 - источники
    ItemType_RasterDataset,    // .tif, .jpg и т.д.
    ItemType_VectorDataset,    // .shp, .geojson
    ItemType_DatabaseDataset,  // PostgreSQL, SQLite
    ItemType_WMSService,       // WMS сервис

    // Уровень 4 - слои
    ItemType_RasterLayer,      // Канал/слой растра
    ItemType_VectorLayer,      // Векторный слой
    ItemType_QueryLayer,       // Слой-запрос

    // Уровень 5 - детали
    ItemType_Style,
    ItemType_Labels,
    ItemType_Attributes
};

enum class MapToolType
{
    PointDigitizingTool = 0,
    MultiPointDigitizingTool,
    LineStringDigitizingTool,
    MultiLineStringDigitizingTool,
    PolygonDigitizingTool,
    MultiPolygonDigitizingTool
};

enum class SymbolType
{
    SimpleSymbol = 0,
    SimplePointSymbol,
    SimpleLineSymbol,
    SimplePolygonSymbol,
    //containers
    LayerSymbol,
    LayerPointSymbol,
    LayerLineSymbol,
    LayerPolygonSymbol,
    Unknown
};

class Types
{
public:
    enum class Crs {};

    enum class Drivers{
        ESRIShapefile
    };

    Types();
};

}
}
Q_DECLARE_METATYPE(vrsa::common::CustomRoles);
Q_DECLARE_METATYPE(vrsa::common::GeometryType);
Q_DECLARE_METATYPE(vrsa::common::DatasetType);
Q_DECLARE_METATYPE(vrsa::common::TreeItemType);
#endif // LIPTYPES_H
