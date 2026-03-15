#ifndef LAYERCREATIONPARAMS_H
#define LAYERCREATIONPARAMS_H
#include <memory>
#include <string>
#include <vector>
#include <QMetaType>

namespace vrsa
{
namespace common
{

//cтруктура для одного поля (атрибута)
struct FieldDefinition {
    std::string name;
    std::string type;
    int typeIndex; //для внутреннего енама

    FieldDefinition() = default;

    FieldDefinition(const std::string &n, const std::string &t, const std::string &def = std::string())
        : name(n), type(t) {}
};

/**
 * @english
 * @brief Data Transfer Object (DTO) for vector layer definition
 * Used to pass layer creation data between UI and controller.
 * @russian
 * @brief DTO (Data Transfer Object) для определения векторного слоя
 *
 * Используется для передачи данных о создании слоя между UI и контроллером.
 */
struct LayerDefinition {
    std::string  format;
    std::string  filePath;
    int geometryType; //static_cast<int>(common::GeometryType)
    std::string  crs; //name e.g. WGS 84
    std::vector<FieldDefinition> fields;
    std::unordered_map<std::string, std::string> gdalOptions;

    bool isValid() const { return !filePath.empty() && (geometryType > 0 || geometryType <8); }
    int fieldCount() const { return fields.size(); }
};
}
}
//для передачи через сигнал-слот
Q_DECLARE_METATYPE(vrsa::common::FieldDefinition)
Q_DECLARE_METATYPE(vrsa::common::LayerDefinition)
#endif // LAYERCREATIONPARAMS_H
