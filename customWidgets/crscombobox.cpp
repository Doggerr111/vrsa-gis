#include "crscombobox.h"
#include <QDebug>
#include "spatialref/spatialrefdatabase.h"

CRSComboBox::CRSComboBox(QWidget* parent)
    : QComboBox(parent)
{
    int index = 0;
    for (const auto& crsInfo: vrsa::spatialref::SpatialReferenceDatabase::instance().getPredefinedCRSs())
        addItem(QString::fromStdString(crsInfo.name), index++);
}

