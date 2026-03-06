#ifndef SYMBOLTYPECOMBOBOX_H
#define SYMBOLTYPECOMBOBOX_H

#include <QComboBox>
#include "common/GisDefines.h"

class SymbolTypeComboBox : public QComboBox
{
    using SymbolType = vrsa::common::SymbolType;
    using LayerGeometryType = vrsa::common::GeometryType;
public:
    SymbolTypeComboBox();
    inline void setLayerGeometryType (LayerGeometryType type) noexcept
    {
        mLayerGeomType = type;
    }
    void setItems()
    {
        switch (mLayerGeomType)
        {
        case LayerGeometryType::Point:
        case LayerGeometryType::MultiPoint:
            break;
        case LayerGeometryType::LineString:
        case LayerGeometryType::MultiLineString:
            break;
        case LayerGeometryType::Polygon:
        case LayerGeometryType::MultiPolygon:
            break;
        default:
            break;
        }
    }

private:
    LayerGeometryType mLayerGeomType;

};

#endif // SYMBOLTYPECOMBOBOX_H
