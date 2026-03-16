#ifndef FEATUREDRAWINGPOLICY_H
#define FEATUREDRAWINGPOLICY_H
#include <QPainter>
#include <ogr_geometry.h>
#include <QStyleOptionGraphicsItem>
#include "common/GisDefines.h"
#include "graphics/drawingcontext.h"
#include "graphics/symbols/symbol.h"
#include "calculations/unitconverter.h"
#include <QPainterPath>
namespace vrsa
{
namespace graphics
{


/**
 * @english
 * @brief Base class for vector feature drawing policies
 *
 * Defines the interface for how vector features ((multi)points, (multi)lines, multi(polygons))
 * are rendered on the map. Each geometry type has its own policy
 * that handles:
 * - Bounding rectangle calculation
 * - Painter path creation
 * - Rendering logic
 *
 * Uses caching to optimize performance by storing precomputed paths
 * and bounding rectangles.
 *
 * @russian
 * @brief Базовый класс для политик отрисовки векторных объектов
 *
 * Определяет интерфейс для того, как векторные объекты ((мульти)точки, (мульти)линии, мульти(полигоны))
 * отображаются на карте. Каждый тип геометрии имеет свою политику,
 * которая отвечает за:
 * - Расчет ограничивающего прямоугольника
 * - Создание пути рисования
 * - Логику рендеринга
 *
 * Использует кэширование для оптимизации производительности путем
 * хранения предвычисленных путей и ограничивающих прямоугольников.
 */
class VectorFeatureDrawingPolicy
{
public:
    VectorFeatureDrawingPolicy();
    virtual vrsa::common::GeometryType getType() const = 0;
    virtual ~VectorFeatureDrawingPolicy() = default;
    /**
    * @english @brief Returns the bounding rectangle of the feature
    * @param context Drawing context with current settings
    * @return Bounding rectangle in scene coordinates
    * @russian @brief Возвращает ограничивающий прямоугольник объекта
    * @param context Контекст рисования с текущими настройками
    * @return Ограничивающий прямоугольник в координатах сцены
    */
    virtual QRectF boundingRect(const DrawingContext& context) const = 0;
    /**
     * @english @brief Renders the feature
     * @param context Drawing context with current settings
     * @russian @brief Отрисовывает объект
     * @param context Контекст рисования с текущими настройками
     */
    virtual void paint(const DrawingContext& context) = 0;
    void rebuildCache() const noexcept { mCache.invalidate(); }
    //virtual void cacheBoundingRect(DrawingContext& context) const = 0;
    //метод для изменения отрисовки при выборе графического объекта (вызывается из renderer, как правило)
    void setSelection(bool selected) { mIsSelected = selected; };
    //virtual QPainterPath geometryToPath(const DrawingContext &context) const;
protected:
    virtual void cacheGeometry(OGRGeometry *geom, double scale) const = 0; //вызывать при первой отрисовке и по событию от фичи! (ну то есть от рендерера)
    virtual void updateScaleDependentParams(double scale) const = 0;
    virtual void updateStrokePath() const = 0; //вызывать только в updateScaleCache!!
    /**
     * @english
     * @brief Cache data for rendering optimization
     *
     * Stores precomputed geometry and styling information to avoid
     * recalculations on every paint event.
     *
     * @russian
     * @brief Кэш-данные для оптимизации отрисовки
     *
     * Хранит предвычисленную геометрию и информацию о стилях
     * для избежания пересчетов при каждом обновлении.
     */
    struct CacheData
    {
        QPainterPath path{};             ///< @english Precomputed painter path @russian Предвычисленный путь
        QPainterPath strokePath{};       ///< @english Stroked painter path (for correct bounding rect computation) @russian "Обведенный" путь для корректного расчета ограничивающего прямоугольника, учитывая qpen style, width и т.д..
        QRectF boundingRect{};           ///< @english Cached bounding rectangle @russian Кэшированный ограничивающий прямоугольник
        QPen penScaled{};                ///< @english Cached scaled pen @russian Кэшированное отмасштабированное перо
        QBrush brush{};                  ///< @english Cached brush @russian Кэшированная кисть
        double offsetXScaled;
        double offsetYScaled;
        double sceneScale = -1;          ///< @english Cached scene scale @russian Кэшированный масштаб сцены
        bool isValid{false};             ///< @english Cache validity flag @russian Флаг валидности кэша
        bool isGeomValid{false};         ///< @english Geometry validity flag @russian Флаг валидности геометрии
        bool isBoundingRectValid{false}; ///< @english Bounding rect validity flag @russian Флаг валидности ограничивающего прямоугольника
        OGRGeometry* geom;               ///< @english Original OGR geometry to detect changes  @russian Исходная OGR геометрия для обнаружения изменений
        bool isStrokedPathValid{false};
        double pointSize = -1;



