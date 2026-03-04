//#ifndef MTFVIEWWIDGET_H
//#define MTFVIEWWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include <QPointF>
//#include <QString>
//#include "qcustomplot.h"
////class QCustomPlot;
////class QCPTextElement;
////class QCPGraph;
//
//struct MTFLine {
//    int index = 0;
//    QString label;
//    QString colorName;
//    bool visible = true;
//    bool selected = false;
//    QVector<QPointF> data;
//    QCPGraph* graph = nullptr;
//};
//
//class MTFViewer : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit MTFViewer(QWidget* parent = nullptr);
//
//    // 数据加载
//    bool loadFromFile(const QString& filename);
//    bool loadFromString(const QString& data);
//    void clearData();
//
//    // 视图控制
//    void setXRange(double min, double max);
//    void setYRange(double min, double max);
//    void resetView();
//    void setTitle(const QString& title);
//
//    // 图例控制
//    void setLegendPosition(bool leftSide);
//    void showLegend(bool show);
//
//    QString currentFile() const { return m_currentFile; }
//
//signals:
//    void dataLoaded(bool success);
//    void errorOccurred(const QString& message);
//
//private slots:
//    void onLegendClick(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event);
//    void onSelectionChanged();
//    void onMouseDoubleClick(QMouseEvent* event);
//    void onPlottableClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);
//
//private:
//    void setupUI();
//    void plotData();
//    QColor getColorFromName(const QString& colorName);
//    void updateGraphSelection(int graphIndex, bool selected);
//    void clearAllSelections();
//
//    QCustomPlot* m_plot;
//    QString m_currentFile;
//    QVector<MTFLine> m_lines;
//
//    // 默认视图范围
//    double m_defaultXMin = 0.0;
//    double m_defaultXMax = 200.0;
//    double m_defaultYMin = 0.0;
//    double m_defaultYMax = 1.1;
//
//    // 选择状态
//    int m_currentSelectedIndex = -1;
//};
//
//#endif // MTFVIEWWIDGET_H
#ifndef MTFVIEWWIDGET_H
#define MTFVIEWWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QColor>
#include <QMap>
#include <QToolBar>
#include <QSplitter>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_picker_machine.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include "PlotSetting.h"
#include "QwtPropertyBrowser.h"
#include "SimplePropertyBrowser.h"


#include "PlotBase.h"

class MTFViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MTFViewer(QWidget* parent = nullptr);
    ~MTFViewer();

    // 数据加载
    bool loadFromFile(const QString& filename);
    bool loadFromString(const QString& data);
    void clearData();

    // 视图控制
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    void setTitle(const QString& title);
    void setLegendPosition(bool leftSide);
    void showLegend(bool show);

    // 自动缩放
    void autoScaleAxes();


signals:
    void dataLoaded(bool success);
    void errorOccurred(const QString& message);

private slots:
    void onPlotClicked(const QPointF& pos);
    void updateLegendSelection(QwtPlotCurve* curve, bool selected);
    bool eventFilter(QObject* object, QEvent* event);
    void onLegendChecked(const QVariant& itemInfo, bool on, int);
    void showContextMenu(const QPoint& pos);

public Q_SLOTS:
    void zoomIn();
    void zoomOut();
    void fitView();

private:
    void setupUI();
    void setupInteractions();
    void plotData();
    QColor getColorFromName(const QString& colorName);
    void updateCurveSelection(int curveIndex, bool selected);
    void clearAllSelections();


    QwtPlot* m_plot;
    QString m_currentFile;

    QwtPlotGrid* m_grid;
    QwtLegend* m_legend;
    QToolBar* m_toolBar;
    //QwtPropertyBrowser* m_browser;
    //SimplePropertyBrowser* m_simple_browser;
    QwtPropertyBrowser* m_simple_browser;
    QSplitter* m_splitter;

    // 交互对象
    QwtPlotPanner* m_panner;
    QwtPlotMagnifier* m_magnifier;
    QwtPlotPicker* m_picker;

    // 默认范围
    double m_defaultXMin = 0.0, m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0, m_defaultYMax = 1.1;

    int m_currentSelectedIndex = -1;
    QVector<QColor> m_defaultColors;

    PlotSettings* m_settings;
    PlotBase* m_toolBar_plot;
};

#endif // MTFVIEWWIDGET_H