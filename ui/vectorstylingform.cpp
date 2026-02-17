#include "vectorstylingform.h"
#include "ui_vectorstylingform.h"
#include "graphics/symbols/symbolrenderer.h"
#include <QGraphicsPixmapItem>
VectorStylingForm::VectorStylingForm(vrsa::graphics::Symbol * s, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VectorStylingForm)
{
    ui->setupUi(this);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, "root");
    ui->treeWidget->addTopLevelItem(item);
    for (int i=0; i<s->childCount(); ++i)
    {
        QTreeWidgetItem* itemchild = new QTreeWidgetItem();
        itemchild->setText(0, "child #" + QString::number(i));
        item->addChild(itemchild);
    }
    auto pix = vrsa::graphics::SymbolRenderer::renderSymbol(s);
    ui->treeWidget->topLevelItem(0)->setIcon(0, QIcon(pix));

    QGraphicsPixmapItem* pixItem = new QGraphicsPixmapItem(pix);
    ui->graphicsView->setScene(new QGraphicsScene);
    //ui->graphicsView->setTransform(QTransform());
    qDebug()<< ui->graphicsView->transform().m11();
    // Центрируем на сцене
    QRectF sceneRect = ui->graphicsView->scene()->sceneRect();
    pixItem->setPos(sceneRect.center() - pixItem->boundingRect().center());

    // Добавляем на сцену
    ui->graphicsView->scene()->addItem(pixItem);

    // Масштабируем view чтобы увидеть
    //ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);

}

VectorStylingForm::~VectorStylingForm()
{
    delete ui;
}

void VectorStylingForm::on_toolButtonAddSymbol_clicked()
{

}


void VectorStylingForm::on_toolButtonDeleteSymbol_clicked()
{

}


void VectorStylingForm::on_toolButtonCopySymbol_clicked()
{

}

