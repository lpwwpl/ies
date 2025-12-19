//// IESSpotLightWidget.h
//#pragma once
//#include <QWidget>
//#include "QCustomPlot.h"
//#include "tiny_ies.hpp"
//
//
//class IESPolarWidget:public QCustomPlot
//{
//	Q_OBJECT
//public:
//	IESPolarWidget();
//	~IESPolarWidget();
//	void updateIES(double angle=0);
//	
//	
//	void Init();
//
//
//	std::vector<QPointF> generateC0C180Profile(double angle=0);  //c0-c180
//	std::vector<QPointF> generateC90C270Profile(double angle = 0); //c90 - c270
//	//void addContourLines(QCPColorMap* colorMap, const QVector<double>& isoLevels);
//
//public Q_SLOTS:
//	void on_chkFillBlue_stateChanged(int value);
//	void on_chkFillRed_stateChanged(int);
//	void on_chkViewRed_stateChanged(int);
//
//	void on_chkFillGreen_stateChanged(int);
//	void on_chkFillYellow_stateChanged(int);
//	void on_chkViewYellow_stateChanged(int);
//
//	void on_horizontalSlider_valueChanged(int);
//	void on_horizontalSlider_2_valueChanged(int);
//public:
//	QCPPolarAxisRadial* radialAxis = nullptr;
//	QCPPolarAxisAngular* angularAxis = nullptr;
//	QCPPolarGraph* graph0_180 = nullptr;
//	QCPPolarGraph* graph90_270 = nullptr;
//
//
//	//tiny_ies<double>::light light;
//	//QCPLayer* backgroundLayer;
//	//QCPLayer* foregroundLayer;
//};

//#ifndef IESPOLARWIDGET_H
//#define IESPOLARWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include <QPointF>
//#include <QPen>
//#include <QBrush>
//#include <QFont>
//
//#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
//#include <qwt_plot_grid.h>
//#include <qwt_plot_marker.h>
//#include <qwt_legend.h>
//#include <qwt_symbol.h>
//#include <qwt_text.h>
//
//class IESPolarWidget : public QwtPlot
//{
//    Q_OBJECT
//
//public:
//    explicit IESPolarWidget(QWidget* parent = nullptr);
//    ~IESPolarWidget();
//
//    void updateIES(double angle = 0);
//    void Init();
//
//    std::vector<QPointF> generateC0C180Profile(double angle = 0);  // c0-c180
//    std::vector<QPointF> generateC90C270Profile(double angle = 0); // c90-c270
//
//public slots:
//    void on_chkFillBlue_stateChanged(int value);
//    void on_chkFillRed_stateChanged(int value);
//    void on_chkViewRed_stateChanged(int value);
//
//    void on_chkFillGreen_stateChanged(int value);
//    void on_chkFillYellow_stateChanged(int value);
//    void on_chkViewYellow_stateChanged(int value);
//
//    void on_horizontalSlider_valueChanged(int value);
//    void on_horizontalSlider_2_valueChanged(int value);
//
//private:
//    // 将极坐标转换为直角坐标
//    QPointF polarToCartesian(double angle, double radius) const;
//
//    // 创建极坐标曲线
//    void createPolarCurve(const std::vector<QPointF>& polarPoints,
//        QColor color,
//        const QString& name,
//        bool fill = false);
//
//    // 更新径向标记
//    void updateRadialMarkers();
//
//    // 更新角度标记
//    void updateAngularMarkers();
//
//    // 清空所有曲线和标记
//    void clearAll();
//
//    // 曲线对象
//    QwtPlotCurve* m_curve0_180;
//    QwtPlotCurve* m_curve90_270;
//
//    // 标记对象
//    QList<QwtPlotMarker*> m_radialMarkers;
//    QList<QwtPlotMarker*> m_angularMarkers;
//    QList<QwtPlotCurve*> m_gridCurves;
//
//    // 当前设置
//    bool m_showCurve90_270;
//    bool m_fill0_180;
//    bool m_fill90_270;
//    QColor m_color0_180;
//    QColor m_color90_270;
//
//    // 最大半径
//    double m_maxRadius;
//
//    // 当前角度
//    double m_currentAngle;
//};
//
//#endif // IESPOLARWIDGET_H


#pragma once

#include <QWidget>
#include <qwt_polar_plot.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_marker.h>
#include <qwt_legend.h>
#include <qwt_point_polar.h>
#include <qwt_series_data.h>
#include <vector>
#include "common.h"

