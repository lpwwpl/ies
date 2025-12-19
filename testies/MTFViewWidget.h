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

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_picker_machine.h>
#include <qwt_text.h>

struct MTFLine {
    int index = 0;
    QString label;
    QString colorName;
    bool visible = true;
    bool selected = false;
    QVector<QPointF> data;
    QwtPlotCurve* curve = nullptr;
};

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
    void resetView();
    void setTitle(const QString& title);

    // 图例控制
    void setLegendPosition(bool leftSide);
    void showLegend(bool show);

    QString currentFile() const { return m_currentFile; }

signals:
    void dataLoaded(bool success);
    void errorOccurred(const QString& message);

private slots:
    void onLegendChecked(const QVariant& itemInfo, bool on,int);
    void onPlotClicked(const QPointF& pos);
    void onMouseDoubleClick();
    void showContextMenu(const QPoint& pos);

private:
    void setupUI();
    void setupInteractions();
    void plotData();
    QColor getColorFromName(const QString& colorName);
    void updateCurveSelection(int curveIndex, bool selected);
    void clearAllSelections();
    void updateLegendSelection(QwtPlotCurve* curve, bool selected);
    void autoScaleAxes();

    QwtPlot* m_plot;
    QString m_currentFile;
    QVector<MTFLine> m_lines;

    // 交互对象
    QwtPlotPanner* m_panner;
    QwtPlotMagnifier* m_magnifier;
    QwtPlotPicker* m_picker;

    // 默认视图范围
    double m_defaultXMin = 0.0;
    double m_defaultXMax = 200.0;
    double m_defaultYMin = 0.0;
    double m_defaultYMax = 1.1;

    // 选择状态
    int m_currentSelectedIndex = -1;

    // 默认颜色列表
    QVector<QColor> m_defaultColors;
    QwtLegend* m_legend;
};

#endif // MTFVIEWWIDGET_H