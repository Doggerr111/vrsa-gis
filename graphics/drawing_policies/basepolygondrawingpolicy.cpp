#include "basepolygondrawingpolicy.h"
#include "graphics/symbols/simplepolygonsymbol.h"

vrsa::graphics::BasePolygonDrawingPolicy::BasePolygonDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimplePolygonSymbol);
    mSymbol = static_cast<const SimplePolygonSymbol*>(symbol);
}

void vrsa::graphics::BasePolygonDrawingPolicy::paint(const DrawingContext &context)
{
    context.painter->save();
    if (!mCache.isGeomValid) //если рисуем первый раз или если изменилась геометрия в фиче
        cacheGeometry(context.geom, context.sceneScale);
    //если масштаб равен тому, что хранили до этого, не надо перерассчитывать qpen width и offset
    if (!mCache.isScaleValid(context.sceneScale))
        updateScaleDependentParams(context.sceneScale);
    context.painter->setPen(mCache.penScaled);
    if (mSymbol->pen().widthF() > 0.5)
    {
        context.painter->setRenderHint(QPainter::Antialiasing, false);
    }
    context.painter->translate(mCache.offsetXScaled , mCache.offsetYScaled);
    QBrush brush = mSymbol->brush();
    brush.setTransform(context.painter->worldTransform().inverted()); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);
    context.painter->setOpacity(mSymbol->opacity);
    context.painter->drawPath(mCache.path);
    context.painter->restore();
}
void vrsa::graphics::BasePolygonDrawingPolicy::updateStrokePath() const
{
    QPainterPathStroker stroker(mCache.penScaled);
    mCache.strokePath = stroker.createStroke(mCache.path);
    mCache.isStrokedPathValid = true;
}

void vrsa::graphics::BasePolygonDrawingPolicy::updateScaleDependentParams(double scale) const
{
    mCache.sceneScale = scale;
    QPen pen = mSymbol->pen();
    pen.setWidthF(pen.widthF()/mCache.sceneScale);
    mCache.penScaled = pen;
    mCache.offsetXScaled = mSymbol->getXOffSet() / mCache.sceneScale;
    mCache.offsetYScaled = mSymbol->getYOffSet() / mCache.sceneScale;
    updateStrokePath();
}

QRectF vrsa::graphics::BasePolygonDrawingPolicy::boundingRect(const DrawingContext &context) const
{
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom, context.sceneScale);
    if (!mCache.isScaleValid(context.sceneScale))
        updateScaleDependentParams(context.sceneScale);
    if (!mCache.isStrokedPathValid)
        updateStrokePath();
    return mCache.strokePath.boundingRect();
}
