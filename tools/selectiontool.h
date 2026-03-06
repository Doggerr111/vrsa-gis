#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "maptool.h"
namespace vrsa
{
namespace tools
{


class SelectionTool : public MapTool
{
public:
    explicit SelectionTool(graphics::MapScene* scene, QObject *parent = nullptr)
        :MapTool(scene, parent){};

    // MapTool interface
public:
    QString name() const override { return tr("Базовый класс для всех инструментов выделения"); };
    common::MapToolType type() const override { return common::MapToolType::SelectionTool; };
    QCursor cursor() const override { return QCursor(Qt::CrossCursor); };
};

}
}

#endif // SELECTIONTOOL_H
