#include "rasterstyle.h"


vrsa::graphics::RasterStyle::RasterStyle(std::unique_ptr<raster::RasterDataset> &rDs, common::BandRenderType type)
    : mBandType{type},
      mDsRef{rDs}
{
    switch (type)
    {
    case common::BandRenderType::MultiBandColor:
        initializeAsRgb();
        break;
    case common::BandRenderType::SingleBandGray:
        //initializeAsRgb();
        break;
    }
}

void vrsa::graphics::RasterStyle::initializeAsRgb()
{
    auto &channels = mDsRef->getChannels();
    auto bandCount = channels.size();
    if (!bandCount)
        return;
    RGBComposite rgbComp;
    if (bandCount >= 3)
    {
        rgbComp.redChannel=0;
        rgbComp.greenChannel=1;
        rgbComp.blueChannel=2;

        ChannelStretchParams redStretch;
        redStretch.minValue = channels.at(0)->getMinValue();
        redStretch.maxValue = channels.at(0)->getMaxValue();
        rgbComp.redParams = redStretch;
        ChannelStretchParams greenStretch;
        greenStretch.minValue = channels.at(1)->getMinValue();
        greenStretch.maxValue = channels.at(1)->getMaxValue();
        rgbComp.greenParams = greenStretch;
        ChannelStretchParams blueStretch;
        blueStretch.minValue = channels.at(2)->getMinValue();
        blueStretch.maxValue = channels.at(2)->getMaxValue();
        rgbComp.blueParams = blueStretch;
    }
    mConfig = rgbComp;
    //TODO ELSE
}

//QImage vrsa::graphics::RasterStyle::renderToQImage()
//{

//    int width = mDsRef->getChannel(std::get_if<RGBComposite>(&mConfig)->redChannel)->getWidth();
//    int height = mDsRef->getChannel(0)->getHeight();
//    int numPixels = width*height;
//    QImage im = QImage(width, height, QImage::Format_ARGB32_Premultiplied);

//    for (int i = 0; i < NumPixels; i++)
//    {

//        const auto pixelValue1 = mRasterData[mRGBStyle->bandIndex1-1][i];
//        const auto pixelValue2 = mRasterData[mRGBStyle->bandIndex2-1][i];
//        const auto pixelValue3 = mRasterData[mRGBStyle->bandIndex3-1][i];

//        int normalizedBrightness1=0;
//        int normalizedBrightness2=0;
//        int normalizedBrightness3=0;
//        if (mRGBStyle->maxPixelValue_band1 - mRGBStyle->minPixelValue_band1!=0)
//            normalizedBrightness1 = (pixelValue1 - mRGBStyle->minPixelValue_band1) * 255 / (mRGBStyle->maxPixelValue_band1 - mRGBStyle->minPixelValue_band1);
//        if (mRGBStyle->maxPixelValue_band2 - mRGBStyle->minPixelValue_band2!=0)
//            normalizedBrightness2 = (pixelValue2 - mRGBStyle->minPixelValue_band2) * 255 / (mRGBStyle->maxPixelValue_band2 - mRGBStyle->minPixelValue_band2);
//        if (mRGBStyle->maxPixelValue_band3 - mRGBStyle->minPixelValue_band3!=0)
//            normalizedBrightness3 = (pixelValue3 - mRGBStyle->minPixelValue_band3) * 255 / (mRGBStyle->maxPixelValue_band3 - mRGBStyle->minPixelValue_band3);


//        if (normalizedBrightness1>255)
//            normalizedBrightness1=255;
//        if (normalizedBrightness2>255)
//            normalizedBrightness2=255;
//        if (normalizedBrightness3>255)
//            normalizedBrightness3=255;
//        if  (pixelValue1 != noDataValue)
//        {
//            QRgb color = qRgb(normalizedBrightness1, normalizedBrightness2, normalizedBrightness3);
//            mImage.setPixel(i % mWidth, i / mWidth, color);
//        }
//        else
//        {
//            QRgb color = qRgba(normalizedBrightness1, normalizedBrightness2, normalizedBrightness3,0);
//            mImage.setPixel(i % mWidth, i / mWidth, color);
//        }

//    }
//    mPixmapItem->setPixmap(QPixmap::fromImage(mImage));
//    return true;
//}
