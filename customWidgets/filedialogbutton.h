#ifndef FILEDIALOGBUTTON_H
#define FILEDIALOGBUTTON_H
#include <QPushButton>

/**
 * @brief Кнопка для выбора файла с предустановленной ролью
 *
 * При нажатии открывает QFileDialog с настроенным фильтром и режимом,
 * после выбора файла испускает сигнал clicked(QString) с путём.
 *
 * @see ButtonRole
 */
class FileDialogButton: public QPushButton
{
public:
    enum class ButtonRole
    {
        NewLayer,          // создание нового слоя
        OpenDataset,       // открытие существующего датасета
        SaveDataset,       // сохранение датасета
        SaveRasterDataset, //сохранение растрового датасета
        ExportLayer,       // экспорт слоя
        ImportLayer        // импорт слоя
    };

    Q_OBJECT
public:
    FileDialogButton(QWidget* parent);
    void setRole(ButtonRole role) noexcept { mRole = role; };

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked(QString path);
private:
    ButtonRole mRole;
};

#endif // FILEDIALOGBUTTON_H
