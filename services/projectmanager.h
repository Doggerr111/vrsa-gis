#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include "vector/vectordataset.h"
#include "common/gisexceptions.h"
#include "common/logger.h"
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
    vrsa::gdalwrapper::Dataset* getDatasetBySource(const std::string str);

    /**
     * @english
     * @brief Retrieves the names of all layers from a dataset located at the specified path
     * @param src Path to the dataset file or data source (e.g., Shapefile, GeoPackage, GeoJSON)
     * @return Vector of strings containing names of all layers found in the dataset
     * @note This function opens the dataset in read-only mode using GDAL/OGR.
     *       The dataset is automatically closed after retrieving layer names.
     *       For multi-layer formats (GeoPackage, FileGDB), all available layers are returned.
     *       For single-layer formats (Shapefile), a vector with one element is returned.
     *       An empty vector is returned if the dataset cannot be opened or contains no layers.
     * @endenglish
     * @russian
     * @brief Возвращает имена всех слоев из набора данных, расположенного по указанному пути
     * @param src Путь к файлу набора данных или источнику данных (например, Shapefile, GeoPackage, GeoJSON)
     * @return Вектор строк, содержащий имена всех слоев, найденных в наборе данных
     * @note Функция открывает набор данных в режиме только для чтения с использованием GDAL/OGR.
     *       Набор данных автоматически закрывается после получения имен слоев.
     *       Для многослойных форматов (GeoPackage, FileGDB) возвращаются все доступные слои.
     *       Для однослойных форматов (Shapefile) возвращается вектор с одним элементом.
     *       Пустой вектор возвращается, если набор данных не может быть открыт или не содержит слоев.
     * @endrussian
     */
    std::vector<std::string> getLayerNames(const std::string& src);

    vrsa::vector::VectorLayer*  getLayer(const std::string src, int idx);

    void setActiveVectorLayer(const std::string& src, int idx);

    inline void setActiveVectorLayer(vector::VectorLayer* vL) noexcept
    {
        mActiveVectorLayer = vL;
    }

    inline vector::VectorLayer* getActiveVectorLayer() const noexcept
    {
        return mActiveVectorLayer;
    }

signals:
    /**
      * @brief Сигнал о добавлении нового датасета
      */
     void datasetAdded();

     /**
      * @brief Сигнал об изменении списка датасетов
      */
     void datasetsChanged();
private:
    explicit ProjectManager(QObject *parent = nullptr);
    std::vector<DatasetPtr> mDatasets;
    vector::VectorLayer* mActiveVectorLayer;
};
}
}
#endif // PROJECTMANAGER_H
