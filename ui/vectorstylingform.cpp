#include "vectorstylingform.h"
#include "ui_vectorstylingform.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsPathItem>
#include "common/logger.h"
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"
#include "graphics/symbols/layersymbol.h"
#include "customWidgets/stylingWidgets/vectorstylingspinbox.h"
#include "customWidgets/stylingWidgets/stylingcomboboxes.h"
#include "customWidgets/stylingWidgets/colorpushbutton.h"
#include <QLabel>

#ifdef VRSA_ENABLE_TEST_UTILS
#include "test_utils/testsymbolfactory.h"
#endif

VectorStylingForm::VectorStylingForm(vrsa::graphics::Symbol* symbol, LayerGeometryType type, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::VectorStylingForm),
      mSymbol{symbol},
      mScene{new QGraphicsScene},
      mLastSelectedItem{nullptr},
      mSymbolRenderer(mScene)
{
    ui->setupUi(this);
    if (mSymbol)
        mClonedSymbol = mSymbol->clone();

    connect(this, &VectorStylingForm::itemRenderRequired, &mSymbolRenderer, &vrsa::graphics::SymbolRenderer::renderToItem);
    connect(this, &VectorStylingForm::symbolRenderRequired, &mSymbolRenderer, &vrsa::graphics::SymbolRenderer::renderToIcon);
    itemRenderRequired(mClonedSymbol.get());
    //auto scene = new QGraphicsScene;
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    //vrsa::graphics::SymbolRenderer::renderSymbolToGraphicsItem(mClonedSymbol.get(), mItem.get());
    //ui->graphicsView->scene()->addItem(mItem.get());


    initialize();
}

#ifdef VRSA_ENABLE_TEST_UTILS
VectorStylingForm::VectorStylingForm(int testCase, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::VectorStylingForm),
      mSymbol{nullptr},
      mScene{new QGraphicsScene},
      mLastSelectedItem{nullptr},
      mSymbolRenderer(mScene)
{
    ui->setupUi(this);
    switch (testCase)
    {
    case 0:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithOnePoint();
        break;
    case 1:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithThreePoints();
        break;
    case 2:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPointSymbolWithFivePoints();
        break;
    case 3:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithOneLine();
        break;
    case 4:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithThreeLines();
        break;
    case 5:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestLineSymbolWithFiveLines();
        break;
    case 6:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithOnePolygon();
        break;
    case 7:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithThreePolygons();
        break;
    case 8:
        mClonedSymbol = vrsa::test_utils::TestSymbolFactory::createTestPolygonSymbolWithFivePolygons();
        break;
    }
    mClonedSymbol.get();
    connect(this, &VectorStylingForm::itemRenderRequired, &mSymbolRenderer, &vrsa::graphics::SymbolRenderer::renderToItem);
    connect(this, &VectorStylingForm::symbolRenderRequired, &mSymbolRenderer, &vrsa::graphics::SymbolRenderer::renderToIcon);
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    //vrsa::graphics::SymbolRenderer::renderSymbolToGraphicsItem(mClonedSymbol.get(), mItem.get());
    itemRenderRequired(mClonedSymbol.get());
    initialize();
}
#endif //VRSA_ENABLE_TEST_UTILS

VectorStylingForm::~VectorStylingForm()
{
    delete ui;
}

void VectorStylingForm::initialize()
{
    setStackedWidgetPage();
    buildSymbolTree();
    ui->treeWidget->header()->setVisible(false);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged,
            this, &VectorStylingForm::om_TreeWidgetCurretItemChanged);
}

void VectorStylingForm::setStackedWidgetPage()
{
    switch (mGeomType)
    {
    case LayerGeometryType::Point:
    case LayerGeometryType::MultiPoint:
    {
        ui->stackedWidget->setCurrentIndex(pointPageIndx);
        break;
    }
    case LayerGeometryType::LineString:
    case LayerGeometryType::MultiLineString:
    {
        ui->stackedWidget->setCurrentIndex(linePageIndx);
        break;
    }
    case LayerGeometryType::Polygon:
    case LayerGeometryType::MultiPolygon:
    {
        ui->stackedWidget->setCurrentIndex(polygonPageIndx);
        break;
    }
    default:
        VRSA_ERROR("VectorStylingForm","Unsupported layer geometry type");
        break;
    }
}

