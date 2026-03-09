#ifndef VectorLayer_H
#define VectorLayer_H
#include <QString>
#include <QObject>
#include <QRectF>
#include "vector/vectorfeature.h"
#include "gdal/geometrytypeconverter.h"
#include "graphics/vectorfeaturestyle.h"


namespace vrsa {
namespace vector {


class VectorLayer: public QObject
{
    Q_OBJECT
public:
    using featuresVec = std::vector<std::unique_ptr<vrsa::vector::VectorFeature>>;
    VectorLayer(OGRLayer *l);
    
    int id();
    void setFeatures(featuresVec features);
    inline std::size_t getFeaturesCount() const
    {
        return mFeatures.size();
    }
    common::GeometryType getGeomType() const;
    const char* getName() const;

    /** возвращает путь к слою*/
    QString returnFileName();
    OGRLayer* getOGRLayer();
    QRectF getBoundingBox();
    std::vector<VectorFeature*> getFeatures();
    std::vector<std::string> getAttributesNames() const;
    VectorFeature* getFeatureAt(size_t idx);
    void setVisible(bool visible);

    bool addFeature(std::unique_ptr<VectorFeature> feature);
    bool deleteFeature(GIntBig fid);
    bool deleteFeature(VectorFeature* feature);

    void setStyle(std::unique_ptr<graphics::VectorFeatureStyle> style, common::GeometryType geomType)
    {
        mStyles.emplace(geomType, std::move(style));
    }
    void setZValue(int zValue) noexcept
    {
        mZValue = zValue;
        emit ZValueChanged(mZValue);
    }
    graphics::VectorFeatureStyle* getStyle(common::GeometryType geomType) const noexcept
    {
        auto it = mStyles.find(geomType);
        if (it != mStyles.end()) {
            return it->second.get();
        }
        return nullptr;
    };



    std::vector<std::string> getFieldNames() const;
       vrsa::common::FieldType getFieldType(const std::string& fieldName) const;
       bool hasField(const std::string& fieldName) const;
       int getFieldCount() const;

       // НОВЫЕ МЕТОДЫ для работы с мультигеометриями
       bool hasMultiGeometry() const;
       std::unique_ptr<VectorLayer> createExplodedLayer(const std::string& newLayerName = "") const;

       // НОВЫЙ МЕТОД для экспорта в GDALDataset
       GDALDataset* exportToGDALDataset(const std::string& format = "ESRI Shapefile",
                                        const std::string& outputPath = "",
                                        bool explodeMultiGeometries = true) const;

   private:
       // Кэш для типов полей (для оптимизации)
       mutable std::unordered_map<std::string, vrsa::common::FieldType> mFieldTypesCache;
       mutable bool mFieldTypesCacheValid = false;

       void buildFieldTypesCache() const;

signals:
    void featureAdded(VectorFeature*);
    void featureRemoved(VectorFeature*);
    void ZValueChanged(int);

protected:
    OGRLayer *mOGRLayer;
    GDALDataset *dS;
    QString fileName;
    //QVector<LIPAttributeType> attributeTypes;
    QVector<QString> attributeNames;
    double mScaleFactor;
    int mZValue;
    int mSelectedFeatureIndex;
    bool mIsSelected;
    featuresVec mFeatures;
    vrsa::common::GeometryType geomType;
    std::unique_ptr<graphics::VectorFeatureStyle> mStyle;
    std::unordered_map<common::GeometryType, std::unique_ptr<graphics::VectorFeatureStyle>> mStyles;

};

}
}

#endif // VectorLayer_H
