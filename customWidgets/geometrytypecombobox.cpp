#include "geometrytypecombobox.h"
#include "GisDefines.h"
GeometryTypeComboBox::GeometryTypeComboBox(QWidget *parent, FilterFlags flags)
    : QComboBox(parent)
{
    populateItems(flags);
}



void GeometryTypeComboBox::populateItems(FilterFlags flags)
{
    clear();
    using namespace vrsa;
    //cписок всех типов с их именами
    std::vector<TypeItem> allTypes = {
        {common::GeometryType::Point, tr("Точка"), ":/images/icons/geometry/icon_point_black.png"},
        {common::GeometryType::LineString, tr("Линия"), ":/images/icons/lineLayer.png"},
        {common::GeometryType::Polygon, tr("Полигон"), ":/images/icons/polygonLayer.png"},
        {common::GeometryType::MultiPoint, tr("Мультиточка"), ":/images/icons/geometry/icon_multi_points_gray.png"},
        {common::GeometryType::MultiLineString, tr("Мультилиния"), ":/images/icons/geometry/icon_multi_lines_black.png"},
        {common::GeometryType::MultiPolygon, tr("Мультиполигон"), ":/images/icons/geometry/icon_multi_polygon_black.png"},
        {common::GeometryType::GeometryCollection, tr("Коллекция"), ":/images/icons/geometry/shapes_icon_black.png"}
    };

    for (const auto& item : allTypes)
    {
        bool add = false;

        // Проверяем флаги
        if (flags & FilterFlag::Simple)
        {
            if (item.type == common::GeometryType::Point ||
                    item.type == common::GeometryType::LineString ||
                    item.type == common::GeometryType::Polygon)
            {
                add = true;
            }
        }

        if (flags & FilterFlag::Multi)
        {
            if (item.type == common::GeometryType::MultiPoint ||
                    item.type == common::GeometryType::MultiLineString ||
                    item.type == common::GeometryType::MultiPolygon)
            {
                add = true;
            }
        }

        if (flags & FilterFlag::Collection)
        {
            if (item.type == common::GeometryType::GeometryCollection)
            {
                add = true;
            }
        }

        if (add)
            addItem(QIcon(item.icon), item.name, static_cast<int>(item.type));
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GeometryTypeComboBox::FilterFlags)
