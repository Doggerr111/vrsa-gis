#ifndef RESAMPLINGMETHODCOMBOBOX_H
#define RESAMPLINGMETHODCOMBOBOX_H
#include <QComboBox>

/**
 * @brief Комбобокс для выбора метода ресемплинга растров
 *
 * @see vrsa::common::ResamplingMethod
 *
 */
class ResamplingMethodComboBox : public QComboBox
{
public:
    ResamplingMethodComboBox(QWidget* parent = nullptr);
};

#endif // RESAMPLINGMETHODCOMBOBOX_H
