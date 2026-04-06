#pragma once

#include <QWidget>


#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

#include "qtpropertybrowser.h"
#include "qtpropertybrowserutils_p.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"


#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "PlotSetting.h"
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtLegend;
class StringEditorFactory;

class QwtPropertyBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QwtPropertyBrowser(PlotSettings* settings, QwtPlot* plot, QwtPlotGrid* grid,QwtLegend* legend, QWidget* parent = nullptr);
    ~QwtPropertyBrowser();


    QwtPlot* plot() const { return m_plot; }

signals:
    void propertyChanged();
    void signalUpdateItemStyle(int);
    void signalXScaleAxes();
    void signalYScaleAxes();
    void signalUpdateScaleDiv();

private slots:
    //property属性更改，则更新m_settings和plot
    void onValueChanged(QtProperty* property, const QVariant& value);
    void onValueChanged(QtProperty* property, const QString& value);
public:
    void createGlobalProperties();
    void createAxisProperties();
    void createCurveProperties();
    void createPlotItemProperties();
    
    //只更新plot
    void updateCurveStyle(int index);
    void updateLegendItemsStyle();
    //property属性更改,进而更新m_settings和plot
    void updateLineCombo();
    void loadLineSettings(int index);
    void loadGroupSettings(int index);
    void setCurrentCurve(int);
    void applyXAxisSettings();
    void applyYAxisSettings();
    void applyBackgroundColor();
    void applyGridSettings();
    void applyTitleSettings();
    void applyLegendSettings();
    void applyOrigin();


    void updateItemCombo();
    void loadItemSettings(int index);
    void loadItemGroupSettings(int index);
    void setCurrentItem(int);
    void updateItemStyle(int index);

    //只更新plot
    void applyXAxisSettings_plot();
    void applyYAxisSettings_plot();
    void applyBackgroundColor_plot();
    void applyGridSettings_plot();
    void applyTitleSettings_plot();
    void applyLegendSettings_plot();
    void applyOrigin_plot();

    //更新plot和m_settings
    void autoXScaleAxes();
    void autoYScaleAxes();

    //void updateGroupLine()
    //{

    //}
    //更新property进而更新m_settings和plot
    void InitSetupUI();
    QVector<qreal> parseDashPattern(const QString& text);
public:
    QwtPlot* m_plot;
    //QwtPlotCurve* m_currentCurve;
    QwtPlotGrid* m_grid;
    QwtLegend* m_legend;
    PlotSettings* m_settings;
    QtStringPropertyManager* m_stringManager;
    QtVariantPropertyManager* m_manager;
    QtTreePropertyBrowser* m_browser;

    StringEditorFactory* m_stringEditorFactory;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    QtVariantProperty* group_global;
    QtVariantProperty* group_axis;
    QtVariantProperty* group_curve;
    QtVariantProperty* group_item;
    // 全局属性
    QtVariantProperty* m_gridVisibleProperty;
    QtVariantProperty* m_originProperty;
    QtProperty* m_titleProperty;
    QtVariantProperty* m_titleFontProperty;
    QtVariantProperty* m_titleColorProperty;
    QtVariantProperty* m_backgroundColorProperty;
    //QtVariantProperty* m_canvasBackgroundProperty;
    QtVariantProperty* m_legendVisibleProperty;
    QtVariantProperty* m_legendPositionProperty;
    QtVariantProperty* m_legendFontProperty;
    QtVariantProperty* m_legendColorProperty;

    // 坐标轴属性
    struct AxisProperties {
        QtVariantProperty* visibleProperty;
        QtVariantProperty* titleProperty;
        QtVariantProperty* titleFontProperty;
        QtVariantProperty* titleColorProperty;
        QtVariantProperty* autoRangeProperty;
        QtVariantProperty* minProperty;
        QtVariantProperty* maxProperty;
        QtVariantProperty* stepProperty;
        QtVariantProperty* tickFontProperty;
        QtVariantProperty* tickColorProperty;
        //QtVariantProperty* scaleTypeProperty;
    };
    QMap<int, AxisProperties> m_axisProps;

    // 曲线属性
    QMap<int,QString> m_CurveTitles;
    QStringList m_CurveGroups;
    QtVariantProperty* m_curveGroupEditableProperty;
    QtVariantProperty* m_curveTitleProperty;
    QtVariantProperty* m_curveVisibleProperty;
    QtVariantProperty* m_curveLineColorProperty;
    QtVariantProperty* m_curveLineCurveProperty;
    QtVariantProperty* m_curveLineWidthProperty;
    QtVariantProperty* m_curveLineStyleProperty;
    QtVariantProperty* m_curveSymbolStyleProperty;
    QtVariantProperty* m_curveSymbolSizeProperty;
    QtVariantProperty* m_curveGroupProperty;
    QtVariantProperty* m_curveSymbolColorProperty;
    QtVariantProperty* m_curveCustomPatternProperty;

    QStringList m_itemGroups;
    QMap< int, QString> m_itemTitles;
    QtVariantProperty* m_itemGroupEditableProperty;
    QtVariantProperty* m_itemTitleProperty;
    QtVariantProperty* m_itemVisiableProperty;
    QtVariantProperty* m_itemGroupProperty;
    QtVariantProperty* m_itemXProperty;
    QtVariantProperty* m_itemYProperty;
    QtVariantProperty* m_itemAngleProperty;
    QtVariantProperty* m_itemLengthProperty;
    QtVariantProperty* m_itemColorProperty;
    QtVariantProperty* m_itemWidthProperty;
    //QtVariantProperty* m_curveSymbolFilledProperty;
};