QTreeWidgetItem *VectorStylingForm::createItem(Symbol* symbol)
{
    auto item = new QTreeWidgetItem;
    if (!item)
        return nullptr;
    //item->setTextAlignment(0, Qt::AlignCenter);  // по горизонтали и вертикали
    item->setTextAlignment(1, Qt::AlignVCenter); // только по вертикали
    item->setText(TEXT_COLUMN, symbol->mName);
    qDebug()<< symbol->mName;
    item->setData(DATA_COLUMN, vrsa::common::SymbolPtrRole, QVariant::fromValue(symbol));
    QIcon icon = QIcon();
    emit symbolRenderRequired(symbol, icon);
    item->setIcon(DATA_COLUMN, icon);
    return item;
}

void VectorStylingForm::buildSymbolTree()
{
    if (!mClonedSymbol)
        return;
    QTreeWidgetItem* rootItem = createItem(mClonedSymbol.get());
    auto pix = vrsa::graphics::SymbolRenderer::renderSymbolToPixmap(mClonedSymbol.get());
    rootItem->setIcon(0, QIcon(pix));
    ui->treeWidget->addTopLevelItem(rootItem);
    qDebug() << "Items count:" << ui->treeWidget->topLevelItemCount();
    //static_cast<vrsa::graphics::LayerSymbol*>(mClonedSymbol.get())->reverseChildren();
    for (int i=0; i<mClonedSymbol.get()->childCount(); ++i)
    {
        auto childSymbol = mClonedSymbol.get()->child(i);
        populateTree(childSymbol, rootItem);
    }
}

void VectorStylingForm::populateTree(Symbol *symbol, QTreeWidgetItem *parentItem)
{
    using namespace vrsa::common;
    if (!symbol)
        return;
    if (symbol->canHaveChildren())
    {
        QTreeWidgetItem* rootItem = createItem(symbol);
        parentItem->addChild(rootItem);
        for (int i=0; i<symbol->childCount(); ++i)
        {
            auto childSymbol = symbol->child(i);
            populateTree(childSymbol, rootItem);
        }
        return;
    }
    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        QTreeWidgetItem* childItem = createItem(symbol);
        parentItem->addChild(childItem);
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        QTreeWidgetItem* childItem = createItem(symbol);
        parentItem->addChild(childItem);
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        QTreeWidgetItem* childItem = createItem(symbol);
        parentItem->addChild(childItem);
        break;
    }
    default:
        break;
    }
}

void VectorStylingForm::update()
{
    if (!mClonedSymbol)
        return;
    auto rootItem = ui->treeWidget->topLevelItem(0);
    if (!rootItem)
        return;
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);
    itemRenderRequired(mClonedSymbol.get());
}

QTreeWidgetItem *VectorStylingForm::findItemByData(const QVariant &data, int role)
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
        if (item->data(0, role) == data)
            return item;
        if (auto* child = findItemInChildren(item, data, role))
            return child;
    }
    return nullptr;
}

QTreeWidgetItem *VectorStylingForm::findItemInChildren(QTreeWidgetItem *parent, const QVariant &data, int role)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        QTreeWidgetItem* child = parent->child(i);
        if (child->data(0, role) == data)
            return child;
        if (auto* found = findItemInChildren(child, data, role))
            return found;
    }
    return nullptr;
}

