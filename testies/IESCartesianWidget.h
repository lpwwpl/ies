//// IESSpotLightWidget.h
//#pragma once
//#include <QWidget>
//#include "QCustomPlot.h"
//#include "tiny_ies.hpp"
//
//
//class IESCartesianWidget:public QCustomPlot
//{
//	Q_OBJECT
//public:
//	IESCartesianWidget(QWidget* parent=0);
//	~IESCartesianWidget();
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
//
//	QCPGraph* graph0_180 = nullptr;
//	QCPGraph* graph90_270 = nullptr;
//
//
//	//tiny_ies<double>::light light;
//	//QCPLayer* backgroundLayer;
//	//QCPLayer* foregroundLayer;
//};

#pragma once

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <vector>
#include <memory>
#include <qwt_scale_map.h>
#include <QPainter>

class IESCartesianWidget : public QwtPlot
{
    Q_OBJECT
public:
    IESCartesianWidget(QWidget* parent = nullptr);
    ~IESCartesianWidget();

    void updateIES(double angle = 0);
    void initPlot();

    std::vector<QPointF> generateC0C180Profile(double angle = 0);  // c0-c180
    std::vector<QPointF> generateC90C270Profile(double angle = 0); // c90-c270

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
    // 自定义曲线类，支持填充
    class FilledCurve : public QwtPlotCurve
    {
    public:
        explicit FilledCurve(const QString& title = QString())
            : QwtPlotCurve(title)
            , m_fillBrush(Qt::NoBrush)
        {
        }

        void setFillBrush(const QBrush& brush)
        {
            m_fillBrush = brush;
        }

        virtual void drawCurve(QPainter* painter, int style,
            const QwtScaleMap& xMap,
            const QwtScaleMap& yMap,
            const QRectF& canvasRect,
            int from,
            int to) const override
        {
            // 先绘制填充
            if (m_fillBrush != Qt::NoBrush && dataSize() > 1)
            {
                drawFilled(painter, xMap, yMap, from, to);
            }

            // 然后绘制曲线
            QwtPlotCurve::drawCurve(painter, style, xMap, yMap, canvasRect, from, to);
        }

    private:
        void drawFilled(QPainter* painter,
            const QwtScaleMap& xMap,
            const QwtScaleMap& yMap,
            int from, int to) const
        {
            painter->save();
            painter->setBrush(m_fillBrush);
            painter->setPen(Qt::NoPen);

            QPolygonF polygon;

            // 添加数据点
            for (int i = from; i <= to; ++i)
            {
                const QPointF sample = data()->sample(i);
                const double x = xMap.transform(sample.x());
                const double y = yMap.transform(sample.y());
                polygon.append(QPointF(x, y));
            }

            // 创建封闭区域
            if (!polygon.isEmpty())
            {
                // 连接到X轴形成封闭区域
                QPointF lastPoint = polygon.last();
                polygon.append(QPointF(lastPoint.x(), yMap.transform(0)));

                QPointF firstPoint = polygon.first();
                polygon.append(QPointF(firstPoint.x(), yMap.transform(0)));

                // 绘制填充多边形
                painter->drawPolygon(polygon);
            }

            painter->restore();
        }

        QBrush m_fillBrush;
    };

    FilledCurve* m_curve0_180 = nullptr;
    FilledCurve* m_curve90_270 = nullptr;
    QwtPlotGrid* m_grid = nullptr;

    QPen m_pen0_180;
    QPen m_pen90_270;
    QBrush m_brush0_180;
    QBrush m_brush90_270;

    // 缩放和平移工具
    QwtPlotZoomer* m_zoomer = nullptr;
    QwtPlotPanner* m_panner = nullptr;
    QwtPlotMagnifier* m_magnifier = nullptr;
};