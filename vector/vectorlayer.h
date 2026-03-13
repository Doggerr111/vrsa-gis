#ifndef VectorLayer_H
#define VectorLayer_H
#include <QString>
#include <QObject>
#include <QRectF>
#include "gdal/gdalresourcehandles.h"
#include "vector/vectorfeature.h"


namespace vrsa
{
namespace graphics
{
class VectorFeatureStyle;
}
namespace geometry
{
    class Geometry;
}
namespace common
{
enum class FieldType: int;
enum class GeometryType: int;
}
namespace vector
{

class VectorLayer: public QObject
{
    Q_OBJECT
public:
    using featuresVec = std::vector<std::unique_ptr<VectorFeature>>; //объекты слоя

    VectorLayer(OGRLayer* ogrLayer);
    VectorLayer(const VectorLayer&)            = delete;
    VectorLayer& operator=(const VectorLayer&) = delete;
    VectorLayer(VectorLayer&&)                 = delete;
    VectorLayer& operator=(VectorLayer&&)      = delete;
    ~VectorLayer();
    
    void setFeatures(featuresVec features);
    inline std::size_t getFeaturesCount() const noexcept { return mFeatures.size(); }

    const char * getName()         const { return mOGRLayer ? mOGRLayer->GetName() : "Unknown"; };
    std::string getNameAsString()  const { return std::string(getName()); };
    OGRLayer*   getOGRLayer()      const noexcept { return mOGRLayer; };
    QRectF      getBoundingBox()   const;
    OGREnvelope getOGRBoundingBox()const;
    /**
     * @english
     * @brief Returns raw pointers to all features stored in the layer
     * This method returns pointers to the original features managed by this layer.
     * No spatial or attribute filtering is applied - ALL features are returned.
     * @return Vector of raw pointers to features (do not delete!)
     * @russian
     * @brief Возвращает сырые указатели на все объекты, хранящиеся в слое
     * Метод возвращает указатели на оригинальные объекты, которыми управляет слой.
     * Пространственная или атрибутивная фильтрация НЕ применяется - возвращаются ВСЕ объекты.
     * @return Вектор сырых указателей на объекты (не удалять!)
     */
    std::vector<VectorFeature*> getFeatures();
    /**
     * @english
     * @brief Returns cloned VectorFeature objects respecting current filters
     *
     * If spatial or attribute filters are set on the OGR layer, returns only
     * features that pass these filters. Otherwise returns all features.
     *
     * @param reset If true, resets reading position to beginning
     * @return Vector of cloned VectorFeature objects (caller takes ownership)
     * @note Current filters (spatial/attribute) are NOT cleared after call
     *
     * @russian
     * @brief Возвращает клонированные VectorFeature объекты с учетом текущих фильтров
     *
     * Если на OGR слое установлены пространственный или атрибутивный фильтры,
     * возвращаются только фичи, проходящие эти фильтры. Иначе возвращаются все фичи.
     *
     * @param reset Если true, сбрасывает позицию чтения в начало
     * @return Вектор клонированных VectorFeature объектов (вызывающий код владеет)
     * @note Текущие фильтры (пространственный/атрибутивный) НЕ сбрасываются после вызова
     */
    std::vector<std::unique_ptr<VectorFeature>> getClonedFeatures(bool reset = true);
    /**
     * @english
     * @brief Returns cloned OGRFeature objects respecting current filters
     *
     * If spatial or attribute filters are set on the OGR layer, returns only
     * features that pass these filters. Otherwise returns all features.
     *
     * @param reset If true, resets reading position to beginning
     * @return Vector of smart pointers to cloned OGRFeature objects
     * @note Current filters (spatial/attribute) are NOT cleared after call
     *
     * @russian
     * @brief Возвращает клонированные OGRFeature объекты с учетом текущих фильтров
     *
     * Если на OGR слое установлены пространственный или атрибутивный фильтры,
     * возвращаются только фичи, проходящие эти фильтры. Иначе возвращаются все фичи.
     *
     * @param reset Если true, сбрасывает позицию чтения в начало
     * @return Вектор умных указателей на клонированные OGRFeature объекты
     * @note Текущие фильтры (пространственный/атрибутивный) НЕ сбрасываются после вызова
     */
    std::vector<vrsa::gdalwrapper::OgrFeaturePtr> getClonedOGRFeatures(bool reset = true);
    /**
     * @english
     * @brief Returns raw pointers to original VectorFeature objects respecting current filters
     *
     * If spatial or attribute filters are set on the OGR layer, returns only
     * features that pass these filters. Otherwise returns all features.
     *
     * @param reset If true, resets reading position to beginning
     * @return Vector of raw pointers to original features (do not delete!)
     * @note Current filters (spatial/attribute) are NOT cleared after call
     * @warning Pointers become invalid if layer is destroyed or features removed
     *
     * @russian
     * @brief Возвращает сырые указатели на оригинальные VectorFeature объекты с учетом текущих фильтров
     *
     * Если на OGR слое установлены пространственный или атрибутивный фильтры,
     * возвращаются только фичи, проходящие эти фильтры. Иначе возвращаются все фичи.
     *
     * @param reset Если true, сбрасывает позицию чтения в начало
     * @return Вектор сырых указателей на оригинальные фичи (не удалять!)
     * @note Текущие фильтры (пространственный/атрибутивный) НЕ сбрасываются после вызова
     * @warning Указатели становятся невалидными при удалении слоя или удалении фич
     */
    std::vector<VectorFeature*> getOriginalFeatures(bool reset = true);
    VectorFeature*       getFeatureAt(size_t idx)       noexcept;
    const VectorFeature* getFeatureAt(size_t idx) const noexcept;

