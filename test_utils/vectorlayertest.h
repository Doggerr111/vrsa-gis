#ifndef VECTORLAYERTEST_H
#define VECTORLAYERTEST_H
#include "test_utils/geometryfactory.h"
#include "common/logger.h"

#ifdef VRSA_ENABLE_TEST_UTILS
namespace vrsa
{
namespace test_utils
{

// Определяем возможности формата
enum class DatasetCapability {
    SingleLayer,     // Только один слой (Shapefile, GeoJSON)
    MultiLayer       // Много слоев (GPKG, PostGIS, Memory)
};

// Определяем возможности формата по имени
DatasetCapability getFormatCapability(const std::string& format) {
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);

    if (lowerFormat.find("shapefile") != std::string::npos ||
        lowerFormat.find("geojson") != std::string::npos ||
        lowerFormat.find("csv") != std::string::npos) {
        return DatasetCapability::SingleLayer;
    }

    // Memory, GPKG, PostgreSQL поддерживают много слоев
    return DatasetCapability::MultiLayer;
}

// Создание тестового датасета (адаптировано под возможности формата)
GDALDataset* createTestDataset(
    const std::string& layerName,
    OGRwkbGeometryType geomType,
    int numFeatures,
    int numFields = 3,
    const std::string& format = "Memory",
    const std::string& outputPath = "")
{
    GDALAllRegister();

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver) {
        VRSA_DEBUG("ERROR", "Driver not found: " + format);
        return nullptr;
    }

    // Определяем путь для датасета
    std::string dsPath = outputPath;
    if (dsPath.empty()) {
        dsPath = "/vsimem/test_dataset_" + layerName;
    }

    // Проверяем возможности формата
    DatasetCapability capability = getFormatCapability(format);

    // Создаем датасет
    GDALDataset* poDS;
    if (capability == DatasetCapability::SingleLayer && !outputPath.empty()) {
        // Для однослойных форматов создаем напрямую
        poDS = poDriver->Create(dsPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
    } else {
        poDS = poDriver->Create(dsPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
    }

    if (!poDS) {
        VRSA_DEBUG("ERROR", "Failed to create dataset at: " + dsPath);
        return nullptr;
    }

    // Создаем слой
    OGRLayer* poLayer = poDS->CreateLayer(layerName.c_str(), nullptr, geomType, NULL);
    if (!poLayer) {
        // Для некоторых форматов (GeoJSON) имя слоя может быть фиксированным
        poLayer = poDS->GetLayer(0);
        if (!poLayer) {
            VRSA_DEBUG("ERROR", "Failed to create or get layer");
            GDALClose(poDS);
            return nullptr;
        }
    }

    // Создаем поля с учетом ограничения на длину имен
    for (int i = 0; i < numFields; i++) {
        std::string fieldName = "fld" + std::to_string(i); // Короткие имена
        OGRFieldDefn fieldDefn(fieldName.c_str(), OFTString);
        if (poLayer->CreateField(&fieldDefn) != OGRERR_NONE) {
            VRSA_DEBUG("WARNING", "Failed to create field: " + fieldName);
        }
    }

    // Числовые поля с короткими именами
    OGRFieldDefn intField("int_fld", OFTInteger);
    poLayer->CreateField(&intField);

    OGRFieldDefn doubleField("dbl_fld", OFTReal);
    poLayer->CreateField(&doubleField);

    // Генерируем фичи
    RandomGenerator gen;

    for (int i = 0; i < numFeatures; i++) {
        OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

        // Устанавливаем геометрию
        OGRGeometry* geom = createRandomGeometry(geomType, gen);
        if (geom) {
            poFeature->SetGeometry(geom);
            delete geom;
        }

        // Устанавливаем атрибуты
        for (int j = 0; j < numFields; j++) {
            std::string fieldName = "fld" + std::to_string(j);
            std::string value = "val" + std::to_string(i) + "_" + std::to_string(j);
            poFeature->SetField(fieldName.c_str(), value.c_str());
        }

        poFeature->SetField("int_fld", i * 100);
        poFeature->SetField("dbl_fld", i * 3.14159);

        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
            VRSA_DEBUG("WARNING", "Failed to create feature " + std::to_string(i));
        }

        OGRFeature::DestroyFeature(poFeature);
    }

    return poDS;
}

// Создание тестового датасета с разными типами (только для многослойных форматов)
GDALDataset* createCompleteTestDataset(const std::string& format = "Memory") {
    if (getFormatCapability(format) == DatasetCapability::SingleLayer) {
        VRSA_DEBUG("WARNING", "Format " + format + " does not support multiple layers. Use Memory, GPKG or PostgreSQL instead.");
        return nullptr;
    }

    GDALAllRegister();

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver) return nullptr;

    GDALDataset* poDS = poDriver->Create("/vsimem/complete_test", 0, 0, 0, GDT_Unknown, NULL);
    if (!poDS) return nullptr;

