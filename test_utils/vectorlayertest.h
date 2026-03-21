#ifndef VECTORLAYERTEST_H
#define VECTORLAYERTEST_H
#include "test_utils/geometryfactory.h"
#include "common/logger.h"
#include <QString>
#include <QFile>
#include "common/GisDefines.h"
#ifdef VRSA_ENABLE_TEST_UTILS
//методы для создания различных векторных датасетов, слоев и объектов, используя исключительно GDAL api.
namespace vrsa
{
namespace test_utils
{



inline std::string data_folder;

struct TestLayerParamsStruct {
    std::string layerName;
    OGRwkbGeometryType geomType;
    int numFeatures;
    int numFields;
    std::string format;

    TestLayerParamsStruct(std::string name, OGRwkbGeometryType type, int features, int fields, std::string fmt)
            : layerName(name), geomType(type), numFeatures(features), numFields(fields), format(fmt) {}


        std::string toString() const {
            return layerName + " | " +
                   OGRGeometryTypeToName(geomType) + " | " +
                   std::to_string(numFeatures) + " объектов | " +
                   std::to_string(numFields) + " аттрибутивных полей | " +
                   format;
        }
};

//возможности формата
enum class DatasetCapability {
    SingleLayer,     // только один слой (Shapefile, GeoJSON, CSV, GPX, KML, DXF ...)
    MultiLayer       // много слоев (GPKG, PostGIS, SQLIte, Memory)
};

//определяем возможности формата по имени
inline DatasetCapability getFormatCapability(const std::string& format)
{
    //копия строки в нижнем регистре ("ESRI Shapefile" = "esri shapefile")
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    //ищем подстроку
    //форматы, поддерживающие 1 слой
    if (lowerFormat.find("shapefile") != std::string::npos ||
        lowerFormat.find("shp") != std::string::npos ||
        //lowerFormat.find("geojson") != std::string::npos ||
        lowerFormat.find("csv") != std::string::npos ||
        lowerFormat.find("gpx") != std::string::npos ||
        lowerFormat.find("kml") != std::string::npos ||
        lowerFormat.find("dxf") != std::string::npos )
        return DatasetCapability::SingleLayer;


    return DatasetCapability::MultiLayer;
}

// cоздание тестового датасета
inline GDALDataset* createTestDataset( const std::string& layerName, OGRwkbGeometryType geomType, int numFeatures,
                int numFields = 3, const std::string& format = "Memory", const std::string& outputPath = "")
{
    GDALAllRegister();

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver) {
        VRSA_DEBUG("TEST", "Driver not found: " + format);
        return nullptr;
    }

    //VRSA_ERROR("TEST", "createTestDataset called(). outputPath:" + outputPath);
    std::string dsPath = outputPath;
    DatasetCapability capability = getFormatCapability(format);

    GDALDataset* poDS;

    //меняем пути в зависимости от возможностьи хранения нескольких слоев
    if (capability == DatasetCapability::SingleLayer)
    {
        dsPath = outputPath.empty() ? common::TEST_VECTOR_DATA_FOLDER+"/test_dataset_" + layerName : outputPath;
        VRSA_DEBUG("TEST", "Creating Single Layer Dataset");
    }
    else
    {
        dsPath = outputPath.empty() ? common::TEST_VECTOR_DATA_FOLDER+"/test_multi_dataset_" : outputPath;
        VRSA_DEBUG("TEST", "Creating Multi Layer Dataset");
    }

