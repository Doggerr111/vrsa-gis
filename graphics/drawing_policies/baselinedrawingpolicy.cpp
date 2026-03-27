#include "baselinedrawingpolicy.h"
#include "simplelinesymbol.h"

vrsa::graphics::BaseLineDrawingPolicy::BaseLineDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimpleLineSymbol);
    mSymbol = static_cast<const SimpleLineSymbol*>(symbol);
}
void vrsa::graphics::BaseLineDrawingPolicy::updateSymbol(const Symbol *symbol)
{
    assert(symbol->type() == common::SymbolType::SimpleLineSymbol);
    mSymbol = static_cast<const SimpleLineSymbol*>(symbol);
    updateScaleDependentParams(mCache.sceneScale);
}
void vrsa::graphics::BaseLineDrawingPolicy::paint(const DrawingContext &context)
{
    //assert(qFuzzyCompare(context.option->levelOfDetailFromTransform(context.painter->worldTransform()), context.sceneScale));
    if (mCache.path.isEmpty() || !mCache.geom) return;
    context.painter->save();
    if (!mCache.isGeomValid) //если рисуем первый раз или если изменилась геометрия в фиче
        cacheGeometry(context);
    //если масштаб равен тому, что хранили до этого, не надо перерассчитывать qpen width и offset
    if (!mCache.isScaleValid(context.sceneScale))
        updateScaleDependentParams(context.sceneScale);
    context.painter->setPen(mCache.penScaled);
    context.painter->translate(mCache.offsetXScaled , mCache.offsetYScaled);
    context.painter->setOpacity(mSymbol->opacity);
//    if (mSymbol->pen().widthF() > 1)
//        context.painter->setRenderHint(QPainter::Antialiasing, false);
    context.painter->drawPath(mCache.path);
    context.painter->restore();
}
void vrsa::graphics::BaseLineDrawingPolicy::updateStrokePath() const
{
    QPainterPathStroker stroker(mCache.penScaled);
    mCache.strokePath = stroker.createStroke(mCache.path);
    mCache.isStrokedPathValid = true;
}

void vrsa::graphics::BaseLineDrawingPolicy::updateScaleDependentParams(double scale) const
{
    mCache.sceneScale = scale;
    QPen pen = mSymbol->pen();
    double width = pen.widthF()/mCache.sceneScale;
    //static const double VISIBILITY_THRESHOLD = 0.06;
    //qDebug()<<width;
    if (width>0)
    {
        pen.setWidthF(width);
        mCache.penScaled = pen;
        mCache.offsetXScaled = mSymbol->getXOffSet() / mCache.sceneScale;
        mCache.offsetYScaled = mSymbol->getYOffSet() / mCache.sceneScale;
        updateStrokePath();
    }

}

QRectF vrsa::graphics::BaseLineDrawingPolicy::boundingRect(const DrawingContext &context) const
{
    //qDebug() << "boundingRect called with scale:" << context.mapScaleDenominator;
    if (!mCache.isGeomValid)
        cacheGeometry(context);
    if (context.isLodOn)
    {
        auto [lodgeom, lodpath] = mLodCache->getForScale(context.mapScaleDenominator);
        mCache.geom = lodgeom;
        mCache.path = lodpath;
    }
//    qDebug()<<mCache.geom;
//    qDebug()<<mCache.path;
    if (!mCache.isScaleValid(context.sceneScale))
        updateScaleDependentParams(context.sceneScale);
    if (!mCache.isStrokedPathValid)
        updateStrokePath();
    return mCache.strokePath.boundingRect();

//    if (mCache.isGeomValid && mCache.isBoundingRectValid && (context.sceneScale == mCache.sceneScale))
                //        return mCache.boundingRect;
}