QWidget *VectorStylingForm::createSimplePointPage(Symbol *symbol)
{
    if (!symbol)
        return nullptr;
    if (symbol->type()!=SymbolType::SimplePointSymbol)
        return nullptr;
    QWidget* page = new QWidget(ui->stackedWidget);
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    ui->stackedWidget->addWidget(page);
    QFrame* frame = new QFrame(page);
    frame->setFrameShape(QFrame::StyledPanel);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);

    // виджеты для настройки параметров точки
    frameLayout->addWidget(addSpinBoxFrame(tr("Размер символа"), symbol, StyleParam::PointSize));
    frameLayout->addWidget(addColorBtnFrame(symbol, StyleParam::PenColor));
    frameLayout->addWidget(addColorBtnFrame(symbol, StyleParam::BrushColor));
    frameLayout->addWidget(addComboBoxFrame(tr("Тип точки"), symbol, StyleParam::PointType));
    frameLayout->addWidget(addComboBoxFrame(tr("Стиль границы"), symbol, StyleParam::PenStyle));
    frameLayout->addWidget(addComboBoxFrame(tr("Стиль заливки"), symbol, StyleParam::BrushStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Толщина границы"), symbol, StyleParam::PenWidth));
    frameLayout->addWidget(addComboBoxFrame(tr("Тип соединения"), symbol, StyleParam::JoinStyle));
    frameLayout->addWidget(addComboBoxFrame(tr("Оконечности"), symbol, StyleParam::CapStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Поворот"), symbol, StyleParam::Rotation));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по X"), symbol, StyleParam::XOffset));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по Y"), symbol, StyleParam::YOffset));
    frameLayout->addWidget(addSliderFrame(tr("Прозрачность"), symbol, StyleParam::Opacity));

    // обавляем фрейм на страницу
    pageLayout->addWidget(frame);
    pageLayout->addStretch();  // чтобы не растягивалось
    ui->stackedWidget->setCurrentWidget(page);
    return page;

}

QWidget *VectorStylingForm::createSimpleLinePage(Symbol *symbol)
{
    if (!symbol)
        return nullptr;
    if (symbol->type()!=SymbolType::SimpleLineSymbol)
        return nullptr;
    QWidget* page = new QWidget(ui->stackedWidget);
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    ui->stackedWidget->addWidget(page);
    QFrame* frame = new QFrame(page);
    frame->setFrameShape(QFrame::StyledPanel);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);

    // виджеты для настройки параметров линии
    frameLayout->addWidget(addColorBtnFrame(symbol, StyleParam::PenColor));
    frameLayout->addWidget(addComboBoxFrame(tr("Стиль линии"), symbol, StyleParam::PenStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Толщина линии"), symbol, StyleParam::PenWidth));
    frameLayout->addWidget(addComboBoxFrame(tr("Тип соединения"), symbol, StyleParam::JoinStyle));
    frameLayout->addWidget(addComboBoxFrame(tr("Оконечности"), symbol, StyleParam::CapStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по X"), symbol, StyleParam::XOffset));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по Y"), symbol, StyleParam::YOffset));
    frameLayout->addWidget(addSliderFrame(tr("Прозрачность"), symbol, StyleParam::Opacity));

    // обавляем фрейм на страницу
    pageLayout->addWidget(frame);
    pageLayout->addStretch();  // чтобы не растягивалось
    ui->stackedWidget->setCurrentWidget(page);
    return page;
}

