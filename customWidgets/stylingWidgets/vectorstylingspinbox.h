#ifndef VECTORSTYLINGCOMBOBOX_H
#define VECTORSTYLINGCOMBOBOX_H
#include <QSpinBox>
#include "common/GisDefines.h"

namespace vrsa{
namespace graphics{
class Symbol;
}
}

class VectorStylingSpinBox: public QDoubleSpinBox
{
    Q_OBJECT
    using StyleParam = vrsa::common::StyleParametr;
    using Symbol = vrsa::graphics::Symbol;
public:
    explicit VectorStylingSpinBox(Symbol* s, StyleParam param, QWidget *parent = nullptr);

private:
    void setSpinBox();
    StyleParam mParam;
    Symbol* mSymbol;

};

#endif // VECTORSTYLINGCOMBOBOX_H
