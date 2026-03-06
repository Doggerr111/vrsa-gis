#ifndef GRAPHICSITEMRENDERER_H
#define GRAPHICSITEMRENDERER_H

#include "graphics/vectorfeaturestyle.h"
#include "graphics/drawingcontext.h"
#include "graphics/vectorfeaturedrawingpolicy.h"
namespace vrsa
{
namespace graphics
{


class FeatureGraphicsItemRenderer
{
public:
    FeatureGraphicsItemRenderer(VectorFeatureStyle* style, common::GeometryType featureType);
    void paint(const DrawingContext& context);
    QRectF boundingRect(const DrawingContext& context);
    void renderSymbol(Symbol* symbol, const DrawingContext& context);
//    void createPolicy(Symbol* symbol);
//    void createSelectPolicy(Symbol* symbol);
    void update(); //вызывать каждый раз при изменении графического объекта,
    //например, при изменении геометрии для перестройки кеша политик

    //разбирает символ на составные простые и добавляет для соответсвующие политики, сохраняя их в контейнере
    template<typename Container>
    void createPolicies(Symbol* symbol, Container& container);


    VectorFeatureStyle* getStyle() const noexcept { return mStyle; };
    void setFeatureSelected(bool selected);

private:
    VectorFeatureStyle* mStyle;
    std::unique_ptr<VectorFeatureStyle> mSelectStyle;
    common::GeometryType mFeatureType;
    bool mIsFeatureSelected;
    std::vector<std::unique_ptr<VectorFeatureDrawingPolicy>> mPolicies;
    std::vector<std::unique_ptr<VectorFeatureDrawingPolicy>> mSelectPolicies;
};

}
}

#endif // GRAPHICSITEMRENDERER_H
