#ifndef GEOMETRYTYPECOMBOBOX_H
#define GEOMETRYTYPECOMBOBOX_H

#include <QComboBox>

namespace vrsa
{
namespace common
{
enum class GeometryType:int;
}
}
class GeometryTypeComboBox : public QComboBox
{
public:
    //фильтрация типов геометрии
    enum FilterFlag
    {
        Simple      = 0x01,  // только простые (point, line, polygon)
        Multi       = 0x02,  // только мульти (multiPoint, multiLine, multiPoly)
        Collection  = 0x04,  // коллекции (geometryCollection)
        All         = 0xFF   // все типы
    };
    //Q_PROPERTY(FilterFlags geometryTypes READ geometryTypes WRITE setGeometryTypes)
    Q_DECLARE_FLAGS(FilterFlags, FilterFlag)
    Q_FLAG(FilterFlags)

    struct TypeItem
    {
        vrsa::common::GeometryType type;
        QString name;
        QString icon;
    };
public:
    GeometryTypeComboBox(QWidget* parent = nullptr, FilterFlags flags = Simple);
    //FilterFlags geometryTypes() const noexcept { return mFlags; }
    void setGeometryTypes(FilterFlags flags);
private:
    void populateItems(FilterFlags flags);
private:
    //FilterFlag mFlags;
};

#endif // GEOMETRYTYPECOMBOBOX_H
