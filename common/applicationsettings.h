#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H
#include <QObject>
#include "GisDefines.h"
namespace vrsa
{
namespace common
{
/**
 * @brief Глобальные настройки приложения (синглтон)
 *
 * Содержит параметры для рендеринга, позже через него будут загружаться сохраненный настройки/файл проекта
 * Для доступа используйте ApplicationSettings::instance()
 */
class ApplicationSettings: public QObject
{
    Q_OBJECT

public:
    static ApplicationSettings& instance()
    {
        static ApplicationSettings instance;
        return instance;
    }

    // LOD
    bool isLodEnabled() const noexcept { return mIsLodEnabled; }
    void setLodEnabled(bool enabled);

    common::LodAlgorithmType getLodAlgorithm() const noexcept { return mLodAlgorithm; }
    void setLodAlgorithm(common::LodAlgorithmType algo) noexcept;


private:
    bool mIsLodEnabled = true;
    common::LodAlgorithmType mLodAlgorithm = common::LodAlgorithmType::DouglasPeucker;

private:
    ApplicationSettings();
    ApplicationSettings(const ApplicationSettings& other) = delete;
    ApplicationSettings& operator=(const ApplicationSettings& other) = delete;
signals:
    void lodSettingsChanged(bool lodEnabled, LodAlgorithmType type);
};

}
}
#endif // APPLICATIONSETTINGS_H
