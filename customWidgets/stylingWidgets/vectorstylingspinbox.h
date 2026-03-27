#ifndef VECTORSTYLINGCOMBOBOX_H
#define VECTORSTYLINGCOMBOBOX_H
#include <QSpinBox>

namespace vrsa{
namespace common{
enum class StyleParametr: int;
}
namespace graphics{
class Symbol;
}
}

/**
 * @brief Специализированный спин-бокс для настройки стилей векторных объектов
 */
class VectorStylingSpinBox: public QDoubleSpinBox
{
    Q_OBJECT
    using StyleParam = vrsa::common::StyleParametr;
    using Symbol = vrsa::graphics::Symbol;
public:
    explicit VectorStylingSpinBox(Symbol* s, StyleParam param, QWidget *parent = nullptr);

private:
    void updateFromSymbol  ();
    void setRangeForParam  ();
    void setStepForParam   ();
    void setValueFromSymbol();

    double getPointSize() const;
    double getPenWidth () const;
    double getXOffset  () const;
    double getYOffset  () const;
    double getRotation () const;
    double getOpacity  () const;
    StyleParam mParam;
    Symbol* mSymbol;

};

#endif // VECTORSTYLINGCOMBOBOX_H
