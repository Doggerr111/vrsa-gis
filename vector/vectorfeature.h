#ifndef VECTORFEATURE_H
#define VECTORFEATURE_H
#include <QObject>
#include "gdal/gdalresourcehandles.h"
#include <unordered_map>
#include <variant>

namespace vrsa
{
namespace common
{
enum class GeometryType : int;
enum class FieldType : int;
}
namespace geometry
{
class Geometry;
}
namespace vector
{
class VectorLayer;

class VectorFeature :  public QObject
{
    Q_OBJECT

public:
    using AttributeValue = std::variant<
        std::string,
        int,
        double,
        bool,
        std::nullptr_t
    >;
    using AttributeMap = std::unordered_map<std::string, AttributeValue>;

public:
    VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature, OGRLayer* layer = nullptr);
    VectorFeature(const VectorFeature&) = delete;
    VectorFeature& operator=(const VectorFeature&) = delete;

    VectorFeature(VectorFeature&& other) noexcept
           : QObject()
           , mFeature(std::move(other.mFeature))
           , mParentLayer(std::exchange(other.mParentLayer, nullptr))
           , mAttributes(std::move(other.mAttributes))
           , mFieldTypes(std::move(other.mFieldTypes))
           , mIsVisible(other.mIsVisible)
           , mIsSelected(other.mIsSelected)
       {}

       VectorFeature& operator=(VectorFeature&& other) noexcept
       {
           if (this != &other)
           {
               mFeature = std::move(other.mFeature);
               mParentLayer = std::exchange(other.mParentLayer, nullptr);
               mAttributes = std::move(other.mAttributes);
               mFieldTypes = std::move(other.mFieldTypes);
               mIsVisible = other.mIsVisible;
               mIsSelected = other.mIsSelected;
           }
           return *this;
       }

    /**
     * @english
     * @brief Takes ownership of the geometry and sets it to feature
     * @return true if geometry was successfully set
     *
     * @russian
     * @brief Забирает владение геометрией и устанавливает ее для векторного объекта
     * @return true если геометрия успешно установлена
     */
    [[nodiscard]] bool setGeometry(vrsa::gdalwrapper::OgrGeometryPtr ptr);
    /**
     * @english
     * @brief Sets feature geometry (creates a copy)
     * @param ogrGeom Source geometry pointer (unchanged)
     * @return true if geometry was successfully set
     *
     * @note Creates a copy of the source geometry.
     *       Source geometry is not modified or deleted.
     *       Caller is responsible for original cleanup.
     *
     * @russian
     * @brief Устанавливает геометрию объекта (создает копию)
     * @param ogrGeom Указатель на исходную геометрию (не изменяется)
     * @return true если геометрия успешно установлена
     *
     * @note Создает копию исходной геометрии.
     *       Исходная геометрия не изменяется и не удаляется.
     *       Вызывающий код отвечает за удаление оригинала.
     */
    [[nodiscard]] bool setGeometry(OGRGeometry *ogrGeom);
    /**
     * @english
     * @brief Sets геометрию geometry to feature
     * @return true if geometry was successfully set
     *
     * @russian
     * @brief Устанавливает геометрию для векторного объекта
     * @return true если геометрия успешно установлена
     */
    [[nodiscard]] bool setGeometry(const geometry::Geometry& geometry);
    /**
     * @english @brief Returns raw OGR geometry (read-only)
     * @russian @brief Возвращает исходную OGR геометрию (только чтение)
     */
    OGRGeometry* getOGRGeometry() const;
    /**
     * @english @brief Creates a copy of OGR geometry
     * @return OgrGeometryPtr unique pointer with OGRGeometryFactory::DestroyGeometry() deleter
     * @russian @brief Создает копию OGR геометрии
     * @return OgrGeometryPtr уникальный указатель с OGRGeometryFactory::DestroyGeometry() удалителем
     */
    gdalwrapper::OgrGeometryPtr cloneOGRGeometry() const;
    /**
     * @english @brief Returns geometry converted to internal geometry format
     * @russian @brief Возвращает геометрию, преобразованную во внутренний формат геометрии
     */
    geometry::Geometry getInternalGeometry() const;

