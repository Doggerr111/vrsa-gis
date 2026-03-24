#ifndef LIPCRSCOMBOBOX_H
#define LIPCRSCOMBOBOX_H
#include <QComboBox>

/**
 * @brief Комбобокс для выбора системы координат
 */
class CRSComboBox : public QComboBox
{
    Q_OBJECT
public:
    CRSComboBox(QWidget* parent=nullptr);
};

#endif // LIPCRSCOMBOBOX_H
