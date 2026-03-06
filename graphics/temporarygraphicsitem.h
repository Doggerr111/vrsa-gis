#ifndef TEMPORARYGRAPHICSITEM_H
#define TEMPORARYGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include "graphics/drawingcontext.h"
#include "graphics/featuregraphicsitemrenderer.h"
#include "geometry/geometry.h"
namespace vrsa
{
namespace graphics
{


/**
 * @english
 * @brief Temporary graphical item not associated with any vector layer or feature
 * @details Used for rubberband, previews, and other transient graphics during
 *          digitizing operations. These items exist only in the scene and are
 *          not backed by persistent data.
 * @endenglish
 * @russian
 * @brief Временный графический объект, не связанный с векторным слоем или объектом
 * @details Используется для резиновых нитей, предпросмотра и других временных
 *          элементов при оцифровке. Эти объекты существуют только на сцене и
 *          не имеют постоянного хранения в данных.
 * @endrussian
 */
class TemporaryGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum class TemporaryItemRole {
        Digitizing,
        Selection,
        Measurement,
        Preview,
        RubberBand
    };
using Renderer = std::unique_ptr<FeatureGraphicsItemRenderer>;
public:
    explicit TemporaryGraphicsItem(common::GeometryType geomType, TemporaryItemRole role = TemporaryItemRole::Digitizing);
    /**
     * @english
     * @brief Sets new geometry for temporary item
     * @details Updates FeatureGraphicsItemRenderer and triggers repaint via prepareGeometryChange()
     *          and update() calls.
     * @param geom New geometry data (point, line, polygon, etc.)
     * @endenglish
     * @russian
     * @brief Устанавливает новую геометрию для временного объекта
     * @details Обновляет FeatureGraphicsItemRenderer и вызывает перерисовку через
     *          prepareGeometryChange() и update().
     * @param geom Новая геометрия (точка, линия, полигон и т.д.)
     * @endrussian
     */
    void setGeometry(const geometry::Geometry& geom);
    void setScale(const double scale) noexcept
    {
        mScale = scale;
    }
private:
    void setupRenderer();
    VectorFeatureStyle* generateStyle();

private:
    Renderer mRenderer;
    TemporaryItemRole mRole;
    common::GeometryType mGeomType;
    std::unique_ptr<VectorFeatureStyle> mStyle;
    gdalwrapper::OgrGeometryPtr mGeom;
    double mScale;
signals:


    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
}
}
#endif // TEMPORARYGRAPHICSITEM_H
