#include "rasterchannel.h"


vrsa::raster::RasterChannel::RasterChannel(GDALRasterBand *band)
    : mBand{band},
      mStorage{DataStorage<float>()}
{
    mWidth = mBand->GetXSize();
    mHeight = mBand->GetYSize();
    mDataType = mBand->GetRasterDataType();
    createStorage(mBand);
}


void vrsa::raster::RasterChannel::createStorage(GDALRasterBand *band)
{
    switch (mDataType)
    {
        case GDT_Byte:
            loadAndCalculateStats<unsigned char>(band);
            break;
        case GDT_UInt16:
            loadAndCalculateStats<unsigned short>(band);
            break;
        case GDT_Int16:
            loadAndCalculateStats<short>(band);
            break;
        case GDT_UInt32:
            loadAndCalculateStats<unsigned int>(band);
            break;
        case GDT_Int32:
            loadAndCalculateStats<int>(band);
            break;
        case GDT_Float32:
            loadAndCalculateStats<float>(band);
            break;
        case GDT_Float64:
            loadAndCalculateStats<double>(band);
            break;
        default:
            throw common::GDALDataTypeException(mDataType);
    }
}

double vrsa::raster::RasterChannel::getValueAsDouble(int x, int y) const
{
    return std::visit([x, y](const auto& mStorage) {
        return static_cast<double>(mStorage.getValue(x, y));
    }, mStorage);
}

float vrsa::raster::RasterChannel::getValueAsFloat(int x, int y) const
{
    return std::visit([x, y](const auto& mStorage) {
        return static_cast<float>(mStorage.getValue(x, y));
    }, mStorage);
}

const char* vrsa::raster::RasterChannel::getName()
{
    return mBand->GetDescription();
}

template<typename T>
void vrsa::raster::RasterChannel::loadAndCalculateStats(GDALRasterBand *band)
{
    mStorage = DataStorage<T>(mWidth, mHeight);
    auto& dataStorage = std::get<DataStorage<T>>(mStorage);
    dataStorage.loadFromGDALBand(band);

    const auto& rawData = dataStorage.getRawData();
    if (!rawData.empty()) {
        auto minIt = std::min_element(rawData.begin(), rawData.end());
        auto maxIt = std::max_element(rawData.begin(), rawData.end());
        mStyle.minValue = static_cast<double>(*minIt);
        mStyle.maxValue = static_cast<double>(*maxIt);
    }
}
