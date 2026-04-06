//#ifndef FOOTPRINTWIDGET_H
//#define FOOTPRINTWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include <QString>
//#include <QFile>
//#include <QTextStream>
//#include <QVBoxLayout>
//#include "qcustomplot.h"
//
//struct FootprintData {
//    QString field;
//    double left;
//    double right;
//    double top;
//    double bottom;
//    double radius;
//};
//
//class FootprintWidget : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit FootprintWidget(QWidget* parent = nullptr);
//    bool loadDataFromFile(const QString& filename);
//    void plotFootprints();
//
//private:
//    QCustomPlot* m_customPlot;
//    QVector<FootprintData> m_footprintData;
//
//    void setupPlot();
//    QColor getFieldColor(const QString& field);
//};
//
//#endif // FOOTPRINTWIDGET_H
#ifndef FOOTPRINTWIDGET_H
#define FOOTPRINTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QMap>
#include <QColor>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>


#include "PlotSetting.h"
#include "QwtPropertyBrowser.h"
#include "PlotBase.h"
class QSplitter;

struct FootprintData {
    QString field;
    double left;
    double right;
    double top;
    double bottom;
    double radius;
};

class FootprintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FootprintWidget(QWidget* parent = nullptr);
    ~FootprintWidget();

    bool loadDataFromFile(const QString& filename);
    void plotFootprints();
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
    void clearAllCurves();
    QColor getFieldColor(const QString& field);
    QVector<QPointF> createCirclePoints(double centerX, double centerY, double radius, int segments = 100);
    QVector<QPointF> createRectanglePoints(double left, double right, double top, double bottom);

    QwtPlot* m_plot;

    // 交互对象
    QwtPlotPanner* m_panner;
    QwtPlotMagnifier* m_magnifier;

    // 存储曲线对象
    QList<QwtPlotCurve*> m_circleCurves;
    QList<QwtPlotCurve*> m_legendCurves;

    QVector<FootprintData> m_footprintData;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    QwtPlotGrid* m_grid;
    //QwtLegend* m_legend;
    QToolBar* m_toolBar;
    QwtPropertyBrowser* m_simple_browser;
    QSplitter* m_splitter;
    PlotSettings* m_settings;
    PlotBase* m_toolBar_plot;


    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double maxRadius = 0;


    double m_current_factor = 1;
    double m_initialXMin, m_initialXMax;        // 初始X轴范围
    double m_initialYMin, m_initialYMax;        // 初始Y轴范围

    double m_initialXMin_orig, m_initialXMax_orig;        // 初始X轴范围
    double m_initialYMin_orig, m_initialYMax_orig;        // 初始Y轴范围
};

#endif // FOOTPRINTWIDGET_H