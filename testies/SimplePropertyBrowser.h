#ifndef SIMPLEPROPERTYBROWSER_H
#define SIMPLEPROPERTYBROWSER_H

#include <QWidget>
#include "PlotSetting.h"
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtLegend;

class QLineEdit;
class QCheckBox;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QSpinBox;

class SimplePropertyBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit SimplePropertyBrowser(PlotSettings* settings,QwtPlot* plot, QwtPlotGrid* grid, QwtLegend* legend, QWidget* parent = nullptr);
    ~SimplePropertyBrowser();

    void setCurrentCurve(int);
    QwtPlotCurve* currentCurve() const { return m_currentCurve; }

public:
    void initSimplePropertyWidget();

Q_SIGNALS:
    void signalXScaleAxes();
    void signalYScaleAxes();
    void signalUpdateScaleDiv();

public:
    void applyBackgroundColor();
    void applyXAxisSettings();
    void applyYAxisSettings();
    void applyGridSettings();
    void applyTitleSettings();
    void applyLegendSettings();
    void applyOrigin();
    void updateLegendItemsStyle();
    void updateCurveStyle(int index);
    void updateAllCurvesStyle();
    void updateLineCombo();

    void autoXScaleAxes();
    void autoYScaleAxes();
    QVector<qreal> parseDashPattern(const QString& text);
public slots:
    void loadLineSettings(int index);

public:
    QwtPlot* m_plot;
    QwtPlotCurve* m_currentCurve;
    QwtPlotGrid* m_grid;
    QwtLegend* m_legend;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    PlotSettings* m_settings;
    //PlotSettings m_settings_cache;

    QLineEdit* titleEdit;
    QComboBox* originCombo;
    QCheckBox* gridVisibleCheck;
    QLabel* bgColorLabel;
    QLabel* titleFontLabel;
    QLabel* titleColorLabel;

    QCheckBox* legendVisibleCheck;
    QComboBox* legendPosCombo;
    QLabel* legendFontLabel;
    QLabel* legendColorLabel;

    QCheckBox* xVisible;
    QLineEdit* xTitle;
    QLabel* xTitleFontLabel;
    QLabel* xTitleColorLabel;
    QCheckBox* xAutoRange;
    QDoubleSpinBox* xMin;
    QDoubleSpinBox* xMax;
    QDoubleSpinBox* xStep;
    QLabel* xTickFontLabel;
    QLabel* xTickColorLabel;

    QCheckBox* yVisible;
    QLineEdit* yTitle;
    QLabel* yTitleFontLabel;
    QLabel* yTitleColorLabel;
    QCheckBox* yAutoRange;
    QDoubleSpinBox* yMin;
    QDoubleSpinBox* yMax;
    QDoubleSpinBox* yStep;
    QLabel* yTickFontLabel;
    QLabel* yTickColorLabel;

    QComboBox* lineCombo;
    QLineEdit* customDashEdit;
    QDoubleSpinBox* lineWidthSpin;
    QLabel* lineColorLabel;
    QDoubleSpinBox* pointWidthSpin;
    QDoubleSpinBox* pointSizeSpin;
    QLabel* pointColorLabel;
    QComboBox* lineCurveCombo;
    QComboBox* pointStyleCombo;
    QComboBox* lineStyleCombo;

    QWidget* globalTab;
    QWidget* axisTab;
    QWidget* curveTab;
};

#endif // QWTPROPERTYBROWSER_H