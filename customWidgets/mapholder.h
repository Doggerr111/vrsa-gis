#ifndef LIPMAPHOLDER_H
#define LIPMAPHOLDER_H
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>

/**
 * @english
 * @brief Widget for displaying digital map with pan and zoom capabilities
 *
 * MapHolder extends QGraphicsView to provide a digital map view with:
 * - Panning using snapshots (drag image)
 * - Zooming using snapshots (zoom preview)
 * - Mouse interaction for panning and zooming
 *
 * @russian
 * @brief Виджет для отображения цифровой карты с поддержкой панорамирования и масштабирования
 *
 * MapHolder расширяет QGraphicsView для отображения цифровой карты с возможностью:
 * - Панорамирования с использованием снимков (drag image)
 * - Масштабирования с использованием снимков (zoom preview)
 * - Взаимодействия мышью для перемещения и масштабирования
 */
class MapHolder : public QGraphicsView
{
    Q_OBJECT
public:
    MapHolder(QWidget *parent);
    /** @brief Центрирует видимую область по rect */
    void zoomToRect(const QRectF& rect);
    /** @brief Центрирует видимую область по point */
    void zoomToPoint(const QPointF& point);
    /** @brief Возвращает текущий картографический масштаб */
    int getMapScaleDenominator() const noexcept { return mCurrentScale; };
    /** @brief Возвращает scaleFactor виджета (View->transform()->m11()) */
    double getMapHolderScale() const;
    /**
     * @brief Возвращает текущий экстент карты в координатах сцены
     *
     * @note Возвращаемый прямоугольник определяется как:
     *       - left()   = левая координата (X минимум)
     *       - top()    = верхняя координата (Y максимум)   <- Верхний угол!
     *       - right()  = правая координата (X максимум)
     *       - bottom() = нижняя координата (Y минимум)
     *
     * @note height() возвращает ОТРИЦАТЕЛЬНОЕ значение, так как ось Y в координатах сцены
     *       направлена вниз, а в картографических координатах — вверх.
     *
     * @return QRectF Экстент карты с координатами left, top (максимальный Y),
     *         right, bottom (минимальный Y). height() будет отрицательным!
     */
    QRectF getExtent() const;
    /** @brief Возвращает прямоугольник в координатах сцены (как есть, с учетом трансформации) */
    QRectF getSceneRect() const;

public slots:
    /** @brief Блокирует/разрешает панорамирование левой кнопкой мыши */
    void onPanningRequested(bool enable) noexcept { mPanningEnabled = enable; };
    /** @brief Слот-сеттер для обработки сигнала о пересчете картографического масштаба с GISController */
    void onMapScaleChanged(double scale) noexcept { mCurrentScale = scale; };

signals:
    /** @brief Сигнал вызывается при изменении размеров виджета*/
    void resized();
    /** @brief Сигнал вызывается при изменением пользователем масштаба (при зумировании)*/
    void zoomed();
    /** @brief Сигнал вызывается при изменении видимого окна карты и передает видимый экстент
    в координатах карты и размеры виджета */
    void extentChanged(const QRectF& extent, const QRect& widgetRect);
    /** @brief Сигнал вызывается при клике мышкой, может использоваться для перехвата события быстрее виджета,
     *  например, чтобы заблокировать перемещение сцены */
    void mousePressed(QMouseEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void startZoomSnapshot(const QPointF& mousePos);
    void handleZoom(QWheelEvent* event);
    void finishZoom();
    void updateZoomPreview();

private:
    bool mIsDraging;
    bool mIsZooming;
    bool mPanningEnabled;
    double mZoomDelta;
    double mZoomStartScale;
    double mZoomInFactor;
    double mZoomOutFactor;
    int mCurrentScale;
    QPointF mZoomOffset;
    QPointF mZoomMousePos;
    QPoint mClickPos;
    QPoint mLastPos;
    QPixmap mDragPixmap;
    QPixmap mZoomPixmap;
    QImage mDragImage;
    std::unique_ptr<QTimer> mZoomTimer;
};

#endif // LIPMAPHOLDER_H
