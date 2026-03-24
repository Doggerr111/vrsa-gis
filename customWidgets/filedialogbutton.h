#ifndef FILEDIALOGBUTTON_H
#define FILEDIALOGBUTTON_H
#include <QPushButton>


class FileDialogButton: public QPushButton
{
public:
    enum class ButtonRole
    {
        NewLayer,       // создание нового слоя
        OpenDataset,    // открытие существующего датасета
        SaveDataset,    // сохранение датасета
        SaveRasterDataset,
        ExportLayer,    // экспорт слоя
        ImportLayer    // импорт слоя
    };

    Q_OBJECT
public:
    FileDialogButton(QWidget* parent);
    void setRole(ButtonRole role) noexcept { mRole = role; };
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void clicked( QString);
private:
    ButtonRole mRole;
};

#endif // FILEDIALOGBUTTON_H
