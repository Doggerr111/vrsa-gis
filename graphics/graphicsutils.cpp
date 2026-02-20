#include "graphicsutils.h"
#include <QRandomGenerator>


QColor vrsa::graphics::GraphicsUtils::generateColor()
{
    return QColor::fromRgb(QRandomGenerator::global()->bounded(0, 255),
                           QRandomGenerator::global()->bounded(0, 255),
                           QRandomGenerator::global()->bounded(0, 255));
}