// 自定义极坐标数据类
class PolarSeriesData : public QwtSeriesData<QwtPointPolar>
{
public:
    PolarSeriesData(const QVector<QwtPointPolar>& points = QVector<QwtPointPolar>())
        : m_points(points)
    {
    }

    void setPoints(const QVector<QwtPointPolar>& points)
    {
        m_points = points;
    }

    virtual size_t size() const override
    {
        return m_points.size();
    }

    virtual QwtPointPolar sample(size_t i) const override
    {
        if (i < static_cast<size_t>(m_points.size())) {
            return m_points[i];
        }
        return QwtPointPolar();
    }

    virtual QRectF boundingRect() const override
    {
        if (m_points.isEmpty()) {
            return QRectF();
        }

        double minRadius = std::numeric_limits<double>::max();
        double maxRadius = -std::numeric_limits<double>::max();

        for (const auto& point : m_points) {
            double radius = point.radius();
            minRadius = std::min(minRadius, radius);
            maxRadius = std::max(maxRadius, radius);
        }

        return QRectF(0, minRadius, 360, maxRadius - minRadius);
    }

private:
    QVector<QwtPointPolar> m_points;
};

// 带填充的极坐标曲线类
class FilledPolarCurve : public QwtPolarCurve
{
public:
    explicit FilledPolarCurve(const QString& title = QString())
        : QwtPolarCurve(title)
        , m_fillBrush(Qt::NoBrush), m_angle(0), m_type(eC0)
    {
    }

    void setFillBrush(const QBrush& brush)
    {
        m_fillBrush = brush;
    }

    virtual void draw(QPainter* painter,
        const QwtScaleMap& azimuthMap,
        const QwtScaleMap& radialMap,
        const QPointF& pole,
        double radius,
        const QRectF& canvasRect) const override
    {
        // 先调用基类绘制曲线
        QwtPolarCurve::draw(painter, azimuthMap, radialMap, pole, radius, canvasRect);

        // 如果有填充画刷，绘制填充区域
        if (m_fillBrush != Qt::NoBrush && dataSize() > 1)
        {
            painter->save();
            painter->setBrush(m_fillBrush);
            painter->setPen(Qt::NoPen);

            // 创建多边形路径
            QPolygonF polygon;

            // 将极坐标点转换为直角坐标
            for (size_t i = 0; i < dataSize(); ++i)
            {
                QwtPointPolar polarPoint = sample(i);
                double azimuth = azimuthMap.transform(polarPoint.azimuth());
                double radial = radialMap.transform(polarPoint.radius());

                double dx = std::sin(azimuth+ m_angle);
                double dy = -std::cos(azimuth+ m_angle);

                if (m_type > 4) 
                {
                    QPointF direction(dy, dx);
                    QPointF point = pole + radial * direction;
                    polygon.append(point);
                }
                else
                {
                    QPointF direction(dx, dy);
                    QPointF point = pole + radial * direction;
                    polygon.append(point);
                }    
            }

            // 如果曲线不封闭，连接到中心点形成封闭区域
            if (!polygon.isEmpty())
            {
                // 连接到中心点
                polygon.append(pole);
                // 再连接回第一个点
                polygon.append(polygon.first());

                // 绘制填充多边形
                painter->drawPolygon(polygon);
            }

            painter->restore();
        }
    }

public:
    double m_angle;
    EIESType m_type;
    QBrush m_fillBrush;
};

class IESPolarWidget : public QwtPolarPlot
{
    Q_OBJECT
public:
    IESPolarWidget(QWidget* parent = nullptr);
    ~IESPolarWidget();

    void updateIES(double angle = 0);
    void initPlot();

    QVector<QwtPointPolar> generateC0C180Profile(double angle = 0);  // c0-c180
    QVector<QwtPointPolar> generateC90C270Profile(double angle = 0); // c90 - c270

public slots:
    void on_chkFillBlue_stateChanged(int value);
    void on_chkFillRed_stateChanged(int value);
    void on_chkViewRed_stateChanged(int value);
    void on_chkFillGreen_stateChanged(int value);
    void on_chkFillYellow_stateChanged(int value);
    void on_chkViewYellow_stateChanged(int value);
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);

private:
    QwtPolarCurve* m_curve0_180 = nullptr;
    QwtPolarCurve* m_curve90_270 = nullptr;
    QwtPolarGrid* m_grid = nullptr;

    QPen m_pen0_180;
    QPen m_pen90_270;
    QBrush m_brush0_180;
    QBrush m_brush90_270;

};