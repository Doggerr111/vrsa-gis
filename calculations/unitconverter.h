#ifndef UNITCONVERTER_H
#define UNITCONVERTER_H
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>
#include <QFuture>
#include "common/GisDefines.h"
#include "common/logger.h"

namespace vrsa
{
namespace calculations
{
/**
 * @russian
 *@brief   Класс для конвертации единиц измерений
 *@details Предоставляет статические функций для конвертаций пикселей в милиметры, дюймы и т.д.
 * @endrussian
 */
class UnitConversion
{
public:
    static double mmToPixels(double mm, double dpi = 96.0)
    {
        auto* screen = QGuiApplication::primaryScreen();
        if (!screen)
        {
            VRSA_WARNING("CORE", "No primary screen available, using default DPI");
            return mm * (dpi / 25.40);
        }

        static const double dots_per_millimeter = screen->physicalDotsPerInch() / 25.40;
        return mm * dots_per_millimeter;
    };
    static double convertToPixels(double value, common::StyleUnit uni)
    {
        switch (uni)
        {
        case common::StyleUnit::Millimeters:
        {
            return mmToPixels(value);
        }
        default:
            return value;
        }
    }
    static double pixelsToMm(double pixels, double dpi = 96.0);
    static double mmToScenePixels(double mm, double dpi = 96.0, double sceneScale = 1.0);

    static constexpr double STANDARD_DPI = 96.0;
};

}}




#endif // UNITCONVERTER_H
