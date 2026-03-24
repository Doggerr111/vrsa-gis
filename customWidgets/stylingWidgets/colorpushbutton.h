#ifndef COLORPUSHBUTTON_H
#define COLORPUSHBUTTON_H
#include <QPushButton>

namespace vrsa {
namespace graphics {
class Symbol;
}
namespace common {
enum class StyleParametr: int;
}
}
class ColorPushButton : public QPushButton
{
    using Symbol = vrsa::graphics::Symbol;
    using StyleParam = vrsa::common::StyleParametr;
    Q_OBJECT
public:
    ColorPushButton(const Symbol *symbol, StyleParam param, QWidget* parent = nullptr);
    inline QColor getColor() const { return mColor; };

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent*) override;

private:
    QColor mColor;

};

#endif // COLORPUSHBUTTON_H
