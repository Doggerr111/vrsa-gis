#ifndef ATTRIBUTEFIELDTYPECOMBOBOX_H
#define ATTRIBUTEFIELDTYPECOMBOBOX_H
#include <QComboBox>

/**
 * @brief Комбобокс для настройки полей атрибутов при создании векторного слоя
 *
 * @see vrsa::common::FieldType
 *
 */
class AttributeFieldTypeComboBox: public QComboBox
{
    Q_OBJECT
public:
    AttributeFieldTypeComboBox(QWidget* parent = nullptr);
};

#endif // ATTRIBUTEFIELDTYPECOMBOBOX_H
