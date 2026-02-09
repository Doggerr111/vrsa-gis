#ifndef RASTERGRAPHICSITEMFACTORY_H
#define RASTERGRAPHICSITEMFACTORY_H
#include "graphics/rastergraphicsitem.h"
#include "raster/rasterchannel.h"
#include "raster/rasterdataset.h"
#include "common/GisDefines.h"
#include "common/logger.h"
#include "QDebug"
namespace vrsa
{
namespace graphics
{


class RasterGraphicsItemFactory
{
public:
    RasterGraphicsItemFactory();
public:
    static QGraphicsPixmapItem* createForDataset(raster::RasterDataset* dS)
    {
        auto& channels = dS->getChannels();
        if (channels.size()==0)
            return nullptr;

        const int size = channels.size();
        const int idx1 = std::clamp(0, 0, size - 1);
        const int idx2 = std::clamp(1, 0, size - 1);
        const int idx3 = std::clamp(2, 0, size - 1);

        std::vector<raster::RasterChannel*> vecRGB;

        if (channels[idx1]) {
            vecRGB.push_back(channels[idx1].get());
        }
        if (channels[idx2]) {
            vecRGB.push_back(channels[idx2].get());
        }
        if (channels[idx3]) {
            vecRGB.push_back(channels[idx3].get());
        }
        QImage im = createRGBImage(vecRGB);
        if (im.isNull())
        {
            VRSA_ERROR("Raster Graphics Factory", "Can't create RGB image from DataSet");
        }
        //TODO make unique ptr
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem;
        item->setPixmap(QPixmap::fromImage(im));
        item->setTransform(dS->getGeoTransform());
        return item;
    }


    template<typename T>
    static void processRGBChannels(const std::vector<raster::RasterChannel*> channels, QImage& image)
    {
        const int width = image.width();
        const int height = image.height();
        const int numPixels = width * height;

        std::array<std::vector<T>, 3> channelVectors = {
            channels[0]->getData<T>(),
            channels[1]->getData<T>(),
            channels[2]->getData<T>()
        };

        // for valid ptrs
        std::array<const T*, 3> data = {
            channelVectors[0].data(),
            channelVectors[1].data(),
            channelVectors[2].data()
        };

        //normalization
        std::array<double, 3> scale = {0, 0, 0};
        std::array<double, 3> offset = {0, 0, 0};

        for (int c = 0; c < 3; ++c)
        {
            const double range = channels[c]->getMaxValue() - channels[c]->getMinValue();
            if (range != 0.0)
            {
                scale[c] = 255.0 / range;
                offset[c] = -channels[c]->getMinValue() * scale[c];
            }
        }

        std::array<double, 3> noDataValues = {
            channels[0]->getNoDataValue(),
            channels[1]->getNoDataValue(),
            channels[2]->getNoDataValue()
        };


        for (int i = 0; i < numPixels; ++i)
        {
            // cheking if value = no data
            bool isNoData = false;
            for (int c = 0; c < 3; ++c)
            {
                if constexpr (std::is_floating_point_v<T>)
                {
                    if (std::abs(static_cast<double>(data[c][i]) - noDataValues[c]) < 1e-10)
                    {
                        isNoData = true;
                        break;
                    }
                }
                else
                {
                    if (static_cast<double>(data[c][i]) == noDataValues[c])
                    {
                        isNoData = true;
                        break;
                    }
                }
            }

            //normalizing
            std::array<unsigned char, 3> normalized = {0, 0, 0};

            for (int c = 0; c < 3; ++c)
            {
                if (scale[c] != 0.0)
                {
                    double value = static_cast<double>(data[c][i]) * scale[c] + offset[c];
                    value = std::clamp(value, 0.0, 255.0);
                    normalized[c] = static_cast<unsigned char>(std::round(value));
                }
            }

            // setting pixels
            if (isNoData)
            {
                //if pixelValue = noDataValue - alpha = 0
                image.setPixel(i % width, i / width, qRgba(normalized[0], normalized[1], normalized[2], 0));
            }
            else
            {
                image.setPixel(i % width, i / width, qRgb(normalized[0], normalized[1], normalized[2]));
            }
        }
    }

    static QImage createRGBImage(std::vector<raster::RasterChannel*> channels)
    {
        if (channels.size() != 3)
        {
            VRSA_ERROR("Raster Graphics Factory", "Wrong vector size");
            return {};
        }

        const int width = channels[0]->getWidth();
        const int height = channels[0]->getHeight();

        if (!std::all_of(channels.begin(), channels.end(),
                         [&](const auto& ch) {
                         return ch->getWidth() == width &&
                         ch->getHeight() == height &&
                         ch->getDataType() == channels[0]->getDataType();
                         }))
        {
            VRSA_ERROR("Raster Graphics Factory", "Channels mismatch");
            return {};
        }

        QImage mergedImage(width, height, QImage::Format_ARGB32_Premultiplied); // changed to ARGB32!!!

        switch (channels[0]->getDataType())
        {
        case GDT_Byte:    processRGBChannels<unsigned char>(channels, mergedImage); break;
        case GDT_UInt16:  processRGBChannels<unsigned short>(channels, mergedImage); break;
        case GDT_Int16:   processRGBChannels<short>(channels, mergedImage); break;
        case GDT_UInt32:  processRGBChannels<unsigned int>(channels, mergedImage); break;
        case GDT_Int32:   processRGBChannels<int>(channels, mergedImage); break;
        case GDT_Float32: processRGBChannels<float>(channels, mergedImage); break;
        case GDT_Float64: processRGBChannels<double>(channels, mergedImage); break;
        default:
            VRSA_ERROR("Raster Graphics Factory", "Unsupported data type");
            return {};
        }
        return mergedImage;
    }

};
}
}

#endif // RASTERGRAPHICSITEMFACTORY_H