    [[deprecated("Use getGeometryType() instead")]]
    vrsa::common::GeometryType getType() const;
    /**
     * @english @brief Returns geometry type
     * @russian @brief Возвращает тип геометрии
     */
    vrsa::common::GeometryType getGeometryType() const;
    /**
     * @english @brief Returns OGR geometry type
     * @russian @brief Возвращает OGR тип геометрии
     */
    OGRwkbGeometryType getOGRGeometryType() const;
    /**
     * @english @brief Returns parent OGR layer (read-only, do not delete)
     * @russian @brief Возвращает слой, с которым ассоциирован векторный объект (только для чтения, не удалять указатель)
     */
    OGRLayer* getOGRLayer()     const noexcept { return mParentLayer; };
    /**
     * @english @brief Returns underlying OGRFeature (owned by this feature, do not delete)
     * @russian @brief Возвращает внутренний OGRFeature (принадлежит этой фиче, не удалять)
     */
    OGRFeature* getOGRFeature() const noexcept {return mFeature.get(); };
    /**
     * @english @brief Returns feature ID (FID)
     * @return FID or -1 if no feature
     * @russian @brief Возвращает идентификатор объекта (FID)
     * @return FID или -1 если объекта нет
     */
    int64_t getFID() const { return mFeature ? mFeature->GetFID() : -1; };
    /**
     * @english @brief Sets feature visibility and emits visibilityChanged signal
     * @param visible New visibility state
     *
     * @russian @brief Устанавливает видимость объекта и испускает сигнал visibilityChanged
     * @param visible Новое состояние видимости
     */
    void setVisible(bool visible);
    /**
     * @english @brief Sets feature selection state and emits selectionChanged signal
     * @param selected New selection state
     *
     * @russian @brief Устанавливает состояние выделения фичи и испускает сигнал selectionChanged
     * @param selected Новое состояние выделения
     */
    void setSelected(bool selected);

    /**
     * @english @brief Creates new empty feature for given layer
     * @param parentLayer Layer that will own this feature
     * @return New feature or nullptr on error
     *
     * @russian @brief Создает новый пустой векторный объект для указанного слоя
     * @param parentLayer Слой-владелец объекта
     * @return Новый векторный объект или nullptr при ошибке
     */

//=======================АТТРИБУТЫ==============================


    /**
     * @english @brief Sets attribute from QVariant (converts to appropriate type)
     * @param name Attribute name
     * @param value Attribute value as QVariant
     *
     * @russian @brief Устанавливает атрибут из QVariant (конвертирует в нужный тип)
     * @param name Имя атрибута
     * @param value Значение атрибута в виде QVariant
     */
    void setAttribute(const std::string& name, const QVariant& value);

    // Перегрузки для разных типов
    void setAttribute(const std::string& name, int value);
    void setAttribute(const std::string& name, double value);
    void setAttribute(const std::string& name, const std::string& value);
    void setAttribute(const std::string& name, bool value);
    void setAttribute(const std::string& name, const char* value);
    void setAttribute(const std::string& name, const AttributeValue& value);
    bool updateAttribute(const std::string& name, const AttributeValue& value);
    /**
     * @english @brief Checks if attribute exists
     * @param name Attribute name
     * @return true if attribute exists
     *
     * @russian @brief Проверяет существование атрибута
     * @param name Имя атрибута
     * @return true если атрибут существует
     */
    bool hasField(const std::string& name) const;

    /**
     * @english @brief Loads all attributes from OGR feature
     * @param feature Source OGR feature (must not be null)
     * @note Unsupported field types are converted to strings (Date, Time, Binary, etc.)
     *
     * @russian @brief Загружает все атрибуты из OGR feature
     * @param feature Исходный OGR feature (не должен быть null)
     * @note Неподдерживаемые типы полей конвертируются в строки (Date, Time, Binary...)
     */
    void loadFromOGRFeature(OGRFeature* feature);

    /**
     * @english @brief Returns all attribute names
     * @return Vector of attribute names
     *
     * @russian @brief Возвращает имена всех атрибутов
     * @return Вектор с именами атрибутов
     */
    std::vector<std::string> getFieldNames() const;
    /**
     * @english @brief Returns attribute field type
     * @return vrsa::common::FieldType
     *
     * @russian @brief Возвращает тип поля по названию
     * @return vrsa::common::FieldType
     */
    vrsa::common::FieldType getFieldType(const std::string& name) const;
    int getFieldCount() const;


    /**
     * @english @brief Returns raw attribute value
     * @param name Attribute name
     * @return Attribute value or nullptr if not found
     *
     * @russian @brief Возвращает сырое значение атрибута
     * @param name Имя атрибута
     * @return Значение атрибута или nullptr если не найден
     */
    AttributeValue getAttribute(const std::string& name) const;

    /**
     * @english @brief Gets attribute value (throws if not found)
     * @tparam T Expected value type
     * @param name Attribute name
     * @return Attribute value
     * @throws std::runtime_error if attribute doesn't exist
     *
     * @russian @brief Возвращает значение атрибута (бросает исключение если не найден)
     * @tparam T Ожидаемый тип значения
     * @param name Имя атрибута
     * @return Значение атрибута
     * @throws std::runtime_error если атрибут не существует
     */
    template<typename T>
    T getAttribute(const std::string& name) const;

