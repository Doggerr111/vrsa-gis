#include "ellipsoidcombobox.h"
#include "spatialref/spatialrefdatabase.h"

EllipsoidComboBox::EllipsoidComboBox(QWidget *parent)
    : QComboBox(parent)
{
    int index = 0;
    for (const auto& ellipsoid: vrsa::spatialref::SpatialReferenceDatabase::instance().getPredefinedEllipsoids())
        addItem(ellipsoid.getName(), index++);
}
