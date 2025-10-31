#ifndef LIPVECTORLAYER_H
#define LIPVECTORLAYER_H
#include <QString>
#include <QObject>
#include "vector/vectorfeature.h"
//#include "gdal.h"
//#include "gdal_priv.h"
//#include "ogrsf_frmts.h"
//#include "liptypes.h"
//#include "lipattribute.h"
//#include "lipvectorstyle.h"
//#include "lipvectorreader.h"
//#include "lipcoordinatesystem.h"
//#include "lipwidgetmanager.h"
//#include "ogr_spatialref.h"

class LIPPointLayer;
class LIPLineLayer;
class LIPPolygonLayer;

class VectorLayer: public QObject
{
//    Q_OBJECT
//public:
//    //в качестве параметров конструктора используются объекты библиотечных классов GDAL и путь к файлу слоя
//    LIPVectorLayer(OGRLayer *l, QString path, GDALDataset *dataset);
//    // запрещаем копирование и перемещение
//    LIPVectorLayer(LIPVectorLayer&&) = delete;
//    LIPVectorLayer(const LIPVectorLayer&) = delete;
//    LIPVectorLayer& operator= ( const LIPVectorLayer & ) = delete;
//    LIPVectorLayer& operator= ( LIPVectorLayer && ) = delete;
//    /** возвращает путь к слою*/
//    QString returnFileName();
//    /** возвращает вектор с типами атрибутов слоя */
//    QVector<LIPAttributeType> getAttributeTypes();
//    /** возвращает вектор с названиями полей атрибутов слоя */
//    QVector<QString> getAttributeNames();
//    /** возвращает названия полей атрибутов в виде QStringList */
//    QStringList getAttributesNamesAsList();
//    /** возвращает охват слоя */
//    QRectF getBoundingBox();
//    /** возвращает систему координат слоя */
//    LIPCoordinateSystem *getCRS();
//    /** задает систему координат слоя */
//    void setCRS(LIPCoordinateSystem* crs);
//    /** статический метод для работы с атрибутами, используется при создании нового объекта слоя во время заполнения атрибутов*/
//    static QVector<LIPAttribute> stringValToAttrs(QVector<QString> names, QVector<QString> values, QVector<LIPAttributeType> types);
//    /** возвращает текущий стиль слоя */
//    LIPVectorStyle *getStyle();
//    /** возвращает указатель на объект OGRLayer, ассоциируемый со слоем */
//    OGRLayer *getOGRLayer();
//    /** возвращает указатель на объект GDALDataset, асоциируемый со слоем */
//    GDALDataset *getDataSet();
//    /** возвращает атрибуты в виде std::map */
//    std::map<int, QVector<LIPAttribute>> getAttributes();
//    bool setCoordinateSystem(LIPCoordinateSystem *targetCRS);
//    /** выбор объекта по индексу */
//    void setSelectedFeatureIndex(int index);
//    /** получение индекса выбранного объекта */
//    int getSelectedFeatureIndex();
//    /** отмечает слой как выбранный */
//    void setSelected(bool isSelected);
//    /** возвращает true если слой выбран */
//    bool isSelected();

//    /** в случае успешного приведения возвращает объект LIPPointLayer, в противном случае - nullptr */
//    LIPPointLayer* toPointLayer();
//    /** в случае успешного приведения возвращает объект LIPLineLayer, в противном случае - nullptr */
//    LIPLineLayer* toLineLayer();
//    /** в случае успешного приведения возвращает объект LIPPolygonLayer, в противном случае - nullptr */
//    LIPPolygonLayer* toPolygonLayer();




//public:
//    virtual QString returnGISName();
//    virtual ~LIPVectorLayer();
//    virtual void addFeature(QVector<QPointF> coords, QVector<LIPAttribute> attrs);
//    virtual void setVisible(bool=true);
//    virtual bool reproject(LIPCoordinateSystem *targetCRS);
//    virtual bool reproject(LIPCoordinateSystem *sourceCRS, LIPCoordinateSystem *targetCRS);
//    virtual bool reproject(LIPCoordinateSystem *targetCRS, QString fileName);
//    virtual void update();
//    virtual void setMapFeatures();
//    virtual void selectFeature(int index);
//    virtual void setZValue(int zValue);
//    virtual void deselectItems();


//signals:
//    //вызов сигнала приводит к перерисовке слоя
//    void needRepaint();

//public slots:
//    virtual void setSceneScaleFactor(double factor);
//    virtual void flyReprojection();

//protected:
//    OGRLayer *layer;
//    GDALDataset *dS;
//    QString fileName;
//    QVector<LIPAttributeType> attributeTypes;
//    QVector<QString> attributeNames;
//    double mScaleFactor;
//    LIPVectorStyle *mStyle;
//    LIPCoordinateSystem* mCRS;
//    int mZValue;
//    int mSelectedFeatureIndex;
//    bool mIsSelected;
};

#endif // LIPVECTORLAYER_H
