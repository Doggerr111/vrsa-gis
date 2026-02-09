#ifndef RASTERCHANNEL_H
#define RASTERCHANNEL_H

#include <gdal.h>
#include <gdal_priv.h>
#include <QGraphicsPixmapItem>
#include "common/gisexceptions.h"
namespace vrsa
{
namespace raster
{


class RasterChannel: public QObject
{
    Q_OBJECT

public:

    template<typename T>
    class DataStorage
    {
    private:
        std::vector<T> data;
        int width, height;
        T noDataValue;
        bool hasNoData = false;

    public:

        DataStorage() = default;

        DataStorage(int w, int h) : width(w), height(h) {
            data.resize(w * h);
        }

        void loadFromGDALBand(GDALRasterBand* band) {
            GDALDataType gdalType = getGDALType();
            band->RasterIO(GF_Read, 0, 0, width, height,
                           data.data(), width, height,
                           gdalType, 0, 0);


            int hasNoDataFlag = 0;
            double nd = band->GetNoDataValue(&hasNoDataFlag);
            if (hasNoDataFlag) {
                hasNoData = true;
                noDataValue = static_cast<T>(nd);
            }
        }

        inline T getValue(int x, int y) const
        {
            return data[y * width + x];
        }

        inline void setValue(int x, int y, T value)
        {
            data[y * width + x] = value;
        }

        inline const std::vector<T>& getRawData() const
        {
            return data;
        }

        GDALDataType getGDALType() const {
            if constexpr (std::is_same_v<T, unsigned char>) return GDT_Byte;
            if constexpr (std::is_same_v<T, unsigned short>) return GDT_UInt16;
            if constexpr (std::is_same_v<T, short>) return GDT_Int16;
            if constexpr (std::is_same_v<T, unsigned int>) return GDT_UInt32;
            if constexpr (std::is_same_v<T, int>) return GDT_Int32;
            if constexpr (std::is_same_v<T, float>) return GDT_Float32;
            if constexpr (std::is_same_v<T, double>) return GDT_Float64;
            return GDT_Unknown;
        }
    };

public:
    const char *getName();

    struct RasterStyle{
        double minValue;
        double maxValue;
    };

private:
    using StorageType = std::variant<
    DataStorage<unsigned char>,
    DataStorage<unsigned short>,
    DataStorage<short>,
    DataStorage<unsigned int>,
    DataStorage<int>,
    DataStorage<float>,
    DataStorage<double>
    >;

    int mWidth, mHeight;
    GDALDataType mDataType;
    std::string mName;
    GDALRasterBand* mBand;
    StorageType mStorage;
    RasterStyle mStyle;



public:
    RasterChannel(GDALRasterBand* band);
private:
    void createStorage(GDALRasterBand* band);
    template<typename T>
    void loadAndCalculateStats(GDALRasterBand* band);

public:
    // Универсальный метод для получения значения как double
    double getValueAsDouble(int x, int y) const;

    // Универсальный метод для получения значения как float
    float getValueAsFloat(int x, int y) const;

    inline int getWidth() const
    {
        return mWidth;
    }
    inline int getHeight() const
    {
        return mHeight;
    }
    inline GDALDataType getDataType() const
    {
        return mDataType;
    }
    inline double getMinValue() const
    {
        return mStyle.minValue;
    }
    inline double getMaxValue() const
    {
        return mStyle.maxValue;
    }
    inline double getNoDataValue() const
    {
        return mBand->GetNoDataValue();
    }
    //template method to get vector<T> (copy)
    template<typename T>
    std::vector<T> getData() const
    {
        static_assert(
        std::is_same_v<T, unsigned char> ||
                std::is_same_v<T, unsigned short> ||
                std::is_same_v<T, short> ||
                std::is_same_v<T, unsigned int> ||
                std::is_same_v<T, int> ||
                std::is_same_v<T, float> ||
                std::is_same_v<T, double>,
                "Unsupported data type. Use unsigned char, short, int, float, or double."
                );


        const auto& storage = std::get<DataStorage<T>>(mStorage);
        // returning copy
        return storage.getRawData();
    }

    // no copy method
    template<typename T>
    const std::vector<T>& getDataRef() const
    {
        static_assert(
        std::is_same_v<T, unsigned char> ||
                std::is_same_v<T, unsigned short> ||
                std::is_same_v<T, short> ||
                std::is_same_v<T, unsigned int> ||
                std::is_same_v<T, int> ||
                std::is_same_v<T, float> ||
                std::is_same_v<T, double>,
                "Unsupported data type. Use unsigned char, short, int, float, or double."
                );

        const auto& storage = std::get<DataStorage<T>>(mStorage);
        return storage.getRawData();
    }

};
}
}

#endif // RASTERLAYER_H