QWidget *VectorStylingForm::createSimplePolygonPage(Symbol *symbol)
{
    if (!symbol)
        return nullptr;
    if (symbol->type()!=SymbolType::SimplePolygonSymbol)
        return nullptr;
    QWidget* page = new QWidget(ui->stackedWidget);
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    ui->stackedWidget->addWidget(page);
    QFrame* frame = new QFrame(page);
    frame->setFrameShape(QFrame::StyledPanel);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);

    // виджеты для настройки параметров полигона
    frameLayout->addWidget(addColorBtnFrame(symbol, StyleParam::PenColor));
    frameLayout->addWidget(addColorBtnFrame(symbol, StyleParam::BrushColor));
    frameLayout->addWidget(addComboBoxFrame(tr("Стиль границы"), symbol, StyleParam::PenStyle));
    frameLayout->addWidget(addComboBoxFrame(tr("Стиль заливки"), symbol, StyleParam::BrushStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Толщина границы"), symbol, StyleParam::PenWidth));
    frameLayout->addWidget(addComboBoxFrame(tr("Тип соединения"), symbol, StyleParam::JoinStyle));
    frameLayout->addWidget(addComboBoxFrame(tr("Оконечности"), symbol, StyleParam::CapStyle));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по X"), symbol, StyleParam::XOffset));
    frameLayout->addWidget(addSpinBoxFrame(tr("Смещение по Y"), symbol, StyleParam::YOffset));
    frameLayout->addWidget(addSliderFrame(tr("Прозрачность"), symbol, StyleParam::Opacity));

    // обавляем фрейм на страницу
    pageLayout->addWidget(frame);
    pageLayout->addStretch();  // чтобы не растягивалось
    ui->stackedWidget->setCurrentWidget(page);
    return page;

}

QWidget* VectorStylingForm::addSpinBoxFrame(const QString &labelText, Symbol *symbol, const StyleParam param)
{
    QFrame* frame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(frame);
    QLabel* textLabel = new QLabel(labelText);
    frameLayout->addWidget(textLabel);

    switch (param)
    {
    case StyleParam::PointSize:
    case StyleParam::PenWidth:
    case StyleParam::Rotation:
    case StyleParam::XOffset:
    case StyleParam::YOffset:
    {
        VectorStylingSpinBox* spinBox = new VectorStylingSpinBox(symbol, param);
        spinBox->setProperty("symbol", QVariant::fromValue(symbol));
        spinBox->setProperty("param", QVariant::fromValue(param));
        connect(spinBox,
                static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &VectorStylingForm::on_SpinBoxValueChanged);
        frameLayout->addWidget(spinBox);
        return frame;
    }
    case StyleParam::Opacity: //delete this
    {
        QSlider* slider = new QSlider();
        slider->setRange(0, 100);
        slider->setProperty("symbol", QVariant::fromValue(symbol));
        slider->setProperty("param", QVariant::fromValue(param));
        frameLayout->addWidget(slider);
        return frame;
    }
    default:
        return frame;
    }

}

QWidget *VectorStylingForm::addColorBtnFrame(Symbol *symbol, const StyleParam param)
{
    QFrame* frame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(frame);
    QLabel* textLabel = new QLabel();
    frameLayout->addWidget(textLabel);
    ColorPushButton* btn = new ColorPushButton(symbol, param);
    btn->setProperty("symbol", QVariant::fromValue(symbol));
    btn->setProperty("param", QVariant::fromValue(param));
    connect(btn, &ColorPushButton::clicked, this, &VectorStylingForm::on_ColorBtnClicked);
    frameLayout->addWidget(btn);
    switch (param)
    {
    case StyleParam::PenColor:
    {
        textLabel->setText(tr("Цвет пера"));
        return frame;
    }
    case StyleParam::BrushColor:
    {
        textLabel->setText(tr("Цвет кисти"));
        return frame;
    }
    default:
        return frame;

    }

    return frame;
}

QWidget *VectorStylingForm::addComboBoxFrame(const QString &labelText, Symbol *symbol, const StyleParam param)
{
    QFrame* frame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(frame);
    QLabel* textLabel = new QLabel(labelText);
    frameLayout->addWidget(textLabel);

    switch (param)
    {
    case StyleParam::PenStyle:
    {
        PenTypeComboBox* comboBox = new PenTypeComboBox(symbol);
        comboBox->setProperty("symbol", QVariant::fromValue(symbol));
        comboBox->setProperty("param", QVariant::fromValue(param));
        connect(comboBox,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &VectorStylingForm::on_PenStyleCBoxIndexChanged);
        frameLayout->addWidget(comboBox);
        return frame;
    }
    case StyleParam::BrushStyle:
    {
        BrushTypeComboBox* comboBox = new BrushTypeComboBox(symbol);
        comboBox->setProperty("symbol", QVariant::fromValue(symbol));
        comboBox->setProperty("param", QVariant::fromValue(param));
        connect(comboBox,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &VectorStylingForm::on_BrushStyleCBoxIndexChanged);
        frameLayout->addWidget(comboBox);
        return frame;
    }
    case StyleParam::CapStyle:
    {
        PenCapComboBox* comboBox = new PenCapComboBox(symbol);
        comboBox->setProperty("symbol", QVariant::fromValue(symbol));
        comboBox->setProperty("param", QVariant::fromValue(param));
        connect(comboBox,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &VectorStylingForm::on_PenCapCBoxIndexChanged);
        frameLayout->addWidget(comboBox);
        return frame;
    }
    case StyleParam::JoinStyle:
    {
        PenJoinComboBox* comboBox = new PenJoinComboBox(symbol);
        comboBox->setProperty("symbol", QVariant::fromValue(symbol));
        comboBox->setProperty("param", QVariant::fromValue(param));
        connect(comboBox,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &VectorStylingForm::on_PenJoinCBoxIndexChanged);
        frameLayout->addWidget(comboBox);
        return frame;
    }
    case StyleParam::PointType:
    {
        PointTypeComboBox* comboBox = new PointTypeComboBox(symbol);
        comboBox->setProperty("symbol", QVariant::fromValue(symbol));
        comboBox->setProperty("param", QVariant::fromValue(param));
        connect(comboBox,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &VectorStylingForm::on_PointTypeCBoxIndexChanged);
        frameLayout->addWidget(comboBox);
        return frame;
    }
    default:
        return frame;

    }
    return frame;
}

QWidget *VectorStylingForm::addSliderFrame(const QString &labelText, Symbol *symbol, const StyleParam param)
{
    QFrame* frame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(frame);
    QLabel* textLabel = new QLabel(labelText);
    frameLayout->addWidget(textLabel);

    if (param == StyleParam::Opacity)
    {
        QSlider* slider = new QSlider(Qt::Horizontal);

        slider->setRange(0, 100);
        slider->setProperty("symbol", QVariant::fromValue(symbol));
        slider->setProperty("param", QVariant::fromValue(param));
        connect(slider, &QSlider::valueChanged, this, [this, slider](int value){
            StyleParam param = slider->property("param").value<StyleParam>();
            Symbol* symbol = slider->property("symbol").value<Symbol*>();
            if (!symbol)
                return;
            switch (symbol->type())
            {
            case SymbolType::SimplePointSymbol:
            {
                auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
                if (param == StyleParam::Opacity)
                    simplePointS->opacity = static_cast<double>(value) / 100;
                break;
            }
            case SymbolType::SimpleLineSymbol:
            {
                auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
                if (param == StyleParam::Opacity)
                    simpleLineS->opacity = static_cast<double>(value) / 100;
                break;

            }
            case SymbolType::SimplePolygonSymbol:
            {
                auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
                if (param == StyleParam::Opacity)
                    simplePolyS->opacity = static_cast<double>(value) / 100;
                break;
            }
            default:
                break;
            }
            auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
            QIcon itemIcon = QIcon();
            symbolRenderRequired(symbol, itemIcon);
            treeItem->setIcon(DATA_COLUMN, itemIcon);

            auto rootItem = ui->treeWidget->topLevelItem(0);
            QIcon rootItemIcon = QIcon();
            symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
            rootItem->setIcon(DATA_COLUMN, rootItemIcon);

            itemRenderRequired(mClonedSymbol.get());
        });
        frameLayout->addWidget(slider);

    }
    return frame;
}

void VectorStylingForm::on_SpinBoxValueChanged(double value)
{
    auto spinBoxSender = qobject_cast<VectorStylingSpinBox*>(sender());
    if (!spinBoxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = spinBoxSender->property("param").value<StyleParam>();
    Symbol* symbol = spinBoxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a spinBox property");
        return;
    }

    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        switch (param)
        {
        case StyleParam::PointSize:
            simplePointS->pointSize = value;
            break;
        case StyleParam::PenWidth:
            simplePointS->borderWidth = value;
            break;
        case StyleParam::Rotation:
            simplePointS->rotation = value;
            break;
        case StyleParam::XOffset:
            simplePointS->xOffset = value;
            break;
        case StyleParam::YOffset:
            simplePointS->yOffset = value;
            break;
        default:
            break;
        }
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
        switch (param)
        {
        case StyleParam::PenWidth:
            simpleLineS->borderWidth = value;
            break;
        case StyleParam::XOffset:
            simpleLineS->xOffset = value;
            break;
        case StyleParam::YOffset:
            simpleLineS->yOffset = value;
            break;
        default:
            break;
        }
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        switch (param)
        {
        case StyleParam::PenWidth:
            simplePolyS->borderWidth = value;
            break;
        case StyleParam::XOffset:
            simplePolyS->xOffset = value;
            break;
        case StyleParam::YOffset:
            simplePolyS->yOffset = value;
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());


}

void VectorStylingForm::on_ColorBtnClicked()
{
    auto colorBtnSender = qobject_cast<ColorPushButton*>(sender());
    if (!colorBtnSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = colorBtnSender->property("param").value<StyleParam>();
    Symbol* symbol = colorBtnSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a spinBox property");
        return;
    }


    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        if (param == StyleParam::PenColor)
            simplePointS->borderColor = colorBtnSender->getColor();
        else if (param == StyleParam::BrushColor)
            simplePointS->fillColor = colorBtnSender->getColor();
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
        if (param == StyleParam::PenColor)
            simpleLineS->borderColor = colorBtnSender->getColor();
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        if (param == StyleParam::PenColor)
            simplePolyS->borderColor = colorBtnSender->getColor();
        if (param == StyleParam::BrushColor)
            simplePolyS->fillColor = colorBtnSender->getColor();
        break;
    }
    default:
        break;
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

void VectorStylingForm::on_PenStyleCBoxIndexChanged(int indx)
{
    auto cboxSender = qobject_cast<PenTypeComboBox*>(sender());
    if (!cboxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = cboxSender->property("param").value<StyleParam>();
    Symbol* symbol = cboxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a combobox property");
        return;
    }
    if (param != StyleParam::PenStyle)
        return;
    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        simplePointS->borderStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
        simpleLineS->borderStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        simplePolyS->borderStyle = cboxSender->getStyle();
        break;
    }
    default:
        break;
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

void VectorStylingForm::on_BrushStyleCBoxIndexChanged(int indx)
{
    auto cboxSender = qobject_cast<BrushTypeComboBox*>(sender());
    if (!cboxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = cboxSender->property("param").value<StyleParam>();
    Symbol* symbol = cboxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a combobox property");
        return;
    }
    if (param != StyleParam::BrushStyle)
        return;
    if (symbol->type() == vrsa::common::SymbolType::SimplePointSymbol)
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        simplePointS->fillStyle = cboxSender->getStyle();
    }
    else if (symbol->type() == vrsa::common::SymbolType::SimplePolygonSymbol)
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        simplePolyS->fillStyle = cboxSender->getStyle();
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

void VectorStylingForm::on_PenJoinCBoxIndexChanged(int indx)
{
    auto cboxSender = qobject_cast<PenJoinComboBox*>(sender());
    if (!cboxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = cboxSender->property("param").value<StyleParam>();
    Symbol* symbol = cboxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a combobox property");
        return;
    }
    if (param != StyleParam::JoinStyle)
        return;
    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        simplePointS->joinStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
        simpleLineS->joinStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        simplePolyS->joinStyle = cboxSender->getStyle();
        break;
    }
    default:
        break;
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

void VectorStylingForm::on_PenCapCBoxIndexChanged(int indx)
{
    auto cboxSender = qobject_cast<PenCapComboBox*>(sender());
    if (!cboxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = cboxSender->property("param").value<StyleParam>();
    Symbol* symbol = cboxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a combobox property");
        return;
    }
    if (param != StyleParam::CapStyle)
        return;
    switch (symbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
        simplePointS->capStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        auto simpleLineS = static_cast<vrsa::graphics::SimpleLineSymbol*>(symbol);
        simpleLineS->capStyle = cboxSender->getStyle();
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        auto simplePolyS = static_cast<vrsa::graphics::SimplePolygonSymbol*>(symbol);
        simplePolyS->capStyle = cboxSender->getStyle();
        break;
    }
    default:
        break;
    }
    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

void VectorStylingForm::on_PointTypeCBoxIndexChanged(int indx)
{
    auto cboxSender = qobject_cast<PointTypeComboBox*>(sender());
    if (!cboxSender)
    {
        VRSA_DEBUG("Vector Styling Form", "qobject_cast error");
        return;
    }
    StyleParam param = cboxSender->property("param").value<StyleParam>();
    Symbol* symbol = cboxSender->property("symbol").value<Symbol*>();
    if (!symbol)
    {
        VRSA_DEBUG("Vector Styling Form", "nullptr symbol in a combobox property");
        return;
    }
    if (param != StyleParam::PointType || symbol->type() != SymbolType::SimplePointSymbol)
        return;
    auto simplePointS = static_cast<vrsa::graphics::SimplePointSymbol*>(symbol);
    simplePointS->pointType = cboxSender->getPointType();

    auto treeItem = findItemByData(QVariant::fromValue(symbol), vrsa::common::SymbolPtrRole);
    QIcon itemIcon = QIcon();
    symbolRenderRequired(symbol, itemIcon);
    treeItem->setIcon(DATA_COLUMN, itemIcon);

    auto rootItem = ui->treeWidget->topLevelItem(0);
    QIcon rootItemIcon = QIcon();
    symbolRenderRequired(mClonedSymbol.get(), rootItemIcon);
    rootItem->setIcon(DATA_COLUMN, rootItemIcon);

    itemRenderRequired(mClonedSymbol.get());
}

//adding symbol
void VectorStylingForm::on_toolButtonAddSymbol_clicked()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if (!currentItem)
        return;
    auto currentSymbol = currentItem->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    if (!currentSymbol)
        return;
    if (!currentSymbol->canHaveChildren())
        return;
    switch (currentSymbol->type())
    {
    case vrsa::common::SymbolType::LayerPointSymbol:
    {
        auto newPoint = vrsa::graphics::SimplePointSymbol::createDefaultSymbol();
        currentItem->addChild(createItem(newPoint.get()));
        currentSymbol->addChild(std::move(newPoint));
        break;
    }
    case vrsa::common::SymbolType::LayerLineSymbol:
    {
        auto newLine = vrsa::graphics::SimpleLineSymbol::createDefaultSymbol();
        currentItem->addChild(createItem(newLine.get()));
        currentSymbol->addChild(std::move(newLine));
        break;
    }
    case vrsa::common::SymbolType::LayerPolygonSymbol:
    {
        auto newPoly = vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol();
        currentItem->addChild(createItem(newPoly.get()));
        currentSymbol->addChild(std::move(newPoly));
        break;
    }
    default:
        break;
    }
    update();

    //currentItem->data();

}


