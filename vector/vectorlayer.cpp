#include "vectorlayer.h"

vrsa::vector::VectorLayer::VectorLayer(OGRLayer *l)
    :mOGRLayer{l}
{

}

OGRLayer *vrsa::vector::VectorLayer::getOGRLayer()
{
    return mOGRLayer;
}

QRectF vrsa::vector::VectorLayer::getBoundingBox()
{
    OGREnvelope envelope;
    auto er=mOGRLayer->GetExtent(&envelope, true);
    if (er!=OGRERR_NONE)
        return QRectF();
    qreal minX = envelope.MinX;
    qreal minY = envelope.MinY;
    qreal maxX = envelope.MaxX;
    qreal maxY = envelope.MaxY;
    return QRectF(QPointF(minX, minY), QSizeF(maxX - minX, maxY - minY));
}

std::vector<vrsa::vector::VectorFeature *> vrsa::vector::VectorLayer::getFeatures()
{
    std::vector<VectorFeature*> ftrPtrs;
    ftrPtrs.reserve(mFeatures.size());
    for (const auto& layer : mFeatures)
    {
        ftrPtrs.push_back(layer.get());
    }
    return ftrPtrs;
}

std::vector<std::string> vrsa::vector::VectorLayer::getAttributesNames() const
{
    OGRFeatureDefn* defn = mOGRLayer->GetLayerDefn();
    std::vector<std::string> names;
    names.reserve(defn->GetFieldCount());
    for (int i = 0; i < defn->GetFieldCount(); ++i)
    {
        names.push_back(defn->GetFieldDefn(i)->GetNameRef());
    }
    return names;

}

vrsa::vector::VectorFeature* vrsa::vector::VectorLayer::getFeatureAt(size_t idx)
{
    return mFeatures.at(idx).get();
}

void vrsa::vector::VectorLayer::setVisible(bool visible)
{
    for (auto& feat : mFeatures)
    {
        feat->setVisible(visible);
    }

}

int vrsa::vector::VectorLayer::id()
{
    return 2;
}

void vrsa::vector::VectorLayer::setFeatures(featuresVec features)
{
    mFeatures = std::move(features);
}

bool vrsa::vector::VectorLayer::addFeature(std::unique_ptr<VectorFeature> feature)
{
    auto ogrF = feature->getOGRFeature();
    if (!ogrF)
    {
        VRSA_ERROR("VectorLayer", "NUFeature added!");
        return false;
    }
    auto err = mOGRLayer->SetFeature(ogrF);
    if (err != OGRERR_NONE)
    {
        VRSA_DEBUG("VectorLayer", "Feature added! FID:" + std::to_string(ogrF->GetFID()));
        return true;
    }
    else
    {
        VRSA_ERROR("VectorLayer", "Can't add vector feature to layer, OgrError:" + std::to_string(err));
        return false;
    }
}

bool vrsa::vector::VectorLayer::deleteFeature(GIntBig fid)
{
    if (!mOGRLayer)
    {
        VRSA_ERROR("VectorLayer", "Can't delete feature:" + std::to_string(fid)+ " VectorLayer has nullptr OGRLayer");
        return false;
    }

    OGRErr err = mOGRLayer->DeleteFeature(fid);
    if (err != OGRERR_NONE)
    {
        VRSA_ERROR("VectorLayer", "Can't delete feature:" + std::to_string(fid)+ " OGR error:" + std::to_string(err));
        return false;
    }
    auto it = std::remove_if(mFeatures.begin(), mFeatures.end(),
                             [fid](const auto& f) {
        auto ogrF = f->getOGRFeature();
        if (ogrF)
            return ogrF->GetFID() == fid;
    });
    mFeatures.erase(it, mFeatures.end());
    mOGRLayer->SyncToDisk();
    return true;
}

bool vrsa::vector::VectorLayer::deleteFeature(VectorFeature *feature)
{
    if (!feature) return false;
    return deleteFeature(feature->getOGRFeature()->GetFID());
}



vrsa::common::GeometryType vrsa::vector::VectorLayer::getGeomType() const
{
    return gdalwrapper::GeometryTypeConverter::FromOGR(mOGRLayer->GetGeomType());
}

const char *vrsa::vector::VectorLayer::getName()
{
    return mOGRLayer->GetName();
}