    bool addFeature(std::unique_ptr<VectorFeature> feature);
    bool deleteFeature(int64_t fid);
    bool deleteFeature(VectorFeature* feature);

    common::GeometryType getGeomType()    const;
    OGRwkbGeometryType   getOGRGeomType() const;

    std::vector<std::string> getFieldNames()                     const;
    common::FieldType getFieldType(const std::string& fieldName) const;
    bool hasField(const std::string& fieldName)                  const;
    int getFieldCount()                                          const noexcept;


    void setStyle(std::unique_ptr<graphics::VectorFeatureStyle> style, common::GeometryType geomType);
    graphics::VectorFeatureStyle* getStyle() const noexcept;
    void setVisible(bool visible);
    void setZValue(int zValue) noexcept;
    int getZValue() const noexcept {return mZValue; };





    //===========================ФИЛЬТРАЦИЯ=====================================

    /**
     * @english @brief Returns filtered OGR features (copies) as smart pointers and clears filter
     * @param filter Spatial filter geometry (ownership not transferred)
     * @return Vector of OGR feature smart pointers
     * @note Caller takes ownership via OgrFeaturePtr
     *
     * @russian @brief Возвращает отфильтрованные OGR фичи (копии) в виде умных указателей и очищает фильтр
     * @param filter Пространственный фильтр (право владения не передается)
     * @return Вектор умных указателей на OGR фичи
     * @note Вызывающий код получает владение через OgrFeaturePtr
     */
    std::vector<vrsa::gdalwrapper::OgrFeaturePtr> getClonedFilteredOGRFeatures(OGRGeometry* filter);
    /**
     * @english @brief Returns filtered VectorFeatures (clones of matching features)
     * @param filter Spatial filter geometry
     * @return Vector of cloned VectorFeature smart pointers
     * @note Uses FID to match OGR features with internal VectorFeatures and clears filter
     *
     * @russian @brief Возвращает отфильтрованные VectorFeature (клоны совпадающих объектов) и очищает фильтр
     * @param filter Пространственный фильтр
     * @return Вектор умных указателей на клоны VectorFeature
     * @note Использует FID для сопоставления OGR фич с внутренними VectorFeature
     */
    std::vector<std::unique_ptr<VectorFeature>> getClonedFilteredFeatures(OGRGeometry* filter);
    std::vector<std::unique_ptr<VectorFeature>> getClonedFilteredFeatures(const geometry::Geometry& filter);

    std::vector<std::unique_ptr<VectorFeature>> getClonedAttributeFilteredFeatures(const std::string& attributeFilter);
    std::vector<gdalwrapper::OgrFeaturePtr> getClonedAttributeFilteredOGRFeatures(const std::string& attributeFilter);
    std::vector<vrsa::vector::VectorFeature*> getOriginalAttributeFilteredFeatures(const std::string& attributeFilter);



    bool hasSpatialFilter() const;
    bool hasAttributeFilter() const;
    bool hasFilters() const;

    bool setSpatialFilter(OGRGeometry* filter);
    bool setSpatialFilter(geometry::Geometry filter);
    bool setAttributeFilter(const std::string& filter);

    OGRGeometry*         getSpatialOGRFilter();
    geometry::Geometry   getSpatialFilter();
    std::string          getAttributeFilter(const std::string& filter);


    bool hasMultiGeometry() const;
    std::unique_ptr<VectorLayer> createExplodedLayer(const std::string& newLayerName = "") const;

    GDALDataset* exportToGDALDataset(const std::string& format = "ESRI Shapefile",
                                     const std::string& outputPath = "",
                                     bool explodeMultiGeometries = true) const;



private:
    void buildFieldTypesCache() const;
    void applyStyleToFeatures();
    void applyZValueToFeatures();

signals:
    void featureAdded(VectorFeature*);
    void featureRemoved(int64_t);
    void visibilityChanged(bool);
    void ZValueChanged(int);

    void styleChanged(graphics::VectorFeatureStyle* newStyle);

private:
    OGRLayer *mOGRLayer;
    GDALDataset *dS;
    QString fileName;
    QVector<QString> attributeNames;
    int mZValue;
    bool mIsSelected;
    featuresVec mFeatures;
    vrsa::common::GeometryType geomType;
    std::unique_ptr<graphics::VectorFeatureStyle> mStyle;
    std::unordered_map<common::GeometryType, std::unique_ptr<graphics::VectorFeatureStyle>> mStyles;
    mutable std::unordered_map<std::string, vrsa::common::FieldType> mFieldTypesCache;
    mutable bool mFieldTypesCacheValid = false;

};

}
}

#endif // VectorLayer_H