void VectorStylingForm::on_toolButtonDeleteSymbol_clicked()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if (!currentItem)
        return;
    auto currentSymbol = currentItem->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    if (!currentSymbol || currentSymbol == mClonedSymbol.get())
        return;
    for (int i = 0; i < mClonedSymbol->childCount(); ++i)
    {
        if (mClonedSymbol->child(i) == currentSymbol)
        {
            mClonedSymbol->removeChild(i);
            auto root = ui->treeWidget->topLevelItem(0);
            root->removeChild(currentItem); //TODO ИЗМЕНИТЬ, НЕ БРАТЬ ПРОСТО КОРЕНь!!
            delete currentItem;
            update();
            ui->treeWidget->setCurrentItem(root);
            return;
        }
    }
}


void VectorStylingForm::on_toolButtonCopySymbol_clicked()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if (!currentItem)
        return;
    auto currentSymbol = currentItem->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    if (!currentSymbol || currentSymbol == mClonedSymbol.get())
        return;
    auto clone = currentSymbol->clone();
    ui->treeWidget->topLevelItem(0)->addChild(createItem(clone.get()));
    mClonedSymbol->addChild(std::move(clone));

    update();

}

void VectorStylingForm::om_TreeWidgetCurretItemChanged(QTreeWidgetItem *currentItem, QTreeWidgetItem *previousItem)
{

    qDebug() << "Current item changed from" << (previousItem ? previousItem->text(0) : "none")
             << "to" << (currentItem ? currentItem->text(0) : "none");

    if (!currentItem)
        return;
    mLastSelectedItem = currentItem;
    auto currentSymbol = currentItem->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    if (!currentSymbol)
        return;
    ui->toolButtonAddSymbol->setEnabled(currentSymbol->canHaveChildren());
    ui->toolButtonCopySymbol->setEnabled(!currentSymbol->canHaveChildren());
    ui->toolButtonDeleteSymbol->setEnabled(currentSymbol->childCount()>1);
    ui->toolButtonDeleteSymbol->setEnabled(currentSymbol != mClonedSymbol.get());

    auto rootItem = ui->treeWidget->topLevelItem(0);
    auto indx = rootItem->indexOfChild(currentItem);
    qDebug()<< "INDEX ELEM:" + QString::number(indx);
    if (rootItem == currentItem)
    {
        ui->toolButtonMoveDown->setEnabled(false);
        ui->toolButtonMoveUp->setEnabled(false);
    }
    else
    {
        ui->toolButtonMoveUp->setEnabled(indx != 0);
        ui->toolButtonMoveDown->setEnabled(indx != rootItem->childCount()-1);
    }
    if (mPageCache.contains(currentItem)) //если уже создали страницу для символа
    {
        ui->stackedWidget->setCurrentWidget(mPageCache[currentItem]);
        return;
    }

    switch (currentSymbol->type())
    {
    case SymbolType::SimplePointSymbol:
    {
        mPageCache[currentItem] = createSimplePointPage(currentSymbol);
        break;
    }
    case SymbolType::SimpleLineSymbol:
    {
        mPageCache[currentItem] = createSimpleLinePage(currentSymbol);
        break;
    }
    case SymbolType::SimplePolygonSymbol:
    {
        mPageCache[currentItem] = createSimplePolygonPage(currentSymbol);
        break;
    }
    default:
        break;
    }

}




