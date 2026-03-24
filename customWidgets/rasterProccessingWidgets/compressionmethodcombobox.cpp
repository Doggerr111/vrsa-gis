#include "compressionmethodcombobox.h"
#include "common/rasterprocessingparams.h"

CompressionMethodComboBox::CompressionMethodComboBox(QWidget *parent)
{
    using namespace vrsa::common;
    addItem(tr("Без сжатия"), QVariant::fromValue(GeoTiffCompression::None));
    addItem(tr("LZW (без потерь, стандарт)"), QVariant::fromValue(GeoTiffCompression::LZW));
    addItem(tr("Deflate (ZIP, лучше сжатие)"), QVariant::fromValue(GeoTiffCompression::Deflate));
    addItem(tr("PackBits (быстро, слабое сжатие)"), QVariant::fromValue(GeoTiffCompression::PackBits));
    addItem(tr("JPEG (с потерями, для RGB)"), QVariant::fromValue(GeoTiffCompression::JPEG));
    addItem(tr("JPEG2000 (для больших файлов)"), QVariant::fromValue(GeoTiffCompression::JPEG2000));
    addItem(tr("ZSTD (современный, быстрый)"), QVariant::fromValue(GeoTiffCompression::ZSTD));
    addItem(tr("LZMA (максимальное сжатие, медленно)"), QVariant::fromValue(GeoTiffCompression::LZMA));
    addItem(tr("WebP (для веб-публикации)"), QVariant::fromValue(GeoTiffCompression::WEBP));
}
