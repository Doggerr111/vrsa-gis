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
    const char* getName();

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

    void setStyle(graphics::VectorFeatureStyle style);
    graphics::VectorFeatureStyle getStyle() const noexcept { return mStyle; };

signals:
    void featureAdded(VectorFeature*);
    void featureRemoved(VectorFeature*);

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
    graphics::VectorFeatureStyle mStyle;
};

}
}

#endif // VectorLayer_H
