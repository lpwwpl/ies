//#ifndef FIELDVIEWWIDGET_H
//#define FIELDVIEWWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include "qcustomplot.h"
//
//struct FieldData {
//    int index;
//    double xField;
//    double yField;
//    double magnitude;
//    double angle;
//};
//
//class FieldViewWidget : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit FieldViewWidget(QWidget* parent = nullptr);
//    ~FieldViewWidget();
//
//    bool loadFieldDataFile(const QString& filename);
//    void clearData();
//    void resetView();
//
//private:
//    void setupFieldViewPlot();
//    void updatePlot();
//    void drawFieldArrows();
//    QPen getArrowPen(double magnitude);
//    double getArrowLength(double magnitude);
//
//    QCustomPlot* customPlot;
//    QVector<FieldData> fieldData;
//    double maxMagnitude;
//    double minMagnitude;
//
//    // 默认视图范围
//    double m_defaultRange = 50.0;
//};
//
//#endif // FIELDVIEWWIDGET_H
#ifndef FIELDVIEWWIDGET_H
#define FIELDVIEWWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QColor>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_map.h>

struct FieldData {
    int index;
    double xField;
    double yField;
    double magnitude;
    double angle;
};

class ArrowItem : public QwtPlotItem
{
public:
    ArrowItem(double x, double y, double angle, double length,
        const QColor& color, double width)
        : m_x(x), m_y(y), m_angle(angle), m_length(length)
        , m_color(color), m_width(width)
    {
        setZ(100); // 设置较高的Z值确保箭头在最前面
    }

    virtual void draw(QPainter* painter,
        const QwtScaleMap& xMap, const QwtScaleMap& yMap,
        const QRectF& canvasRect) const override;

private:
    double m_x, m_y;
    double m_angle;
    double m_length;
    QColor m_color;
    double m_width;
};

class FieldViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FieldViewWidget(QWidget* parent = nullptr);
    ~FieldViewWidget();

    bool loadFieldDataFile(const QString& filename);
    void clearData();
    void resetView();

private:
    void setupFieldViewPlot();
    void setupInteractions();
    void updatePlot();
    void drawFieldArrows();
    void clearAllArrows();
    QPen getArrowPen(double magnitude);
    double getArrowLength(double magnitude);
    QColor getArrowColor(double magnitude);

    QwtPlot* m_plot;

    // 交互对象
    QwtPlotPanner* m_panner;
    QwtPlotMagnifier* m_magnifier;

    // 存储箭头对象
    QList<ArrowItem*> m_arrowItems;

    QVector<FieldData> fieldData;
    double maxMagnitude;
    double minMagnitude;

    // 默认视图范围
    double m_defaultRange = 50.0;
};

#endif // FIELDVIEWWIDGET_H