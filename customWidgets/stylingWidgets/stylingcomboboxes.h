#ifndef PENTYPECOMBOBOX_H
#define PENTYPECOMBOBOX_H
#include <QComboBox>

namespace vrsa{
namespace graphics{
class Symbol;
}
namespace common{
enum class PointSymbolType: int;
}
}

/**
 * @brief Комбобокс для выбора типа пера (стиля линии)
 */
class PenTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenTypeComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::PenStyle getStyle() const;
    void setStyle(Qt::PenStyle style);
};

/**
 * @brief Комбобокс для выбора типа соединений линий
 */
class PenJoinComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenJoinComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::PenJoinStyle getStyle() const;
    void setStyle(Qt::PenJoinStyle style);
};

/**
 * @brief Комбобокс для выбора типа кисти (заливки)
 */
class BrushTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    BrushTypeComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::BrushStyle getStyle() const;
    void setStyle(Qt::BrushStyle style);
};

/**
 * @brief Комбобокс для выбора стиля окончания линии
 */
class PenCapComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenCapComboBox(const Symbol* symbol, QWidget* parent=nullptr);
    Qt::PenCapStyle getStyle() const;
    void setStyle(Qt::PenCapStyle style);
};

/**
 * @brief Комбобокс для выбора стиля условного знака точечного символа
 * @see vrsa::common::PointSymbolType
 */
class PointTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
    using PointType = vrsa::common::PointSymbolType;
public:
    PointTypeComboBox(const Symbol* symbol, QWidget* parent = nullptr);
    void setPointType(PointType type);
    PointType getPointType() const;
private:
    QIcon createIconForType(PointType type) const;

};


#endif // PENTYPECOMBOBOX_H
