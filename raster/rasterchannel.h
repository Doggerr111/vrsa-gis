#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include <gdal.h>
#include <gdal_priv.h>
#include <QGraphicsPixmapItem>

class RasterLayer: public QObject
{
    Q_OBJECT
public:
    RasterLayer(QString filename);
    /** скрытие/отображения слоя с карты*/
    void setVisible(bool fl);
    /** устанавливает порядок размещения слоя*/
    void setZValue(int value);
    /** считывает геопривязку растра и задает его для qimage*/
    bool readGeoTransform();
    /** применяет многоканальный цветной стиль отображения*/
    bool applyRGB();
    /** составляет цветное изображение*/
    bool composeRGBImage();
    /** задает стиль растра*/
    bool setRasterStyle(LIPRasterStyle *style);
    /** возвращает количество каналов растра*/
    int getBandCount();
    /** возвращает графический элемент, ассоциируемый с слоем*/
    QGraphicsPixmapItem *getPixmapItem();
    /** возвращает QPixmap, представляющий растр*/
    QPixmap getPixmap();
    /** объединяет каналы в QImage и возвращает его*/
    QImage mergeBands();
    /** возвращает охват слоя*/
    QRectF getBoundingBox();
    /** возвращает название слоя*/
    QString getGISName();
    /** возвращает путь к файлу, в котором хранится слой*/
    QString getFileName();
    /** возвращает GDALDataset, ассоциируемый с слоем*/
    GDALDataset* getDataSet();
    /** возвращает текущий стиль слоя*/
    LIPRasterStyle* getStyle();
public slots:
    /** обновление слоя*/
    void update();
private:
    QString mFileName;
    GDALDataset* mDs;
    QVector<GDALRasterBand*> mBands;
    QTransform mGeoTransform;
    QGraphicsPixmapItem *mPixmapItem;
    QImage mImage;
    QPixmap mPixmap;
    std::vector<ushort*> mRasterData;
    int mWidth;
    int mHeight;
    int mNumPixels;
    LIPRasterRGBStyle *mRGBStyle;
    LIPRasterStyle *mStyle;
    QString mGISName;
};

#endif // RASTERLAYER_H