void VectorStylingForm::on_toolButtonMoveUp_clicked()
{
    auto rootItem = ui->treeWidget->topLevelItem(0);
    auto currentItem = ui->treeWidget->currentItem();
    auto currentIndx = rootItem->indexOfChild(currentItem);
    if (currentIndx == 0)
        return;
    auto prevIndx = currentIndx-1;
    QTreeWidgetItem* temp1 = rootItem->takeChild(currentIndx);
    QTreeWidgetItem* temp2 = rootItem->takeChild(prevIndx);
    if (!temp1 || !temp2)
        return;
    auto currentSymbol = temp1->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    auto symbolToSwap = temp2->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    if (!currentSymbol || !symbolToSwap)
        return;
    auto lS = static_cast<vrsa::graphics::LayerSymbol*>(mClonedSymbol.get());
    lS->swapChildren(lS->indexOf(currentSymbol), lS->indexOf(symbolToSwap));

    rootItem->insertChild(prevIndx, temp1);
    rootItem->insertChild(currentIndx, temp2);
    ui->treeWidget->setCurrentItem(temp1);
    update();
}


void VectorStylingForm::on_toolButtonMoveDown_triggered(QAction *arg1)
{
}


void VectorStylingForm::on_toolButtonMoveDown_clicked()
{
    auto rootItem = ui->treeWidget->topLevelItem(0);
    auto currentItem = ui->treeWidget->currentItem();
    auto currentIndx = rootItem->indexOfChild(currentItem);
    if (currentIndx == ui->treeWidget->topLevelItem(0)->childCount()-1)
        return;
    auto nextIndx = currentIndx + 1;



    QTreeWidgetItem* temp1 = rootItem->takeChild(currentIndx);
    QTreeWidgetItem* temp2 = rootItem->takeChild(nextIndx-1);

    auto currentSymbol = temp1->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    auto symbolToSwap = temp2->data(DATA_COLUMN, vrsa::common::SymbolPtrRole).value<Symbol*>();
    auto lS = static_cast<vrsa::graphics::LayerSymbol*>(mClonedSymbol.get());
    lS->swapChildren(lS->indexOf(currentSymbol), lS->indexOf(symbolToSwap));

    rootItem->insertChild(currentIndx, temp2);
    rootItem->insertChild(nextIndx, temp1);
    ui->treeWidget->setCurrentItem(temp1);
    update();
}


