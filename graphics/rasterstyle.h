#ifndef RASTERSTYLE_H
#define RASTERSTYLE_H
#include "raster/rasterdataset.h"


namespace vrsa
{
namespace graphics
{


enum class StretchType {
    Linear,              // Линейная
    Logarithmic,         // Логарифмическая
    HistogramEqualization, // Выравнивание гистограммы
    PercentClip,         // Отсечение по процентам
    Manual,              // Ручные min/max
    StandardDeviation,   // По стандартному отклонению
    MinMax               // Использовать мин/макс данных
};


struct ChannelStretchParams {
    StretchType type = StretchType::MinMax;
    double minValue = 0.0;
    double maxValue = 1.0;
    double gamma = 1.0;
    bool useDataMinMax = true;
    std::pair<double, double> percentClip = {2.0, 98.0};

    // Статистика
    double dataMin = 0.0;
    double dataMax = 1.0;
    double dataMean = 0.5;
    double dataStdDev = 0.1;
    double nodataValue = -9999.0;
    bool hasNodata = false;

    // Методы
    ChannelStretchParams() = default;

    // Вычисление значения для отображения (0.0 до 1.0)
    double calculateDisplayValue(double rawValue) const {
        // Проверка на NoData
        if (hasNodata && std::abs(rawValue - nodataValue) < 1e-10) {
            return -1.0; // Специальное значение для NoData
        }

        // Приведение к диапазону
        double normalized = 0.0;

        switch (type) {
            case StretchType::Linear:
            case StretchType::Manual:
            case StretchType::MinMax:
            case StretchType::PercentClip:
            case StretchType::StandardDeviation:
                normalized = (rawValue - minValue) / (maxValue - minValue);
                break;

            case StretchType::Logarithmic:
                if (rawValue <= 0) normalized = 0.0;
                else normalized = std::log1p(rawValue - minValue) / std::log1p(maxValue - minValue);
                break;

            case StretchType::HistogramEqualization:
                // Здесь нужна гистограмма для точного расчета
                normalized = (rawValue - minValue) / (maxValue - minValue);
                break;
        }

        // Применение гамма-коррекции
        if (gamma != 1.0 && normalized >= 0.0) {
            normalized = std::pow(normalized, 1.0 / gamma);
        }

        // Обрезка значений
        return std::clamp(normalized, 0.0, 1.0);
    }

    // Автонастройка на основе статистики
    void autoAdjust() {
        switch (type) {
            case StretchType::MinMax:
                minValue = dataMin;
                maxValue = dataMax;
                break;

            case StretchType::PercentClip:
                // Здесь нужен расчет процентилей из гистограммы
                // Временно используем линейную аппроксимацию
                minValue = dataMin + (dataMax - dataMin) * (percentClip.first / 100.0);
                maxValue = dataMin + (dataMax - dataMin) * (percentClip.second / 100.0);
                break;

            case StretchType::StandardDeviation:
                minValue = dataMean - 2.0 * dataStdDev;
                maxValue = dataMean + 2.0 * dataStdDev;
                break;

            default:
                // Для других типов оставляем текущие значения
                break;
        }
    }

    // Установка статистики из данных
    void setStatistics(double min, double max, double mean = 0.5, double stddev = 0.1) {
        dataMin = min;
        dataMax = max;
        dataMean = mean;
        dataStdDev = stddev;

        if (useDataMinMax) {
            minValue = min;
            maxValue = max;
        }
    }

    // Проверка валидности
    bool isValid() const {
        return maxValue > minValue && gamma > 0;
    }
};

class RasterStyle
{
public:
    RasterStyle(std::unique_ptr<raster::RasterDataset>& rDs, common::BandRenderType = common::BandRenderType::MultiBandColor);
    void initializeAsRgb();
    QImage renderToQImage();

private:
    struct RGBComposite
    {
        int redChannel = 0;
        int greenChannel = 1;
        int blueChannel = 2;
        //int alphaChannel = -1;

        ChannelStretchParams redParams;
        ChannelStretchParams greenParams;
        ChannelStretchParams blueParams;
        //ChannelStretchParams alphaParams;

        //bool synchronizeStretch = false;
    };

    struct PseudoColorConfig
    {
        int channelIndex = 0;
        std::string colorMapName = "Viridis";
        ChannelStretchParams stretchParams;
    };

    struct SingleBandConfig
    {
        int channelIndex = 0;
        std::string colorMap = "Gray";
        ChannelStretchParams stretchParams;
    };

    struct ThematicConfig
    {
        std::unordered_map<int, QColor> classColors;
        QColor defaultColor = {200, 200, 200, 255};
    };

    // Вариант для хранения конфигурации
    using StyleVariant = std::variant<
    std::monostate,
    RGBComposite,
    PseudoColorConfig,
    SingleBandConfig,
    ThematicConfig
    >;

    common::BandRenderType mBandType = common::BandRenderType::MultiBandColor;
    std::unique_ptr<raster::RasterDataset>& mDsRef;
    StyleVariant mConfig;
};
}
}
#endif // RASTERSTYLE_H
