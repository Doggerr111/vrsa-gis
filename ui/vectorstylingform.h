#ifndef VECTORSTYLINGFORM_H
#define VECTORSTYLINGFORM_H

#include <QDialog>
#include <graphics/symbols/symbol.h>
namespace Ui {
class VectorStylingForm;
}

class VectorStylingForm : public QDialog
{
    Q_OBJECT

    using Symbol = vrsa::graphics::Symbol;
public:
    explicit VectorStylingForm(Symbol*, QWidget *parent = nullptr);
    ~VectorStylingForm();

private slots:
    void on_toolButtonAddSymbol_clicked();
    void on_toolButtonDeleteSymbol_clicked();
    void on_toolButtonCopySymbol_clicked();

private:
    Ui::VectorStylingForm *ui;
    Symbol* su;
};

#endif // VECTORSTYLINGFORM_H
