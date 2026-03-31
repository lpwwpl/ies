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


    QwtPlotGrid* m_grid;
    //QwtLegend* m_legend;
    QToolBar* m_toolBar;
    QwtPropertyBrowser* m_simple_browser;
    QSplitter* m_splitter;
    PlotSettings* m_settings;
    PlotBase* m_toolBar_plot;
};

#endif // FOOTPRINTWIDGET_H