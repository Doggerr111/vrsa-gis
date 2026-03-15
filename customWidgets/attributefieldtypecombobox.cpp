#include "attributefieldtypecombobox.h"
#include "GisDefines.h"

AttributeFieldTypeComboBox::AttributeFieldTypeComboBox(QWidget *parent)
    : QComboBox(parent)
{
    using namespace vrsa::common;
    //при создании слоя, пока только эти поля
    addItem("Integer", static_cast<int>(FieldType::Integer));
    addItem("Real", static_cast<int>(FieldType::Real));
    addItem("String", static_cast<int>(FieldType::String));
    addItem("Boolean", static_cast<int>(FieldType::Boolean));
}
