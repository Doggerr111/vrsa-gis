#include "vectorfeature.h"

vrsa::vector::VectorFeature::VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature)
    :mFeature{std::move(feature)}
{

}

void vrsa::vector::VectorFeature::setGeometry(gdalwrapper::OgrGeometryPtr ptr)
{
    //mGeometry = std::move(ptr);
//    emit geometryChanged();
//    emit featureChanged();
}

void vrsa::vector::VectorFeature::setName(const std::string& name) {
    if (mName != name) {
        mName = name;
        //emit featureChanged();
    }
}


void vrsa::vector::VectorFeature::setAttribute(const std::string& name, const AttributeValue& value) {
    mAttributes[name] = value;
    //emit attributesChanged();
    //emit featureChanged();
}

void vrsa::vector::VectorFeature::setVisible(bool visible) {
    if (mVisible != visible) {
        mVisible = visible;
        //emit visibilityChanged(visible);
        //emit featureChanged();
    }
}

vrsa::common::GeometryType vrsa::vector::VectorFeature::getType()
{
    return vrsa::gdalwrapper::GeometryTypeConverter::FromOGR(mFeature->GetGeometryRef()->getGeometryType());
}

OGRGeometry *vrsa::vector::VectorFeature::getOGRGeometry()
{
    return mFeature->GetGeometryRef();
}