    RandomGenerator gen;

    std::vector<std::pair<std::string, OGRwkbGeometryType>> layerTypes = {
        {"points", wkbPoint},
        {"lines", wkbLineString},
        {"polygons", wkbPolygon}
    };

    // Добавляем мультигеометрии только для форматов, которые их поддерживают
    if (format != "GeoJSON") {
        layerTypes.push_back({"multipoints", wkbMultiPoint});
        layerTypes.push_back({"multilines", wkbMultiLineString});
        layerTypes.push_back({"multipolygons", wkbMultiPolygon});
    }

    for (const auto& [name, type] : layerTypes) {
        OGRLayer* poLayer = poDS->CreateLayer(name.c_str(), nullptr, type, NULL);
        if (!poLayer) {
            VRSA_DEBUG("WARNING", "Failed to create layer: " + name);
            continue;
        }

        // Короткие имена полей
        OGRFieldDefn idField("id", OFTInteger);
        poLayer->CreateField(&idField);

        OGRFieldDefn nameField("name", OFTString);
        poLayer->CreateField(&nameField);

        OGRFieldDefn valField("val", OFTReal);
        poLayer->CreateField(&valField);

        int numFeatures = gen.randomCount() + 3;

        for (int i = 0; i < numFeatures; i++) {
            OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

            OGRGeometry* geom = createRandomGeometry(type, gen);
            if (geom) {
                poFeature->SetGeometry(geom);
                delete geom;
            }

            poFeature->SetField("id", i);
            poFeature->SetField("name", ("feat_" + std::to_string(i)).c_str());
            poFeature->SetField("val", i * 1.5);

            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                VRSA_DEBUG("WARNING", "Failed to create feature in layer: " + name);
            }

            OGRFeature::DestroyFeature(poFeature);
        }
    }

    return poDS;
}

// Универсальная функция сохранения с учетом формата
bool saveTestDataset(GDALDataset* poDS, const std::string& outputPath, const std::string& format = "ESRI Shapefile") {
    if (!poDS) return false;

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver) return false;

    // Для однослойных форматов убеждаемся, что есть только один слой
    if (getFormatCapability(format) == DatasetCapability::SingleLayer) {
        if (poDS->GetLayerCount() > 1) {
            VRSA_DEBUG("WARNING", "Dataset has multiple layers but format supports only one. Saving first layer only.");

            // Создаем новый датасет только с первым слоем
            GDALDataset* poSingleDS = poDriver->Create(outputPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if (!poSingleDS) return false;

            OGRLayer* poSrcLayer = poDS->GetLayer(0);
            if (!poSrcLayer) {
                GDALClose(poSingleDS);
                return false;
            }

            // Копируем слой
            OGRLayer* poDstLayer = poSingleDS->CopyLayer(poSrcLayer, poSrcLayer->GetName());
            if (!poDstLayer) {
                GDALClose(poSingleDS);
                return false;
            }

            GDALClose(poSingleDS);
            return true;
        }
    }

    // Для многослойных форматов или когда слой один
    GDALDataset* poCopy = poDriver->CreateCopy(outputPath.c_str(), poDS, FALSE, NULL, NULL, NULL);
    if (!poCopy) return false;

    GDALClose(poCopy);
    return true;
}

// Примеры использования с разными форматами
void runTests() {
    // 1. Shapefile (только один слой, короткие имена полей)
    auto* pointDS = createTestDataset("points", wkbPoint, 100, 2, "ESRI Shapefile");
    if (pointDS) {
        saveTestDataset(pointDS, "/tmp/test_points.shp", "ESRI Shapefile");
        GDALClose(pointDS);
    }

    // 2. GeoJSON (тоже один слой)
    auto* lineDS = createTestDataset("lines", wkbLineString, 50, 2, "GeoJSON");
    if (lineDS) {
        saveTestDataset(lineDS, "/tmp/test_lines.geojson", "GeoJSON");
        GDALClose(lineDS);
    }

    // 3. GPKG (поддерживает много слоев)
    auto* completeDS = createCompleteTestDataset("GPKG");
    if (completeDS) {
        saveTestDataset(completeDS, "/tmp/test.gpkg", "GPKG");
        GDALClose(completeDS);
    }

    // 4. Memory (для тестов в памяти)
    auto* multiDS = createTestDataset("multipolygons", wkbMultiPolygon, 20, 3, "Memory");
    if (multiDS) {
        // Работаем с датасетом в памяти
        // ...
        GDALClose(multiDS);
    }

    // 5. PostgreSQL (много слоев)
    auto* pgDS = createCompleteTestDataset("PostgreSQL");
    if (pgDS) {
        std::string pgConn = "PG:host=localhost dbname=test user=postgres password=1234";
        saveTestDataset(pgDS, pgConn, "PostgreSQL");
        GDALClose(pgDS);
    }
}

}
}

#endif //test utils


#endif // VECTORLAYERTEST_H
