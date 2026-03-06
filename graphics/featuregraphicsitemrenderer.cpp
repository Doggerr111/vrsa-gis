#include "featuregraphicsitemrenderer.h"
#include "pointfeaturedrawingpolicy.h"
#include "linefeaturedrawingpolicy.h"
#include "polygonfeaturedrawingpolicy.h"


vrsa::graphics::FeatureGraphicsItemRenderer::FeatureGraphicsItemRenderer(VectorFeatureStyle *style,
                                                                         common::GeometryType type)
    : mStyle{style},
      mFeatureType{type},
      mIsFeatureSelected{false}
{
    createPolicies(mStyle->getSymbol(), mPolicies);
}

void vrsa::graphics::FeatureGraphicsItemRenderer::paint(const DrawingContext &context)
{
    if (!mIsFeatureSelected)
    {
        for (const auto& policy: mPolicies)
            policy->paint(context);
    }
    else
    {
        for (const auto& policy: mSelectPolicies)
            policy->paint(context);
    }
}

QRectF vrsa::graphics::FeatureGraphicsItemRenderer::boundingRect(const DrawingContext &context)
{
    QRectF totalBoundingRect;
    bool first = true;
    int count = 0;
    if (!mIsFeatureSelected)
    {
        for (const auto& policy: mPolicies)
        {
            //qDebug()<<"calculating bounding rects for police #" << count++ << " rect - " <<policy->boundingRect(context);
            if (first)
            {
                totalBoundingRect = policy->boundingRect(context);
                //qDebug() << "BoundingRect from FeatureGraphicsItemRenderer count - " << callCount++;
                first = false;
            }
            else
            {
                totalBoundingRect = totalBoundingRect.united(policy->boundingRect(context));
                //qDebug() << "BoundingRect from FeatureGraphicsItemRenderer count - " << callCount++;
            }
        }
    }
    else
    {
        for (const auto& policy: mSelectPolicies)
        {
            //qDebug()<<"calculating bounding rects for police #" << count++ << " rect - " <<policy->boundingRect(context);
            if (first)
            {
                totalBoundingRect = policy->boundingRect(context);
                //qDebug() << "BoundingRect from FeatureGraphicsItemRenderer count - " << callCount++;
                first = false;
            }
            else
            {
                totalBoundingRect = totalBoundingRect.united(policy->boundingRect(context));
                //qDebug() << "BoundingRect from FeatureGraphicsItemRenderer count - " << callCount++;
            }
        }
    }

    return totalBoundingRect;
}

void vrsa::graphics::FeatureGraphicsItemRenderer::renderSymbol(Symbol *symbol, const DrawingContext &context)
{

}



void vrsa::graphics::FeatureGraphicsItemRenderer::update()
{
    if (!mIsFeatureSelected)
    {
        for (const auto& policy: mPolicies)
        {
            policy->rebuildCache();
        }
    }
    else
    {
        for (const auto& policy: mSelectPolicies)
        {
            policy->rebuildCache();
        }
    }
}

void vrsa::graphics::FeatureGraphicsItemRenderer::setFeatureSelected(bool selected)
{
    mIsFeatureSelected = selected;
    qDebug()<<"FeatureGraphicsItemRenderer selected!!" << mIsFeatureSelected;
    if (mIsFeatureSelected)
    {
        mSelectStyle = VectorFeatureStyle::createSelectionVectorStyle(mStyle);
        createPolicies(mSelectStyle->getSymbol(), mSelectPolicies);
        qDebug()<<"Создаем политики для рисования выбранного символа!!";
    }
    else //не буду хранить политики и стиль для выделения, проще пересоздать при необходимости
    {
        mSelectStyle.reset();
        mSelectPolicies.clear();
    }
    update();
    qDebug()<<"Обновляем кеш политик!";
}

