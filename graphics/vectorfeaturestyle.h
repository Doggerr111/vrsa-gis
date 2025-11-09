#ifndef VECTORFEATURESTYLE_H
#define VECTORFEATURESTYLE_H
#include <QPen>
#include <QBrush>
#include <QRandomGenerator>
#include "common/GisDefines.h"
namespace vrsa
{
namespace graphics
{


class VectorFeatureStyle
{
public:
    VectorFeatureStyle();
    inline void setPen(QPen pen)
    {
        mPen = pen;
    }
    inline void setBrush(QBrush brush)
    {
        mBrush = brush;
    }

    inline QPen getPen() const
    {
        return mPen;
    }
    inline QBrush getBrush() const
    {
        return mBrush;
    }

    static VectorFeatureStyle createDefaultVectorStyle(vrsa::common::GeometryType type)
    {
        using namespace vrsa::common;
        QPen pen;
        VectorFeatureStyle st;
        switch (type)
        {
        case GeometryType::Point:
        {
            pen.setWidthF(0);
            QBrush brush;
            brush.setColor(QColor::fromRgb(QRandomGenerator::global()->bounded(0, 255),
                                           QRandomGenerator::global()->bounded(0, 255),
                                           QRandomGenerator::global()->bounded(0, 255)));
            brush.setStyle(Qt::SolidPattern);
            st.setBrush(brush);
            st.setPen(pen);
//            st.setPointSize(1);
//            st.setGeomType(type);
//            st.setPointType(Circle);
            return st;
        }
}
    }


private:
    QPen mPen;
    QBrush mBrush;
};

}
}

#endif // VECTORFEATURESTYLE_H
