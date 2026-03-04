#ifndef QWTPROPERTYBROWSER_H
#define QWTPROPERTYBROWSER_H

#include <QWidget>


#include "qtproperty/qtpropertymanager.h"
#include "qtproperty/qteditorfactory.h"
#include "qtproperty/qtbuttonpropertybrowser.h"
#include "qtproperty/qtgroupboxpropertybrowser.h"

#include "qtproperty/qtpropertybrowser.h"
#include "qtproperty/qtpropertybrowserutils_p.h"
#include "qtproperty/qtvariantproperty.h"
#include "qtproperty/qttreepropertybrowser.h"


#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "PlotSetting.h"
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtLegend;

class QwtPropertyBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QwtPropertyBrowser(PlotSettings* settings, QwtPlot* plot, QwtPlotGrid* grid,QwtLegend* legend, QWidget* parent = nullptr);
    ~QwtPropertyBrowser();


    QwtPlot* plot() const { return m_plot; }

signals:
    void propertyChanged();

private slots:
    //property属性更改，则更新m_settings和plot
    void onValueChanged(QtProperty* property, const QVariant& value);

public:
    void createGlobalProperties();
    void createAxisProperties();
    void createCurveProperties();

    
    //只更新plot
    void updateCurveStyle(int index);
    void updateLegendItemsStyle();
    //property属性更改,进而更新m_settings和plot
    void updateLineCombo();
    void loadLineSettings(int index);
    void setCurrentCurve(int);
    void applyXAxisSettings();
    void applyYAxisSettings();
    void applyBackgroundColor();
    void applyGridSettings();
    void applyTitleSettings();
    void applyLegendSettings();
    void applyOrigin();

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

    //更新property进而更新m_settings和plot
    void InitSetupUI();
    QVector<qreal> parseDashPattern(const QString& text);
public:
    QwtPlot* m_plot;
    //QwtPlotCurve* m_currentCurve;
    QwtPlotGrid* m_grid;
    QwtLegend* m_legend;
    PlotSettings* m_settings;

    QtVariantPropertyManager* m_manager;
    QtTreePropertyBrowser* m_browser;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    QtVariantProperty* group_global;
    QtVariantProperty* group_axis;
    QtVariantProperty* group_curve;
    // 全局属性
    QtVariantProperty* m_gridVisibleProperty;
    QtVariantProperty* m_originProperty;
    QtVariantProperty* m_titleProperty;
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
    QStringList m_CurveTitles;
    QtVariantProperty* m_curveTitleProperty;
    QtVariantProperty* m_curveVisibleProperty;
    QtVariantProperty* m_curveLineColorProperty;
    QtVariantProperty* m_curveLineCurveProperty;
    QtVariantProperty* m_curveLineWidthProperty;
    QtVariantProperty* m_curveLineStyleProperty;
    QtVariantProperty* m_curveSymbolStyleProperty;
    QtVariantProperty* m_curveSymbolSizeProperty;
    QtVariantProperty* m_curveSymbolColorProperty;
    QtVariantProperty* m_curveCustomPatternProperty;
    //QtVariantProperty* m_curveSymbolFilledProperty;
};

#endif // QWTPROPERTYBROWSER_H