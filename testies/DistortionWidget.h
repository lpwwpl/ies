//#ifndef DISTORTIONGRIDWIDGET_H
//#define DISTORTIONGRIDWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include <QPointF>
//#include "qcustomplot.h"
//
//struct DistortionPoint {
//    double relXFld;    // 相对X视场
//    double relYFld;    // 相对Y视场
//    double paraxX;     // 理想X坐标
//    double paraxY;     // 理想Y坐标
//    double realX;      // 实际X坐标
//    double realY;      // 实际Y坐标
//    double radDist;    // 径向畸变
//    double tanDist;    // 切向畸变
//};
//
//class DistortionGridWidget : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit DistortionGridWidget(QWidget* parent = nullptr);
//
//    // 公共接口
//    bool loadDistortionData(const QString& filename);
//    void plotGrids();
//
//private:
//    void setupPlot();
//    void organizeGridData();
//    void plotIdealGrid();
//    void plotDistortedGrid();
//    void updateInfoText();
//
//    QCustomPlot* m_customPlot;
//
//    QVector<DistortionPoint> m_distortionData;
//    QVector<QVector<DistortionPoint>> m_gridData; // 11x11网格数据
//
//    double m_maxRadialDistortion = 0.0;
//    double m_maxTangentialDistortion = 0.0;
//
//    double xMin = INFINITY;
//    double yMin = INFINITY;
//    double xMax = -INFINITY;
//    double yMax = -INFINITY;
//
//    double xRange = 0;
//    double yRange = 0;
//    double marginX = 0;
//    double marginY = 0;
//};
//
//#endif // DISTORTIONGRIDWIDGET_H
#ifndef DISTORTIONGRIDWIDGET_H
#define DISTORTIONGRIDWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QColor>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_point_data.h>
#include <qwt_legend.h>
#include "PlotSetting.h"
#include "QwtPropertyBrowser.h"
#include "PlotBase.h"

class QSplitter;
struct DistortionPoint {
    double relXFld;    // 相对X视场
    double relYFld;    // 相对Y视场
    double paraxX;     // 理想X坐标
    double paraxY;     // 理想Y坐标
    double realX;      // 实际X坐标
    double realY;      // 实际Y坐标
    double radDist;    // 径向畸变
    double tanDist;    // 切向畸变
};

class DistortionGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DistortionGridWidget(QWidget* parent = nullptr);
    ~DistortionGridWidget();

    // 公共接口
    bool loadDistortionData(const QString& filename);
    void plotGrids();
    void autoScaleAxes();
    void saveInitialView();

public Q_SLOTS:
    void zoomIn();
    void zoomOut();
    void fitView();
    void updateXScaleAxes();
    void updateYScaleAxes();
private:
    void setupPlot();
    void setupInteractions();
    void organizeGridData();
    void plotIdealGrid(int&);
    void plotDistortedGrid(int&);
    void updateInfoText();
    void clearAllCurves();

    QwtPlot* m_plot;

    // 交互对象
    QwtPlotPanner* m_panner;
    QwtPlotMagnifier* m_magnifier;

    QVector<DistortionPoint> m_distortionData;
    QVector<QVector<DistortionPoint>> m_gridData; // 11x11网格数据

    // 存储曲线对象
    QList<QwtPlotCurve*> m_idealCurves;
    QList<QwtPlotCurve*> m_distortedCurves;

    double m_maxRadialDistortion = 0.0;
    double m_maxTangentialDistortion = 0.0;

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

    PlotSettings* m_settings;
    PlotBase* m_toolBar_plot;
    QwtPlotGrid* m_grid;
    //QwtLegend* m_legend;
    QToolBar* m_toolBar;
    QwtPropertyBrowser* m_simple_browser;
    QSplitter* m_splitter;

    double m_current_factor = 1;
    double m_initialXMin, m_initialXMax;        // 初始X轴范围
    double m_initialYMin, m_initialYMax;        // 初始Y轴范围

    double m_initialXMin_orig, m_initialXMax_orig;        // 初始X轴范围
    double m_initialYMin_orig, m_initialYMax_orig;        // 初始Y轴范围
};

#endif // DISTORTIONGRIDWIDGET_H