#include "resamplingmethodcombobox.h"
#include "common/rasterprocessingparams.h"
ResamplingMethodComboBox::ResamplingMethodComboBox(QWidget *parent)
    : QComboBox(parent)
{
    using namespace vrsa::common;
    addItem(tr("Ближайший сосед"), QVariant::fromValue(ResamplingMethod::NearestNeighbour));
    addItem(tr("Билинейный"), QVariant::fromValue(ResamplingMethod::Bilinear));
    addItem(tr("Кубический"), QVariant::fromValue(ResamplingMethod::Cubic));
    addItem(tr("Кубический сплайн"), QVariant::fromValue(ResamplingMethod::CubicSpline));
    addItem(tr("Ланцош"), QVariant::fromValue(ResamplingMethod::Lanczos));
    addItem(tr("Усреднение"), QVariant::fromValue(ResamplingMethod::Average));
}