template<typename Container>
void vrsa::graphics::FeatureGraphicsItemRenderer::createPolicies(Symbol *symbol,
                                                                         Container
                                                                         &container)
{
    if (!symbol) return;

    if (symbol->canHaveChildren()) {
        for (int i = 0; i < symbol->childCount(); ++i) {
            createPolicies(symbol->child(i), container);
        }
        return;
    }

    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        if (mFeatureType == common::GeometryType::Point)
            container.push_back(std::make_unique<PointFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiPoint)
            container.push_back(std::make_unique<MultiPointFeatureDrawingPolicy>(symbol));
        break;

    }
    case common::SymbolType::SimpleLineSymbol:
    {
        if (mFeatureType == common::GeometryType::LineString)
            container.push_back(std::make_unique<LineFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiLineString)
            container.push_back(std::make_unique<MultiLineFeatureDrawingPolicy>(symbol));
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        if (mFeatureType == common::GeometryType::Polygon)
            container.push_back(std::make_unique<PolygonFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiPolygon)
            container.push_back(std::make_unique<MultiPolygonFeatureDrawingPolicy>(symbol));
        break;
    }
    default:
        return;
    }

}


//void vrsa::graphics::FeatureGraphicsItemRenderer::createPolicy(Symbol *symbol)
//{
//    if (!symbol)
//        return;
//    if (symbol->canHaveChildren())
//    {
//        for (int i = 0; i < symbol->childCount(); ++i)
//        {
//            createPolicy(symbol->child(i));
//        }
//        return;
//    }
//    switch (symbol->type())
//    {
//    case common::SymbolType::SimplePointSymbol:
//    {
//        if (mFeatureType == common::GeometryType::Point)
//            mPolicies.push_back(std::make_unique<PointFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiPoint)
//            mPolicies.push_back(std::make_unique<MultiPointFeatureDrawingPolicy>(symbol));
//        break;

//    }
//    case common::SymbolType::SimpleLineSymbol:
//    {
//        if (mFeatureType == common::GeometryType::LineString)
//            mPolicies.push_back(std::make_unique<LineFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiLineString)
//            mPolicies.push_back(std::make_unique<MultiLineFeatureDrawingPolicy>(symbol));
//        break;
//    }
//    case common::SymbolType::SimplePolygonSymbol:
//    {
//        if (mFeatureType == common::GeometryType::Polygon)
//            mPolicies.push_back(std::make_unique<PolygonFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiPolygon)
//            mPolicies.push_back(std::make_unique<MultiPolygonFeatureDrawingPolicy>(symbol));
//        break;
//    }
//    default:
//        return;
//    }
//}

//void vrsa::graphics::FeatureGraphicsItemRenderer::createSelectPolicy(Symbol *symbol)
//{
//    if (!symbol)
//        return;
//    if (symbol->canHaveChildren())
//    {
//        for (int i = 0; i < symbol->childCount(); ++i)
//        {
//            createPolicy(symbol->child(i));
//        }
//        return;
//    }
//    switch (symbol->type())
//    {
//    case common::SymbolType::SimplePointSymbol:
//    {
//        if (mFeatureType == common::GeometryType::Point)
//            mSelectPolicies.push_back(std::make_unique<PointFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiPoint)
//            mSelectPolicies.push_back(std::make_unique<MultiPointFeatureDrawingPolicy>(symbol));
//        break;

//    }
//    case common::SymbolType::SimpleLineSymbol:
//    {
//        if (mFeatureType == common::GeometryType::LineString)
//            mSelectPolicies.push_back(std::make_unique<LineFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiLineString)
//            mSelectPolicies.push_back(std::make_unique<MultiLineFeatureDrawingPolicy>(symbol));
//        break;
//    }
//    case common::SymbolType::SimplePolygonSymbol:
//    {
//        if (mFeatureType == common::GeometryType::Polygon)
//            mSelectPolicies.push_back(std::make_unique<PolygonFeatureDrawingPolicy>(symbol));
//        else if (mFeatureType == common::GeometryType::MultiPolygon)
//            mSelectPolicies.push_back(std::make_unique<MultiPolygonFeatureDrawingPolicy>(symbol));
//        break;
//    }
//    default:
//        return;
//    }
//}