    //note: GDT_unknown потому что работаем с векторным датасетом
    poDS = poDriver->Create(dsPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
    if (!poDS)
    {
        VRSA_ERROR("TEST", "Failed to create dataset at: " + dsPath);
        return nullptr;
    }

    // создаем слой с нужным типом геометрии
    OGRLayer* poLayer = poDS->CreateLayer(layerName.c_str(), nullptr, geomType, NULL);
    if (!poLayer)
    {
        //для некоторых форматов (GeoJSON) имя слоя может быть фиксированным
        poLayer = poDS->GetLayer(0);
        if (!poLayer)
        {
            VRSA_ERROR("TEST", "Failed to create or get layer in dataset at: " + dsPath);
            GDALClose(poDS);
            return nullptr;
        }
    }
    VRSA_DEBUG("TEST", "Layer successfuly created. Layer name:" + layerName + " stores in dataset at: " + dsPath);

    //создаем поля с учетом ограничения на длину имен (shapefile например не позволяет длинные имена)
    //TODO Сделать длину поля параметром в сигнатуре функции
    for (int i = 0; i < numFields; ++i)
    {
        std::string fieldName = "fld" + std::to_string(i); // Короткие имена
        OGRFieldDefn fieldDefn(fieldName.c_str(), OFTString);
        if (poLayer->CreateField(&fieldDefn) != OGRERR_NONE)
        {
            //VRSA_DEBUG("WARNING", "Failed to create field: " + fieldName);
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create field: " + fieldName);
        }
    }

    //чсловые поля
    OGRFieldDefn intField("int_fld", OFTInteger);
    poLayer->CreateField(&intField);

    OGRFieldDefn doubleField("dbl_fld", OFTReal);
    poLayer->CreateField(&doubleField);

    //генерируем векторные объекты слоев
    RandomGenerator gen;

    for (int i = 0; i < numFeatures; ++i)
    {
        OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
        OGRGeometry* geom = createRandomGeometry(geomType, gen);
        if (geom)
        {
            poFeature->SetGeometry(geom);
            OGRGeometryFactory::destroyGeometry(geom);
            //delete geom;
        }

        //устанавливаем атрибуты
        for (int j = 0; j < numFields; j++)
        {
            std::string fieldName = "fld" + std::to_string(j);
            std::string value = "val" + std::to_string(i) + "_" + std::to_string(j);
            poFeature->SetField(fieldName.c_str(), value.c_str());
        }

        poFeature->SetField("int_fld", i * 100);
        poFeature->SetField("dbl_fld", i * 3.14159);

        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create feature #" + std::to_string(i));


        OGRFeature::DestroyFeature(poFeature);
    }

    return poDS;
}

//создание тестового датасета с разными типами (только для многослойных форматов)
inline GDALDataset* createCompleteTestDataset(const std::string& format = "Memory", const std::string& outputPath = "")
{
    if (getFormatCapability(format) == DatasetCapability::SingleLayer) {
        VRSA_ERROR("TEST", "Format " + format + " does not support multiple layers. Use Memory, GPKG or PostgreSQL instead.");
        return nullptr;
    }

    GDALAllRegister();

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver)
    {
        VRSA_ERROR("TEST","Failed to create GDALDriver for: " + format );
        return nullptr;
    }


    RandomGenerator gen;

    std::vector<std::pair<std::string, OGRwkbGeometryType>> layerTypes = {
        {"points", wkbPoint},
        {"lines", wkbLineString},
        {"polygons", wkbPolygon}
    };
    auto dsPath = outputPath;
    dsPath = outputPath.empty() ? common::TEST_VECTOR_DATA_FOLDER+"/test_multilayer_dataset_" : outputPath;
    //добавляем мультигеометрии только для форматов, которые их поддерживают
    if (format != "CSV" && format !="XLSX")
    {
        layerTypes.push_back({"multipoints", wkbMultiPoint});
        layerTypes.push_back({"multilines", wkbMultiLineString});
        layerTypes.push_back({"multipolygons", wkbMultiPolygon});
        dsPath = outputPath.empty() ? common::TEST_VECTOR_DATA_FOLDER+"/test_multilayer_multigeometry_dataset_" : outputPath;
    }

