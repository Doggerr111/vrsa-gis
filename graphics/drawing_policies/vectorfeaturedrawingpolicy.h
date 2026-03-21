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
    virtual void updateSymbol(const Symbol* newSymbol) = 0;
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
    void styleUpdated() const noexcept { mCache.invalidateScale(); };
    void geometryUpdated() const noexcept { mCache.invalidateGeometry(); };
    //virtual void cacheBoundingRect(DrawingContext& context) const = 0;
    //метод для изменения отрисовки при выборе графического объекта (вызывается из renderer, как правило)
    void setSelection(bool selected) { mIsSelected = selected; };
    virtual QPainterPath createPathFromOGRGeometry(OGRGeometry* geom) const{ return {}; };
    //virtual QPainterPath geometryToPath(const DrawingContext &context) const;
protected:
    virtual void cacheGeometry(const DrawingContext &context) const = 0; //вызывать при первой отрисовке и по событию от фичи! (ну то есть от рендерера)
    virtual void updateScaleDependentParams(double scale) const = 0;
    virtual void updateStrokePath() const = 0; //вызывать только в updateScaleCache!!


    class LODGeometry
    {
    public:
        struct Level
        {
            int zoomLevel;                    // уровень масштабирования
            double scaleDenominator;          // знаменатель масштаба (например, 1000000)
            gdalwrapper::OgrGeometryPtr geom; // упрощенная геометрия
            QPainterPath path;
            Level(Level&& other) noexcept
                : zoomLevel(other.zoomLevel)
                , scaleDenominator(other.scaleDenominator)
                , geom(std::move(other.geom))
                , path(std::move(other.path)) {}

            // Move assignment
            Level& operator=(Level&& other) noexcept
            {
                if (this != &other) {
                    zoomLevel = other.zoomLevel;
                    scaleDenominator = other.scaleDenominator;
                    geom = std::move(other.geom);
                    path = std::move(other.path);
                }
                return *this;
            }
            Level(const Level&) = delete;
            Level& operator=(const Level&) = delete;
            Level() = default;
        };


        void precompute(OGRGeometry* originalGeom);
        void addLevel(int zoom, double scale, gdalwrapper::OgrGeometryPtr geometry, QPainterPath path)
        {

            if (!geometry)
            {
                VRSA_DEBUG("RENDERING", "nullptr geometry input");
                return;
            }

            double originalArea = 0;
            double originalLength = 0;
//            auto string = dynamic_cast<OGRLineString*>(geometry.get());
//            qDebug()<<"ORIG LENGHT"<< dynamic_cast<OGRLineString*>(originalGeom)->get_Length();
//            qDebug()<<"SIMPLIFIED LENGHT"<< string->get_Length();
            Level level;
            level.zoomLevel = zoom;
            level.scaleDenominator = scale;
            level.geom = std::move(geometry);
            level.path = path;
            //qDebug()<<level.path;//все ок ...
            mLevels.emplace_back(std::move(level));

            // СОРТИРУЕМ ПО ВОЗРАСТАНИЮ (от крупного к мелкому) e.g. 1:1000 -> 1:2000 -> ... ->1:1000000000
            std::sort(mLevels.begin(), mLevels.end(),
                [](const auto& a, const auto& b) {
                    return a.scaleDenominator < b.scaleDenominator;
                });
        }

        //получить геометрию для нужного масштаба
        std::pair<OGRGeometry*, QPainterPath> getForScale(double scaleDenom)
        {
            //слишком крупный масштаб (маленький знаменатель)
            if (scaleDenom <= mLevels.front().scaleDenominator)
            {
                //возвращать оригинал!!!
               qDebug() << "Масштаб слишком большой!!, "
                           "Возвращаем наименее детализированный уровень" + QString::number(mLevels.front().zoomLevel);
               return std::make_pair(mLevels.front().geom.get(), mLevels.front().path);
            }
            //слишком мелкий масштаб (большой знаменатель) возвращаем "самую упрощенную" геометрию и путь
            if (scaleDenom >= mLevels.back().scaleDenominator)
            {
                return std::make_pair(mLevels.back().geom.get(), mLevels.back().path);
            }
            //ищем первый уровень, который МЕЛЬЧЕ или равен текущему масштабу
            for (const auto& level : mLevels)
            {
                //проходим от самого мелкого
                if (level.scaleDenominator >= scaleDenom)
                {
                    //qDebug()<<"возвращаем геометрию для уровня зума№:" + QString::number(level.zoomLevel);
                    return std::make_pair(level.geom.get(), level.path);
                }

            }
            qDebug() << "Масштаб слишком большой, возвращаем оригинальную геометрию";
            return std::make_pair(originalGeom, originalPath);
        }
    private:
        std::vector<Level> mLevels;
    public:
        OGRGeometry* originalGeom;
        QPainterPath originalPath;
    };

    mutable std::unique_ptr<LODGeometry> mLodCache;
    void precomputeLOD(OGRGeometry* geom) const;

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
        double offsetXScaled = 0.0;
        double offsetYScaled = 0.0;
        double sceneScale = -1;          ///< @english Cached scene scale @russian Кэшированный масштаб сцены
        bool isValid{false};             ///< @english Cache validity flag @russian Флаг валидности кэша
        bool isGeomValid{false};         ///< @english Geometry validity flag @russian Флаг валидности геометрии
        bool isBoundingRectValid{false}; ///< @english Bounding rect validity flag @russian Флаг валидности ограничивающего прямоугольника
        OGRGeometry* geom = nullptr;     ///< @english Original OGR geometry to detect changes  @russian Исходная OGR геометрия для обнаружения изменений
        bool isStrokedPathValid{false};
        double pointSize = -1;



        /**
         * @english @brief Invalidates all cached data
         * @russian @brief Сбрасывает все кэшированные данные
         */
        void invalidate() noexcept
        {
            isValid = false;
            isGeomValid = false;
            isBoundingRectValid = false;
            isStrokedPathValid = false;
            path = QPainterPath();
            boundingRect = QRectF();
            sceneScale = -1;
        }

        /**
         * @english @brief Invalidates only geometry data (Path and Geometry)
         * @russian @brief Сбрасывает только данные геометрии (Путь и геометрию)
         */
        void invalidateGeometry() noexcept
        {
            isGeomValid = false;
            isBoundingRectValid = false;
            path = QPainterPath();
            geom = nullptr;
            boundingRect = QRectF();
        }

        /**
         * @english @brief Invalidates only bounding rectangle (scale changed or pen width)
         * @russian @brief Сбрасывает только ограничивающий прямоугольник (при изменении масштаба или пера)
         */
        void invalidateBoundingRect() noexcept
        {
            isBoundingRectValid = false;
            boundingRect = QRectF();
        }

        void invalidateScale() noexcept
        {
            sceneScale = -1;
        }
        /**
         * @english @brief Checks if cache is valid for given scale
         * @param scale Scene scale to check
         * @return true if cache is valid and scale matches
         * @russian @brief Проверяет валидность кэша для указанного масштаба
         * @param scale Масштаб сцены для проверки
         * @return true если кэш валиден и масштаб совпадает
         */
        bool isValidForScale(double scale) const
        {
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
