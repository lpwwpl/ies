#ifndef THIRDABERRATIONPLOT_H
#define THIRDABERRATIONPLOT_H

#include <QWidget>
#include <vector>
#include <QStringList>
#include <qwt_plot.h>
#include <qwt_plot_multi_barchart.h>
#include "PlotSetting.h"
#include "QwtPropertyBrowser.h"
#include "PlotBase.h"
class QSplitter;

class ThirdAberrationPlot : public QWidget
{
    Q_OBJECT
public:
    explicit ThirdAberrationPlot(QWidget* parent = nullptr);
    ~ThirdAberrationPlot();

    bool loadFile(const QString& fileName);
    void clearPlot();

    void autoScaleAxes();
    void saveInitialView();
    void setupInteractions();
public Q_SLOTS:
    void zoomIn();
    void zoomOut();
    void fitView();
    void updateXScaleAxes();
    void updateYScaleAxes();
    void updateAxesSettings();
    void updateX();
    void updateY();
    void updateXY() { updateX(); updateY(); }
    void slotDisplayProperties(int);
private:
    void setupPlot();
    bool parseFile(const QString& fileName);
    void createBarChart();

    std::vector<std::vector<double>> m_seriesData; // [surfaceIndex][aberIdx]
    std::vector<int> m_surfaceNumbers;
    QStringList m_aberrationNames; // 存储9个像差的中文名作为图例
    QwtPlotMultiBarChart* m_multiBarChart;


    // 交互对象
    MyPlotPanner* m_panner;
    MyPlotMagnifier* m_magnifier;


    double xMin = INFINITY;
    double yMin = INFINITY;
    double xMax = -INFINITY;
    double yMax = -INFINITY;

    double xRange = 0;
    double yRange = 0;
    double marginX = 0;
    double marginY = 0;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    QwtPlot* m_plot;
    PlotSettings* m_settings;
    PlotBase* m_toolBar_plot;
    QwtPlotGrid* m_grid;

    QToolBar* m_toolBar;
    QwtPropertyBrowser* m_simple_browser;
    QSplitter* m_splitter;

    double m_current_factor = 1;
    double m_initialXMin, m_initialXMax;        // 初始X轴范围
    double m_initialYMin, m_initialYMax;        // 初始Y轴范围

    double m_initialXMin_orig, m_initialXMax_orig;        // 初始X轴范围
    double m_initialYMin_orig, m_initialYMax_orig;        // 初始Y轴范围

};

#endif // THIRDABERRATIONPLOT_H