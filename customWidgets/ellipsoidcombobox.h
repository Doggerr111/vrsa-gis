#ifndef ELLIPSOIDCOMBOBOX_H
#define ELLIPSOIDCOMBOBOX_H
#include <QComboBox>

/**
 * @brief Комбобокс для выбора эллипсоида
 */
class EllipsoidComboBox : public QComboBox
{
    Q_OBJECT
public:
    EllipsoidComboBox(QWidget* parent = nullptr);
};

#endif // ELLIPSOIDCOMBOBOX_H
