#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include "vector/vectordataset.h"
#include "common/gisexceptions.h"
#include "common/logger.h"
#include "featuregraphicsitem.h"


namespace vrsa
{
namespace gdalwrapper
{
class Dataset;
}
namespace services
{

//реестр слоев
class ProjectManager : public QObject
{
    //using projectDatasets = std::vector<std::unique_ptr<vrsa::gdalwrapper::Dataset>>;
    using DatasetPtr = std::unique_ptr<gdalwrapper::Dataset>;
    Q_OBJECT
public:

//    static ProjectManager& instance() {
//        static ProjectManager s_instance;
//        return s_instance;
//    }
    void AddDataset(DatasetPtr dS);
    void removeDataset(gdalwrapper::Dataset* dS);
    std::vector<DatasetPtr>& getDatasets() noexcept;
    explicit ProjectManager(QObject *parent = nullptr);
    ~ProjectManager();
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
    vrsa::vector::VectorLayer*  getLayer(const std::string& src, int idx); //care!!
    //возращает слои, координаты которых заданы в метрах
    std::vector<vector::VectorLayer*> getProjectedVectorLayers();
    std::vector<std::string> getVectorLayersNames() const;
    vrsa::vector::VectorLayer*  getVectorLayerByName(const std::string& name); //care!!
    inline vector::VectorLayer* getActiveVectorLayer() const noexcept { return mActiveVectorLayer; }
    vrsa::vector::VectorDataset* getDatasetAssociatedWithVectorLayer(const vector::VectorLayer*)         const;
    vrsa::vector::VectorLayer* getLayerAssociatedWithFeature(const vrsa::vector::VectorFeature* feature) const;
    int getLayerID(const vector::VectorLayer*layer) const;

    std::vector<std::string> getRasterDatasetSources() const;
    std::vector<std::string> getVectorDatasetSources() const;

    void setActiveVectorLayer(const std::string& src, int idx);
    inline void setActiveVectorLayer(vector::VectorLayer* vL) noexcept { mActiveVectorLayer = vL; }

    void createPostGISConnection(const QString &connectionName, const std::string& connectionString);
    void createXYZConnection(const QString& connectionName, const std::string& xmlString);
    void addPostGISLayer(vector::VectorLayer* pgLayer);
public slots:
    void onVectorLayerReadingRequested(const std::string& src);
signals:
    void datasetAdded(gdalwrapper::Dataset* dS);
    void vectorLayerAdded(vector::VectorLayer* layer);
    void featureGraphicsItemCreated(graphics::FeatureGraphicsItem*);//todo delete
    void datasetsChanged();
    void postGisDatasetReady(gdalwrapper::Dataset* pgDs);

    void datasetAboutToBeRemoved(gdalwrapper::Dataset* dS);
private:
    // Удаляем конструкторы копирования и присваивания
    ProjectManager(const ProjectManager&) = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;

    std::unique_ptr<gdalwrapper::Dataset> readPostGISDataset(const std::string& connectionString);
    gdalwrapper::Dataset* readDataset(const std::string& src, unsigned int flags = GDAL_OF_ALL | GDAL_OF_UPDATE);
    gdalwrapper::Dataset* readTMSDataset(const std::string& xml, bool xyz = false,
                                         const std::string& connectoonName = "Unknown",
                                         unsigned int flags = GDAL_OF_RASTER | GDAL_OF_READONLY);
private:
    std::vector<DatasetPtr> mDatasets;
    vector::VectorLayer* mActiveVectorLayer;
};
}
}
#endif // PROJECTMANAGER_H