    GDALDataset* poDS = poDriver->Create(dsPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
    if (!poDS)
    {
        VRSA_LOG_GDAL_ERROR("TEST", "Failed to create GDALDataset at: " + dsPath);
        return nullptr;
    }

    for (const auto& [name, type] : layerTypes)
    {
        OGRLayer* poLayer = poDS->CreateLayer(name.c_str(), nullptr, type, NULL);
        if (!poLayer)
        {
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create layer: " + name + "for dataset at:" + dsPath);
            continue;
        }

        //поля атрибутов
        OGRFieldDefn idField("id", OFTInteger);
        if (poLayer->CreateField(&idField) != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create field id for layer" + name + "in dataset at:" + dsPath);

        OGRFieldDefn nameField("name", OFTString);
        if (poLayer->CreateField(&nameField) != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create field name for layer" + name + "in dataset at:" + dsPath);

        OGRFieldDefn valField("val", OFTReal);
        if (poLayer->CreateField(&valField)  != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("TEST", "Failed to create field val for layer" + name + "in dataset at:" + dsPath);

        //создание векторных объектов
        int numFeatures = gen.randomCount() + 3;
        for (int i = 0; i < numFeatures; ++i)
        {
            OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
            if (!poFeature)
            {
                VRSA_LOG_GDAL_ERROR("TEST", "Failed to create feature #" + std::to_string(i) + " for layer: " + name);
                continue;
            }
            OGRGeometry* geom = createRandomGeometry(type, gen);
            if (geom)
            {
                if (poFeature->SetGeometry(geom) != OGRERR_NONE)
                    VRSA_LOG_GDAL_ERROR("TEST", "Failed to set geometry to feature #" + std::to_string(i)
                                        + " for layer: " + name);
                OGRGeometryFactory::destroyGeometry(geom);
                //delete geom;
            }
            else
                VRSA_LOG_GDAL_ERROR("TEST", "Failed to create geometry for layer: " + name);
            poFeature->SetField("id", i);
            poFeature->SetField("name", ("feat_" + std::to_string(i)).c_str());
            poFeature->SetField("val", i * 1.5);

            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
                VRSA_LOG_GDAL_ERROR("TEST", "Failed to create feature in layer: " + name);


            OGRFeature::DestroyFeature(poFeature);
        }
    }

    return poDS;
}

// универсальная функция сохранения с учетом формата
inline bool saveTestDataset(GDALDataset* poDS, const std::string& outputPath, const std::string& format = "ESRI Shapefile")
{
    if (!poDS) return false;

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver)
    {
        VRSA_LOG_GDAL_ERROR("TEST", "Unsupported format: " + format);
        return false;
    }

    //для однослойных форматов убеждаемся, что есть только один слой
    if (getFormatCapability(format) == DatasetCapability::SingleLayer)
    {
        if (poDS->GetLayerCount() > 1)
        {
            VRSA_DEBUG("TEST", "Dataset has multiple layers but format("+format+") supports only one. Saving first layer only.");

            //cоздаем новый датасет только с первым слоем
            GDALDataset* poSingleDS = poDriver->Create(outputPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if (!poSingleDS) return false;

            OGRLayer* poSrcLayer = poDS->GetLayer(0);
            if (!poSrcLayer)
            {
                GDALClose(poSingleDS);
                return false;
            }

            //копируем слой
            OGRLayer* poDstLayer = poSingleDS->CopyLayer(poSrcLayer, poSrcLayer->GetName());
            if (!poDstLayer)
            {
                GDALClose(poSingleDS);
                return false;
            }

            GDALClose(poSingleDS);
            return true;
        }
    }

    //для многослойных форматов или когда слой один
    GDALDataset* poCopy = poDriver->CreateCopy(outputPath.c_str(), poDS, FALSE, NULL, NULL, NULL);
    if (!poCopy) return false;

    GDALClose(poCopy);
    return true;
}

//примеры использования с разными форматами
//void runTests()
//{
//    // 1. Shapefile (только один слой, короткие имена полей)
//    auto* pointDS = createTestDataset("points", wkbPoint, 100, 2, "ESRI Shapefile");
//    if (pointDS) {
//        saveTestDataset(pointDS, "/tmp/test_points.shp", "ESRI Shapefile");
//        GDALClose(pointDS);
//    }

//    // 2. GeoJSON (тоже один слой)
//    auto* lineDS = createTestDataset("lines", wkbLineString, 50, 2, "GeoJSON");
//    if (lineDS) {
//        saveTestDataset(lineDS, "/tmp/test_lines.geojson", "GeoJSON");
//        GDALClose(lineDS);
//    }

//    // 3. GPKG (поддерживает много слоев)
//    auto* completeDS = createCompleteTestDataset("GPKG");
//    if (completeDS) {
//        saveTestDataset(completeDS, "/tmp/test.gpkg", "GPKG");
//        GDALClose(completeDS);
//    }

//    // 4. Memory (для тестов в памяти)
//    auto* multiDS = createTestDataset("multipolygons", wkbMultiPolygon, 20, 3, "Memory");
//    if (multiDS) {
//        // ...
//        GDALClose(multiDS);
//    }

//    // 5. PostgreSQL (много слоев)
//    auto* pgDS = createCompleteTestDataset("PostgreSQL");
//    if (pgDS) {
//        std::string pgConn = "PG:host=localhost dbname=test user=postgres password=1234";
//        saveTestDataset(pgDS, pgConn, "PostgreSQL");
//        GDALClose(pgDS);
//    }
//}


class TestLayerHelper {
public:
    TestLayerHelper(const QString& testDir) : m_testDir(testDir) {}

    //1. Создание слоев в формате Shapefile
    //точечный
    GDALDataset* createPointLayer(const QString& name, int numFeatures = 100)
    {
        //qDebug() << "DEBUG test layer helper: name ==" << name;
        QString path = getLayerPath(name, "shp");
        //qDebug() << "DEBUG test layer helper: path ==" << path;
        return createAndSaveLayer(name.toStdString(), wkbPoint, numFeatures, path);
    }

    //линейный
    GDALDataset* createLineLayer(const QString& name, int numFeatures = 50)
    {
        QString path = getLayerPath(name, "shp");
        return createAndSaveLayer(name.toStdString(), wkbLineString, numFeatures, path);
    }

    //полигональный
    GDALDataset* createPolygonLayer(const QString& name, int numFeatures = 25)
    {
        QString path = getLayerPath(name, "shp");
        return createAndSaveLayer(name.toStdString(), wkbPolygon, numFeatures, path);
    }

    //мультиточечный
    GDALDataset* createMultiPointLayer(const QString& name, int numFeatures = 20) {
        QString path = getLayerPath(name, "shp");
        return createAndSaveLayer(name.toStdString(), wkbMultiPoint, numFeatures, path);
    }

    //мультилинейный
    GDALDataset* createMultiLineLayer(const QString& name, int numFeatures = 15)
    {
        QString path = getLayerPath(name, "shp");
        return createAndSaveLayer(name.toStdString(), wkbMultiLineString, numFeatures, path);
    }

    //мультиполигональный
    GDALDataset* createMultiPolygonLayer(const QString& name, int numFeatures = 10)
    {
        QString path = getLayerPath(name, "shp");
        return createAndSaveLayer(name.toStdString(), wkbMultiPolygon, numFeatures, path);
    }

    //создание слоя в разных форматах
    GDALDataset* createLayerInFormat(const QString& name, OGRwkbGeometryType geomType, int numFeatures, const QString& format)
    {
        QString extension = formatToExtension(format);
        QString path = getLayerPath(name, extension);
        return createAndSaveLayer(name.toStdString(), geomType, numFeatures, path, format);
    }

    //получить путь к слою
    QString getLayerPath(const QString& name, const QString& extension) const
    {
        return m_testDir + "/" + name + "." + extension;
    }

    //проверить существование слоя
    bool layerExists(const QString& name, const QString& extension) const
    {
        return QFile::exists(getLayerPath(name, extension));
    }

    //удалить слой
    void removeLayer(const QString& name, const QString& extension)
    {
        QString path = getLayerPath(name, extension);
        QFile::remove(path);

        // для Shapefile удаляем все связанные файлы
        if (extension == "shp")
        {
            QFile::remove(path.replace(".shp", ".shx"));
            QFile::remove(path.replace(".shx", ".dbf"));
            QFile::remove(path.replace(".dbf", ".prj"));
        }
    }

private:
    QString m_testDir;

    //внутренний метод для создания и сохранения
    GDALDataset* createAndSaveLayer(const std::string& name, OGRwkbGeometryType geomType, int numFeatures,
                                    const QString& path, const QString& format = "ESRI Shapefile")
    {

        auto* ds = createTestDataset(name, geomType, numFeatures, 3, format.toStdString(), path.toStdString());
        if (!ds)
        {
            VRSA_ERROR("TEST", "TestHelper couldn't create testDataset: "+ name);
            return nullptr;
        }

        bool saved = saveTestDataset(ds, path.toStdString(), format.toStdString());
        if (!saved)
        {
            VRSA_ERROR("TEST", "TestHelper couldn't save testDataset: "+ name + "at " + path.toStdString());
            GDALClose(ds);
            return nullptr;
        }

        return ds;
    }
    //преобразования форматов к расширениям файлов
    QString formatToExtension(const QString& format) const
    {
        if (format == "ESRI Shapefile") return "shp";
        if (format == "GeoJSON") return "geojson";
        if (format == "GPKG") return "gpkg";
        return "dat";
    }
};


}
}

#endif //TEST_UTILS


#endif // VECTORLAYERTEST_H
