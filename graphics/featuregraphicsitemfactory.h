#ifndef FEATUREGRAPHICSITEMFACTORY_H
#define FEATUREGRAPHICSITEMFACTORY_H

#include "graphics/featuregraphicsitem.h"

namespace vrsa
{
namespace graphics
{

/**
 * @english
 * @brief Singleton factory for creating graphical representations of vector features
 *
 * Creates FeatureGraphicsItem objects and immediately emits a signal to add them
 * to the scene. This decouples item creation from scene management.
 *
 * Connect to featureGraphicsItemCreated signal in your controller to handle
 * new items (e.g., add them to MapScene).
 *
 * @russian
 * @brief Фабрика-синглтон для создания графических объектов векторных обьектов
 *
 * Создает FeatureGraphicsItem и немедленно испускает сигнал для их добавления
 * на сцену. Это разделяет создание объектов и управление сценой.
 *
 * Подключитесь к сигналу featureGraphicsItemCreated в контроллере для обработки
 * новых объектов (например, добавления на MapScene).
 */
class FeatureGraphicsItemFactory: public QObject
{
    Q_OBJECT
public:
    static FeatureGraphicsItemFactory& instance()
    {
        static FeatureGraphicsItemFactory factory;
        return factory;
    }
    /**
     * @english
     * @brief Checks if the featureGraphicsItemCreated signal has any connected slots
     * @return true if at least one slot is connected to the signal
     *
     * @russian
     * @brief Проверяет, есть ли подключенные слоты к сигналу featureGraphicsItemCreated
     * @return true если хотя бы один слот подключен к сигналу
     */
    inline bool hasConnection() const
    {
        return receivers(SIGNAL(featureGraphicsItemCreated(FeatureGraphicsItem*))) > 0;
    }

    /**
     * @english
     * @brief Creates a graphics item for a vector feature
     * @param feature Source vector feature
     * @param addToScene If true, emits signal for immediate addition to scene
     * @return Pointer to created FeatureGraphicsItem (ownership transferred to caller if addToScene=false)
     *
     * @russian
     * @brief Создает графический объект для векторной фичи
     * @param feature Исходная векторная фича
     * @param addToScene Если true, испускает сигнал для немедленного добавления на сцену
     * @return Указатель на созданный FeatureGraphicsItem (право владения передается вызывающему если addToScene=false)
     */
    FeatureGraphicsItem* createForFeature(vrsa::vector::VectorFeature* feature, bool addToScene = true)
    {
        FeatureGraphicsItem* item = new FeatureGraphicsItem(feature);
        if (!addToScene)
            return item;
        emit featureGraphicsItemCreated(item);
        return item;
    }

    //ectorLayer *parentLayer;
private:
     FeatureGraphicsItemFactory() = default;
     FeatureGraphicsItemFactory(const FeatureGraphicsItemFactory& other) = delete;
     FeatureGraphicsItemFactory& operator=(const FeatureGraphicsItemFactory& other) = delete;
signals:
     void featureGraphicsItemCreated(FeatureGraphicsItem* item);

};
}
}

#endif // FEATUREGRAPHICSITEMFACTORY_H
