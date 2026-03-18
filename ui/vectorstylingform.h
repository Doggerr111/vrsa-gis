#ifndef VECTORSTYLINGFORM_H
#define VECTORSTYLINGFORM_H

#include <QDialog>
#include <QGraphicsItem>
#include "graphics/symbols/symbolrenderer.h"

class QTreeWidgetItem;
namespace vrsa{
namespace common{
enum class GeometryType: int;
}
namespace vector{
class VectorLayer;
}
namespace graphics{
class Symbol;
class SymbolRenderer;
}
}
namespace Ui {
class VectorStylingForm;
}

class VectorStylingForm : public QDialog
{
    Q_OBJECT
    using StyleParam = vrsa::common::StyleParametr;
    using Symbol = vrsa::graphics::Symbol;
    using SymbolType = vrsa::common::SymbolType;
    using LayerGeometryType = vrsa::common::GeometryType;
    using VectorLayer = vrsa::vector::VectorLayer;

public:
    explicit VectorStylingForm(VectorLayer* layer, QWidget *parent = nullptr);
#ifdef VRSA_ENABLE_TEST_UTILS
    explicit VectorStylingForm(int testCase, QWidget *parent = nullptr);
#endif

    ~VectorStylingForm();
private:
    void initialize();
    void update();
    void setStackedWidgetPage();

    //treeWidget settings
    QTreeWidgetItem* createItem(Symbol *symbol);
    void buildSymbolTree();
    void populateTree(Symbol *symbol, QTreeWidgetItem* parentItem);
    QTreeWidgetItem* findItemByData(const QVariant& data, int role = Qt::UserRole);
    QTreeWidgetItem* findItemInChildren(QTreeWidgetItem* parent, const QVariant& data, int role);

    //methods for creating ui
    //pages for stacked widget
    QWidget* createSimplePointPage(Symbol* symbol);
    QWidget* createSimpleLinePage(Symbol* symbol);
    QWidget* createSimplePolygonPage(Symbol* symbol);
    //components
    QWidget* addSpinBoxFrame(const QString& labelText, Symbol* symbol, const StyleParam param);
    QWidget* addColorBtnFrame(Symbol* symbol, const StyleParam param);
    QWidget* addComboBoxFrame(const QString& labelText, Symbol* symbol, const StyleParam param);
    QWidget* addSliderFrame(const QString& labelText, Symbol* symbol, const StyleParam param);

private slots:
    void on_toolButtonAddSymbol_clicked();
    void on_toolButtonDeleteSymbol_clicked();
    void on_toolButtonCopySymbol_clicked();
    void on_toolButtonMoveUp_clicked();
    void on_toolButtonMoveDown_triggered(QAction *arg1);
    void on_toolButtonMoveDown_clicked();
    void on_toolButtonSaveSymbol_clicked();

    void om_TreeWidgetCurretItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

    //for dynamic ui
    void on_SpinBoxValueChanged(double value);
    void on_ColorBtnClicked();
    void on_PenStyleCBoxIndexChanged(int indx);
    void on_BrushStyleCBoxIndexChanged(int indx);
    void on_PenJoinCBoxIndexChanged(int indx);
    void on_PenCapCBoxIndexChanged(int indx);
    void on_PointTypeCBoxIndexChanged(int indx);


signals:
    void symbolRenderRequired(Symbol*, QIcon&);
    void itemRenderRequired(Symbol*);

private:
    Ui::VectorStylingForm *ui;
    Symbol* mSymbol;
    vrsa::vector::VectorLayer* mLayer;
    std::unique_ptr<Symbol> mClonedSymbol;
    LayerGeometryType mGeomType;
    QGraphicsScene* mScene;

    //кэш созданных страниц
    QMap<QTreeWidgetItem*, QWidget*> mPageCache;
    QTreeWidgetItem* mLastSelectedItem;

    vrsa::graphics::SymbolRenderer mSymbolRenderer;

    static constexpr int pointPageIndx = 0;
    static constexpr int linePageIndx = 0;
    static constexpr int polygonPageIndx = 0;

    static constexpr int DATA_COLUMN = 0;
    static constexpr int TEXT_COLUMN = 0;
};

#endif // VECTORSTYLINGFORM_H