void VectorStylingForm::on_toolButtonSaveSymbol_clicked()
{

    //*mSymbol = *(mClonedSymbol->clone().get());
    switch(mSymbol->type())
    {
    case vrsa::common::SymbolType::LayerPointSymbol:
    {
        auto LPS_orig = static_cast<vrsa::graphics::LayerPointSymbol*>(mSymbol);
        auto LPS_cloned = static_cast<vrsa::graphics::LayerPointSymbol*>(mClonedSymbol.get());
        *LPS_orig = *LPS_cloned;
        break;
    }
    case vrsa::common::SymbolType::LayerLineSymbol:
    {
        auto LLS_orig = static_cast<vrsa::graphics::LayerLineSymbol*>(mSymbol);
        auto LLS_cloned = static_cast<vrsa::graphics::LayerLineSymbol*>(mClonedSymbol.get());
        *LLS_orig = *LLS_cloned;
        break;
    }
    case vrsa::common::SymbolType::LayerPolygonSymbol:
    {
        auto LPS_orig = static_cast<vrsa::graphics::LayerPolygonSymbol*>(mSymbol);
        auto LPS_cloned = static_cast<vrsa::graphics::LayerPolygonSymbol*>(mClonedSymbol.get());
        *LPS_orig = *LPS_cloned;
        break;
    }
    default:
        break;
    }


}

