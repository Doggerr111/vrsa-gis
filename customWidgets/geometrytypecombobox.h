#ifndef GEOMETRYTYPECOMBOBOX_H
#define GEOMETRYTYPECOMBOBOX_H
#include <QComboBox>

namespace vrsa {
namespace common {
enum class GeometryType:int;
}
}

/**
 * @brief Комбобокс для выбора типа геометрии с фильтрацией
 *
 * @note Фильтры можно комбинировать: Simple | Multi — покажет простые и мульти-типы
 *
 * @see FilterFlag
 * @see vrsa::common::GeometryType
 */
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
    Q_DECLARE_FLAGS(FilterFlags, FilterFlag)
    Q_FLAG(FilterFlags)

private:
    struct TypeItem
    {
        vrsa::common::GeometryType type;
        QString name;
        QString icon;
    };

public:
    GeometryTypeComboBox(QWidget* parent = nullptr, FilterFlags flags = Simple);
    void setGeometryTypes(FilterFlags flags);

private:
    void populateItems(FilterFlags flags);

};

#endif // GEOMETRYTYPECOMBOBOX_H
