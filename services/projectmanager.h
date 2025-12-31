#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include "vector/vectordataset.h"
#include "common/gisexceptions.h"
namespace vrsa
{
namespace services
{
class ProjectManager : public QObject
{
    //using projectDatasets = std::vector<std::unique_ptr<vrsa::gdalwrapper::Dataset>>;
    using DatasetPtr = std::unique_ptr<vrsa::gdalwrapper::Dataset>;
    Q_OBJECT
public:

    // Удаляем конструкторы копирования и присваивания
    ProjectManager(const ProjectManager&) = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;

    //синглтон  Майерса
    static ProjectManager& instance() {
        static ProjectManager s_instance;
        return s_instance;
    }

    void AddDataset(DatasetPtr dS);

    std::vector<DatasetPtr>& getDatasets() noexcept;

    /**
     * @english
     * @brief Finds and returns a dataset by its source path
     * @param src Source path of the dataset to find
     * @return Raw pointer to the found dataset or nullptr if not found
     * @note The returned pointer is a non-owning observing pointer.
     *       DO NOT delete this pointer - memory is managed by the internal container.
     *       The pointer becomes invalid if the dataset is removed from the ProjectManager.
     * @endenglish
     * @russian
     * @brief Находит и возвращает набор данных по его исходному пути
     * @param src Исходный путь набора данных для поиска
     * @return Сырой указатель на найденный набор данных или nullptr если не найден
     * @note Возвращаемый указатель является наблюдающим (non-owning).
     *       НЕ УДАЛЯЙТЕ этот указатель - память управляется внутренним контейнером.
     *       Указатель становится невалидным если набор данных удаляется из ProjectManager.
     * @endrussian
     */
    vrsa::gdalwrapper::Dataset* getDatasetBySource(std::string& src);

signals:
    /**
      * @brief Сигнал о добавлении нового датасета
      * @param dataset Указатель на добавленный датасет
      * @param source Путь к источнику датасета
      */
     void datasetAdded();

     /**
      * @brief Сигнал об изменении списка датасетов
      */
     void datasetsChanged();
private:
    explicit ProjectManager(QObject *parent = nullptr);
    std::vector<DatasetPtr> mDatasets;
};
}
}
#endif // PROJECTMANAGER_H
