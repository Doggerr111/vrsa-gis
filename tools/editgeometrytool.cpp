#include "editgeometrytool.h"
#include <QDebug>
#include "graphics/featuregraphicsitem.h"
#include "graphics/mapscene.h"
#include "graphics/rubberbanditem.h"
#include "gdal/ogrconverter.h"
#include <QMessageBox>
vrsa::tools::EditGeometryTool::EditGeometryTool(graphics::MapScene *scene, QObject *parent)
    : MapTool(scene, parent),
      mSelectionTool{std::make_unique<SingleSelectionTool>(scene, parent)},
      mCurrentState{Selecting},
      mSelectedFeature{nullptr},
      mRubberBand{nullptr}
{
    if (mSelectionTool)
        connect(mSelectionTool.get(), &MapTool::toolEvent, this, &EditGeometryTool::onFeatureSelected);

    mMapScene->setPanningEnable(false);

    emit statusMessage(tr("Выберите объект для редактирования"));
}

vrsa::tools::EditGeometryTool::~EditGeometryTool()
{
    qDebug()<< "~EditGeometryTool() called";
    if (mRubberBand)
        mRubberBand->clearVertexes();

}



void vrsa::tools::EditGeometryTool::onFeatureSelected(ToolEventType type, const QVariant &data)
{
    if (type != ToolEventType::FeatureSelected)
        return;
    auto selectedFeatureGraphicsItem = data.value<graphics::FeatureGraphicsItem*>();
    if (!selectedFeatureGraphicsItem || selectedFeatureGraphicsItem == mSelectedFeature)
        return;

    mSelectedFeature = selectedFeatureGraphicsItem;
    qDebug() << "EditGeometryTool: selected feature address:" << static_cast<void*>(mSelectedFeature);
    mCurrentState = Editing;
    mMapScene->removeTemporaryItems();
    mRubberBand = nullptr;
    if (!mRubberBand)
    {
        auto rubberBandUPtr = std::make_unique<graphics::RubberBandItem>(mSelectedFeature->getFeatureGeometryType());
        mRubberBand = rubberBandUPtr.get();
        mMapScene->addTemporaryItem(std::move(rubberBandUPtr));
        mRubberBand->setTargetItem(mSelectedFeature);
        connect(mRubberBand, &graphics::RubberBandItem::vertexReleased, this, &EditGeometryTool::onVertexReleased);
    }
    else
    {
        mRubberBand->clearVertexes();
        mRubberBand->show();
        mRubberBand->setTargetItem(mSelectedFeature);
    }
    mUndoStack.emplace(mSelectedFeature, mRubberBand->getTargetOriginalGeometry());
    emit statusMessage(tr("Перемещайте вершины или выберите новый объект. Ctrl+Z — отменить, Ctrl+Y - вернуть, Esc — выход"));


}

void vrsa::tools::EditGeometryTool::onVertexReleased(graphics::FeatureGraphicsItem *item, geometry::Geometry geom)
{
    if (item)
        mUndoStack.emplace(item, geom);
}

void vrsa::tools::EditGeometryTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //if (mCurrentState == Selecting)
    mSelectionTool->mousePressEvent(event);

}

void vrsa::tools::EditGeometryTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

void vrsa::tools::EditGeometryTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void vrsa::tools::EditGeometryTool::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Z) //undo
    {
        undo();
    }
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Y) //redo
    {
        redo();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        if  (!mRubberBand)
            return;
        mRubberBand->clearVertexes();
        mRubberBand->hide();
        if (mSelectionTool)
            mSelectionTool->deselectCurrentVectorFeature();
        mSelectedFeature = nullptr;
    }
}


void vrsa::tools::EditGeometryTool::undo()
{
    if (mUndoStack.empty())
        return;
    auto [item, oldGeom] = mUndoStack.top();
    mUndoStack.pop();
    if (!item)
        return;


    auto currentGeom = ogr_utils::OGRConverter::fromOGR(item->getFeatureGeometry());
    mRedoStack.emplace(item, currentGeom);
    item->onGeometryChanged(oldGeom);
    if (mRubberBand)
    {
        mRubberBand->clearVertexes();
        mRubberBand->setTargetItem(item);
        if (mSelectionTool)
            mSelectionTool->selectVectorFeature(item);
    }

}

void vrsa::tools::EditGeometryTool::redo()
{
    if (mRedoStack.empty())
        return;
    auto [item, newGeom] = mRedoStack.top();
    mRedoStack.pop();
    if (!item)
        return;
    auto currentGeom = ogr_utils::OGRConverter::fromOGR(item->getFeatureGeometry());
    mUndoStack.emplace(item, currentGeom);
    item->onGeometryChanged(newGeom);
    if (mRubberBand)
    {
        mRubberBand->clearVertexes();
        mRubberBand->setTargetItem(item);
        if (mSelectionTool)
            mSelectionTool->selectVectorFeature(item);
    }
}

void vrsa::tools::EditGeometryTool::undoAll()
{
    while (!mUndoStack.empty())
        undo();
}




void vrsa::tools::EditGeometryTool::deactivate()
{
    qDebug()<< "EditGeometryTool deactivate() called";
    if (!mUndoStack.empty())
    {
        if (QMessageBox::question(nullptr, tr("Сохранение изменений"), tr("Желаете сохранить изменения?")) == QMessageBox::No)
        {
            undoAll();
        }
    }
}