    /**
     * @english @brief Gets attribute value with default fallback
     * @tparam T Expected value type
     * @param name Attribute name
     * @param defaultValue Value to return if attribute doesn't exist or type mismatch
     * @return Attribute value or defaultValue
     *
     * @russian @brief Возвращает значение атрибута со значением по умолчанию
     * @tparam T Ожидаемый тип значения
     * @param name Имя атрибута
     * @param defaultValue Значение по умолчанию (возвращается если атрибут отсутствует или тип не совпадает)
     * @return Значение атрибута или defaultValue
     */
    template<typename T>
    T getAttribute(const std::string& name, const T& defaultValue) const;

    /**
     * @english @brief Returns all attributes as strings
     * @return Map of attribute names to string values
     *
     * @russian @brief Возвращает все атрибуты в виде строк
     * @return Мапа соответствий "имя атрибута" -> "строковое значение"
     */
    std::unordered_map<std::string, std::string> getAttributesAsString() const;
    std::unordered_map<QString, QString> getAttributesAsQString() const;
    std::string getAttributeAsString(const std::string& name) const;
    QString getAttributeAsQString(const std::string& name) const;


    /**
     * @english @brief Returns attribute as QVariant
     * @param name Attribute name
     * @return Attribute value as QVariant or invalid QVariant if not found
     *
     * @russian @brief Возвращает атрибут в виде QVariant
     * @param name Имя атрибута
     * @return Значение атрибута в виде QVariant или невалидный QVariant если не найден
     */
    QVariant getAttributeAsQVariant(const std::string& name) const;

    // Проверить наличие
    bool hasAttribute(const std::string& name) const
    {
        return mAttributes.find(name) != mAttributes.end();
    }

    // Получить все имена
    [[deprecated("Use getFieldNames() instead")]]
    std::vector<std::string> getAttributeNames() const;

    static std::unique_ptr<vrsa::vector::VectorFeature> createFeature(VectorLayer *parentLayer);

signals:
    void visibilityChanged(bool);
    void selectionChanged(bool);
    void geometryChanged();
    void featureChanged();
    //void featureSelected();
    //void attributesChanged(const std::string& name, const AttributeValue& value)


private:
    /**
     * @english @brief Synchronizes attribute value with underlying OGR feature
     * @tparam T Value type
     * @param name Attribute name
     * @param value Attribute value
     * @note Internal method, called automatically from setAttribute() and constructor VectorFeature()
     *
     * @russian @brief Синхронизирует значение атрибута с внутренним OGR feature
     * @tparam T Тип значения
     * @param name Имя атрибута
     * @param value Значение атрибута
     * @note Внутренний метод, вызывается автоматически из setAttribute() и constructor VectorFeature()
     */
    template<typename T>
    void syncToOGRFeature(const std::string& name, const T& value);
public:
    // НОВЫЕ МЕТОДЫ для геометрии
    bool isMultiGeometry() const;
    std::vector<std::unique_ptr<VectorFeature>> explodeToSimpleFeatures() const;
    std::unique_ptr<VectorFeature> clone() const;



private:
    vrsa::gdalwrapper::OgrFeaturePtr mFeature;
    OGRLayer* mParentLayer;
    //поле для хранения информации о типах полей
    std::unordered_map<std::string, vrsa::common::FieldType> mFieldTypes;
    //сами атрибуты
    AttributeMap mAttributes;
    bool mIsVisible;
    bool mIsSelected;


};

template<typename T>
T VectorFeature::getAttribute(const std::string &name) const
{
    auto it = mAttributes.find(name);
    if (it == mAttributes.end()) {
        throw std::runtime_error("Attribute not found: " + name);
    }
    return std::get<T>(it->second);
}

template<typename T>
T VectorFeature::getAttribute(const std::string &name, const T &defaultValue) const
{
    auto it = mAttributes.find(name);
    if (it == mAttributes.end()) return defaultValue;

    if (!std::holds_alternative<T>(it->second)) return defaultValue;
    return std::get<T>(it->second);
}

template<typename T>
void VectorFeature::syncToOGRFeature(const std::string &name, const T &value)
{
    if (!mFeature) return;

    int fieldIndex = mFeature->GetFieldIndex(name.c_str());
    if (fieldIndex < 0) return;

    if constexpr (std::is_same_v<T, int>)
        mFeature->SetField(fieldIndex, value);
    else if constexpr (std::is_same_v<T, double>)
        mFeature->SetField(fieldIndex, value);
    else if constexpr (std::is_same_v<T, std::string>)
        mFeature->SetField(fieldIndex, value.c_str());
    else if constexpr (std::is_same_v<T, bool>)
        mFeature->SetField(fieldIndex, value ? 1 : 0);
    else if constexpr (std::is_same_v<T, const char*>)
        mFeature->SetField(fieldIndex, value);

}

}
}

#endif // VECTORFEATURE_H