        /**
         * @english @brief Invalidates all cached data
         * @russian @brief Сбрасывает все кэшированные данные
         */
        void invalidate() {
            isValid = false;
            isGeomValid = false;
            isBoundingRectValid = false;
            path = QPainterPath();
            boundingRect = QRectF();
            sceneScale = -1;
        }

        /**
         * @english @brief Invalidates only geometry data (Path and Geometry)
         * @russian @brief Сбрасывает только данные геометрии (Путь и геометрию)
         */
        void invalidateGeometry() {
            isGeomValid = false;
            isBoundingRectValid = false;
            path = QPainterPath();
            boundingRect = QRectF();
        }

        /**
         * @english @brief Invalidates only bounding rectangle (scale changed or pen width)
         * @russian @brief Сбрасывает только ограничивающий прямоугольник (при изменении масштаба или пера)
         */
        void invalidateBoundingRect() {
            isBoundingRectValid = false;
            boundingRect = QRectF();
        }

        /**
         * @english @brief Checks if cache is valid for given scale
         * @param scale Scene scale to check
         * @return true if cache is valid and scale matches
         * @russian @brief Проверяет валидность кэша для указанного масштаба
         * @param scale Масштаб сцены для проверки
         * @return true если кэш валиден и масштаб совпадает
         */
        bool isValidForScale(double scale) const {
            return isValid && qFuzzyCompare(sceneScale, scale);
        }

        /**
         * @english @brief Updates geometry cache but sets isBouningRectValid to false
         * @param p New painter path
         * @param pn New pen
         * @param b New brush
         * @russian @brief Обновляет кэш геометрии но устанавливает флаг валидности кеша ограничивающего прямоугольник в false
         * @param p Новый путь
         * @param pn Новое перо
         * @param b Новая кисть
         */
        void update(const QPainterPath& p, const QPen& pn, const QBrush& b) {
            path = p;
            penScaled = pn;
            brush = b;
            isGeomValid = true;
            isValid = true;
            isBoundingRectValid = false;  // boundingRect нужно пересчитать
        }

        /**
         * @english @brief Sets cached bounding rectangle
         * @param rect New bounding rectangle
         * @russian @brief Устанавливает кэшированный ограничивающий прямоугольник
         * @param rect Новый ограничивающий прямоугольник
         */
        void setBoundingRect(const QRectF& rect) {
            boundingRect = rect;
            isBoundingRectValid = true;
        }
        bool isScaleValid(QStyleOptionGraphicsItem *option, QPainter* painter)
        {
            return sceneScale == option->levelOfDetailFromTransform(painter->worldTransform());
        }
        bool isScaleValid(double other)
        {
            return sceneScale == other;
        }
        bool isGeometryValid(OGRGeometry* other)
        {
            return geom == other;
        }
    };

    mutable CacheData mCache;
    mutable bool mIsSelected{false};
};
}
}
#endif // FEATUREDRAWINGPOLICY_H
