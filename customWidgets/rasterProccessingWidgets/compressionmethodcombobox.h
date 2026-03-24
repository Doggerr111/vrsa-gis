#ifndef COMPRESSIONMETHODCOMBOBOX_H
#define COMPRESSIONMETHODCOMBOBOX_H
#include <QComboBox>

/**
 * @brief Комбобокс для выбора метода сжатия растров
 *
 * @see vrsa::common::GeoTiffCompression
 *
 */
class CompressionMethodComboBox : public QComboBox
{
public:
    CompressionMethodComboBox(QWidget* parent = nullptr);
};

#endif // COMPRESSIONMETHODCOMBOBOX_H
