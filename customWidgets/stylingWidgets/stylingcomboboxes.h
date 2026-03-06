#ifndef LIPPENTYPECOMBOBOX_H
#define LIPPENTYPECOMBOBOX_H

#include <QComboBox>
namespace vrsa{
namespace graphics
{
    class Symbol;
}
}

namespace vrsa{
namespace common
{
    enum class PointSymbolType : int;
}
}



class PenTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenTypeComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::PenStyle getStyle();
    void setStyle(Qt::PenStyle style);
};

class PenJoinComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenJoinComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::PenJoinStyle getStyle();
    void setStyle(Qt::PenJoinStyle style);
};

class BrushTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    BrushTypeComboBox(const Symbol *symbol, QWidget* parent=nullptr);
    Qt::BrushStyle getStyle();
    void setStyle(Qt::BrushStyle style);
};

class PenCapComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
public:
    PenCapComboBox(const Symbol* symbol, QWidget* parent=nullptr);
    Qt::PenCapStyle getStyle();
    void setStyle(Qt::PenCapStyle style);
};


class PointTypeComboBox : public QComboBox
{
    Q_OBJECT
    using Symbol = vrsa::graphics::Symbol;
    using PointType = vrsa::common::PointSymbolType;
public:
    PointTypeComboBox(const Symbol* symbol, QWidget* parent = nullptr);
    void setPointType(PointType type);
    PointType getPointType();
    //PointType getPointType();
private:
    QIcon createIconForType(PointType type);

};



#endif // LIPPENTYPECOMBOBOX_H
