//#include "FootprintWidget.h"
//#include <QDebug>
//#include <QRegularExpression>
//
//FootprintWidget::FootprintWidget(QWidget* parent)
//    : QWidget(parent)
//{
//    // 创建主布局
//    QVBoxLayout* mainLayout = new QVBoxLayout(this);
//
//    // 创建QCustomPlot实例
//    m_customPlot = new QCustomPlot(this);
//    mainLayout->addWidget(m_customPlot);
//
//    // 设置图表
//    setupPlot();
//
//    resize(QSize(640, 640));
//}
//
//void FootprintWidget::setupPlot()
//{
//    // 设置图表标题
//    m_customPlot->plotLayout()->insertRow(0);
//    m_customPlot->plotLayout()->addElement(0, 0,
//        new QCPTextElement(m_customPlot, "Footprint Limits on Surface 1", QFont("sans", 12, QFont::Bold)));
//
//    // 设置坐标轴标签
//    m_customPlot->xAxis->setLabel("X Position");
//    m_customPlot->yAxis->setLabel("Y Position");
//
//    // 设置坐标轴范围（根据数据调整）
//    m_customPlot->xAxis->setRange(-10, 10);
//    m_customPlot->yAxis->setRange(-85, 10);
//
//    // 启用选择、缩放、拖动等功能
//    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//
//    // 显示图例
//    m_customPlot->legend->setVisible(true);
//    m_customPlot->legend->setFont(QFont("Helvetica", 9));
//    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
//}
//
//bool FootprintWidget::loadDataFromFile(const QString& filename)
//{
//    m_footprintData.clear();
//
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qDebug() << "Cannot open file:" << filename;
//        return false;
//    }
//
//    QTextStream in(&file);
//    QString line;
//    FootprintData currentData;
//    bool readingData = false;
//
//    QRegularExpression fieldRegex("Field\\s+(\\w+)");
//    QRegularExpression leftRegex("LEFT\\s+([-\\d.]+)");
//    QRegularExpression rightRegex("RIGHT\\s+([-\\d.]+)");
//    QRegularExpression topRegex("TOP\\s+([-\\d.]+)");
//    QRegularExpression bottomRegex("BOTTOM\\s+([-\\d.]+)");
//    QRegularExpression radiusRegex("RADIUS\\s+([-\\d.]+)");
//
//    while (!in.atEnd()) {
//        line = in.readLine().trimmed();
//
//        if (line.startsWith("Footprint limits on surface")) {
//            readingData = true;
//            currentData = FootprintData();
//            continue;
//        }
//
//        if (readingData) {
//            QRegularExpressionMatch match;
//
//            if ((match = fieldRegex.match(line)).hasMatch()) {
//                currentData.field = match.captured(1);
//            }
//            else if ((match = leftRegex.match(line)).hasMatch()) {
//                currentData.left = match.captured(1).toDouble();
//            }
//            else if ((match = rightRegex.match(line)).hasMatch()) {
//                currentData.right = match.captured(1).toDouble();
//            }
//            else if ((match = topRegex.match(line)).hasMatch()) {
//                currentData.top = match.captured(1).toDouble();
//            }
//            else if ((match = bottomRegex.match(line)).hasMatch()) {
//                currentData.bottom = match.captured(1).toDouble();
//            }
//            else if ((match = radiusRegex.match(line)).hasMatch()) {
//                currentData.radius = match.captured(1).toDouble();
//
//                // 完成一个数据块的读取
//                if (!currentData.field.isEmpty()) {
//                    m_footprintData.append(currentData);
//                }
//                readingData = false;
//            }
//        }
//    }
//
//    file.close();
//
//    qDebug() << "Loaded" << m_footprintData.size() << "footprint records";
//    for (const auto& data : m_footprintData) {
//        qDebug() << "Field:" << data.field
//            << "Left:" << data.left << "Right:" << data.right
//            << "Top:" << data.top << "Bottom:" << data.bottom
//            << "Radius:" << data.radius;
//    }
//
//    return !m_footprintData.isEmpty();
//}
//
//QColor FootprintWidget::getFieldColor(const QString& field)
//{
//    static QMap<QString, QColor> colorMap = {
//        {"0", Qt::red},
//        {"1", Qt::green},
//        {"2", Qt::blue},
//        {"3", Qt::magenta},
//        {"all", Qt::darkYellow}
//    };
//
//    return colorMap.value(field, Qt::gray);
//}
//
//void FootprintWidget::plotFootprints()
//{
//    m_customPlot->clearPlottables();
//    m_customPlot->clearItems();
//
//    for (const FootprintData& data : m_footprintData) {
//        QColor color = getFieldColor(data.field);
//
//        // 创建矩形来表示footprint区域
//        //QCPItemRect* rect = new QCPItemRect(m_customPlot);
//
//        //// 设置矩形位置
//        //rect->topLeft->setCoords(data.left, data.top);
//        //rect->bottomRight->setCoords(data.right, data.bottom);
//
//        //// 设置矩形样式
//        //rect->setPen(QPen(color, 2));
//        QColor fillColor = color;
//        fillColor.setAlpha(50); // 半透明填充
//        //rect->setBrush(QBrush(fillColor));
//
//        // 添加半径圆（可选）
//        QCPItemEllipse* ellipse = new QCPItemEllipse(m_customPlot);
//        double centerX = (data.left + data.right) / 2.0;
//        double centerY = (data.top + data.bottom) / 2.0;
//
//        double x_half_range = (data.right - data.left) / 2;
//        double y_half_range = (data.top - data.bottom) / 2;
//
//        double radius = 0;
//        if (x_half_range > radius)radius = x_half_range;
//        if (y_half_range > radius)radius = y_half_range;
//        if (data.field == "all")
//        {
//            centerX = 0;
//            centerY = 0;
//            radius = data.radius;
//        }
//
//        ellipse->topLeft->setCoords(centerX - radius, centerY + radius);
//        ellipse->bottomRight->setCoords(centerX + radius, centerY - radius);
//        ellipse->setPen(QPen(color, 1, Qt::DashLine));
//        
//        // 创建图例项（使用一个不可见的曲线来创建图例）
//        QCPGraph* legendGraph = m_customPlot->addGraph();
//        legendGraph->setName(QString("Field %1").arg(data.field));
//        legendGraph->setPen(QPen(color, 2));
//        legendGraph->setBrush(QBrush(fillColor));
//        legendGraph->setLineStyle(QCPGraph::lsNone);
//        legendGraph->setScatterStyle(QCPScatterStyle::ssNone);
//    }
//    double range = -INFINITY;
//    // 重新调整坐标轴范围以适应所有数据
//    double minX = 0, maxX = 0, minY = 0, maxY = 0;
//    for (const FootprintData& data : m_footprintData) {
//        minX = qMin(minX, data.left);
//        maxX = qMax(maxX, data.right);
//        minY = qMin(minY, data.bottom);
//        maxY = qMax(maxY, data.top);
//    }
//    if (fabs(minX) > range)range = fabs(minX);
//    if (fabs(maxX) > range)range = fabs(maxX);
//    if (fabs(minY) > range)range = fabs(minY);
//    if (fabs(maxY) > range)range = fabs(maxY);
//    // 添加一些边距
//    double marginX = (range - minX) * 0.1;
//    double marginY = (range - minY) * 0.1;
//    m_customPlot->xAxis->setRange(-range - marginX, range + marginX);
//    m_customPlot->yAxis->setRange(-range - marginY, range + marginY);
//    m_customPlot->rescaleAxes();
//    m_customPlot->replot();
//}

#include "FootprintWidget.h"
#include <QDebug>
#include <QRegularExpression>
#include <QPainter>
#include <cmath>
#include <limits>
#include <algorithm>
#include <qwt_text.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FootprintWidget::FootprintWidget(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建QwtPlot实例
    m_plot = new QwtPlot(this);
    mainLayout->addWidget(m_plot);

    // 设置图表
    setupPlot();
    setupInteractions();

    setMinimumSize(640, 640);
}

FootprintWidget::~FootprintWidget()
{
    clearAllCurves();
}

void FootprintWidget::setupPlot()
{
    // 设置画布背景
    m_plot->setCanvasBackground(Qt::white);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);

    // 设置标题
    QwtText title("Footprint Limits on Surface 1");
    title.setFont(QFont("sans", 12, QFont::Bold));
    m_plot->setTitle(title);

    // 设置坐标轴标签
    m_plot->setAxisTitle(QwtPlot::xBottom, "X Position");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Y Position");

    // 设置坐标轴范围（初始范围，后续会根据数据调整）
    m_plot->setAxisScale(QwtPlot::xBottom, -10, 10);
    m_plot->setAxisScale(QwtPlot::yLeft, -85, 10);

    // 设置网格
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableY(true);
    grid->enableXMin(true);
    grid->enableYMin(true);

    // 设置主网格线样式
    QPen majorGridPen(QColor(200, 200, 200), 1, Qt::SolidLine);
    grid->setMajorPen(majorGridPen);

    // 设置次网格线样式
    QPen minorGridPen(QColor(220, 220, 220), 1, Qt::DotLine);
    grid->setMinorPen(minorGridPen);

    grid->attach(m_plot);

    // 设置坐标轴样式
    QPen axisPen(Qt::black, 2);
    //m_plot->setAxisPen(QwtPlot::xBottom, axisPen);
    //m_plot->setAxisPen(QwtPlot::yLeft, axisPen);

    // 设置图例
    QwtLegend* legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    m_plot->insertLegend(legend, QwtPlot::RightLegend);

    // 设置图例样式
    legend->setFont(QFont("Helvetica", 9));

    // 启用抗锯齿
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);
}

void FootprintWidget::setupInteractions()
{
    // 创建拖拽功能
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    // 创建缩放功能
    m_magnifier = new QwtPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);
}

void FootprintWidget::clearAllCurves()
{
    // 清理圆曲线
    for (auto curve : m_circleCurves) {
        curve->detach();
        delete curve;
    }
    m_circleCurves.clear();

    // 清理图例曲线
    for (auto curve : m_legendCurves) {
        curve->detach();
        delete curve;
    }
    m_legendCurves.clear();
}

QVector<QPointF> FootprintWidget::createCirclePoints(double centerX, double centerY, double radius, int segments)
{
    QVector<QPointF> points;
    points.reserve(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        double x = centerX + radius * cos(angle);
        double y = centerY + radius * sin(angle);
        points.append(QPointF(x, y));
    }

    return points;
}

QVector<QPointF> FootprintWidget::createRectanglePoints(double left, double right, double top, double bottom)
{
    QVector<QPointF> points;
    points.reserve(5);

    // 顺时针创建矩形点
    points.append(QPointF(left, top));
    points.append(QPointF(right, top));
    points.append(QPointF(right, bottom));
    points.append(QPointF(left, bottom));
    points.append(QPointF(left, top)); // 闭合矩形

    return points;
}

bool FootprintWidget::loadDataFromFile(const QString& filename)
{
    m_footprintData.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file:" << filename;
        return false;
    }

    QTextStream in(&file);
    QString line;
    FootprintData currentData;
    bool readingData = false;

    QRegularExpression fieldRegex("Field\\s+(\\w+)");
    QRegularExpression leftRegex("LEFT\\s+([-\\d.]+)");
    QRegularExpression rightRegex("RIGHT\\s+([-\\d.]+)");
    QRegularExpression topRegex("TOP\\s+([-\\d.]+)");
    QRegularExpression bottomRegex("BOTTOM\\s+([-\\d.]+)");
    QRegularExpression radiusRegex("RADIUS\\s+([-\\d.]+)");

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("Footprint limits on surface")) {
            readingData = true;
            currentData = FootprintData();
            continue;
        }

        if (readingData) {
            QRegularExpressionMatch match;

            if ((match = fieldRegex.match(line)).hasMatch()) {
                currentData.field = match.captured(1);
            }
            else if ((match = leftRegex.match(line)).hasMatch()) {
                currentData.left = match.captured(1).toDouble();
            }
            else if ((match = rightRegex.match(line)).hasMatch()) {
                currentData.right = match.captured(1).toDouble();
            }
            else if ((match = topRegex.match(line)).hasMatch()) {
                currentData.top = match.captured(1).toDouble();
            }
            else if ((match = bottomRegex.match(line)).hasMatch()) {
                currentData.bottom = match.captured(1).toDouble();
            }
            else if ((match = radiusRegex.match(line)).hasMatch()) {
                currentData.radius = match.captured(1).toDouble();

                // 完成一个数据块的读取
                if (!currentData.field.isEmpty()) {
                    m_footprintData.append(currentData);
                }
                readingData = false;
            }
        }
    }

    file.close();

    qDebug() << "Loaded" << m_footprintData.size() << "footprint records";
    for (const auto& data : m_footprintData) {
        qDebug() << "Field:" << data.field
            << "Left:" << data.left << "Right:" << data.right
            << "Top:" << data.top << "Bottom:" << data.bottom
            << "Radius:" << data.radius;
    }

    return !m_footprintData.isEmpty();
}

QColor FootprintWidget::getFieldColor(const QString& field)
{
    static QMap<QString, QColor> colorMap = {
        {"0", Qt::red},
        {"1", Qt::green},
        {"2", Qt::blue},
        {"3", Qt::magenta},
        {"all", Qt::darkYellow}
    };

    return colorMap.value(field, Qt::gray);
}

void FootprintWidget::plotFootprints()
{
    // 清除之前的曲线
    clearAllCurves();

    // 计算总体范围
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double maxRadius = 0;

    for (const FootprintData& data : m_footprintData) {
        if (data.field == "all") {
            // 对于"all"字段，圆心在(0,0)，半径为data.radius
            maxRadius = std::max(maxRadius, std::abs(data.radius));
            minX = std::min(minX, -data.radius);
            maxX = std::max(maxX, data.radius);
            minY = std::min(minY, -data.radius);
            maxY = std::max(maxY, data.radius);
        }
        else {
            // 对于其他字段，计算包围矩形
            minX = std::min(minX, data.left);
            maxX = std::max(maxX, data.right);
            minY = std::min(minY, data.bottom);
            maxY = std::max(maxY, data.top);

            // 计算半径（取矩形半宽和半高中较大的一个）
            double xHalfRange = (data.right - data.left) / 2;
            double yHalfRange = (data.top - data.bottom) / 2;
            double radius = std::max(xHalfRange, yHalfRange);
            maxRadius = std::max(maxRadius, radius);
        }
    }

    // 绘制每个Footprint
    for (const FootprintData& data : m_footprintData) {
        QColor color = getFieldColor(data.field);
        QColor fillColor = color;
        fillColor.setAlpha(50); // 半透明填充

        if (data.field == "all") {
            // 绘制整个视场的圆
            QVector<QPointF> circlePoints = createCirclePoints(0, 0, data.radius);

            QwtPlotCurve* circleCurve = new QwtPlotCurve(QString("Field %1").arg(data.field));
            circleCurve->setSamples(circlePoints);

            // 设置圆样式（虚线）
            QPen circlePen(color, 1);
            circlePen.setStyle(Qt::DashLine);
            circleCurve->setPen(circlePen);
            circleCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
            circleCurve->setItemAttribute(QwtPlotItem::Legend, false);
            circleCurve->attach(m_plot);

            m_circleCurves.append(circleCurve);
        }
        else {
            // 计算圆心和半径
            double centerX = (data.left + data.right) / 2.0;
            double centerY = (data.top + data.bottom) / 2.0;

            double xHalfRange = (data.right - data.left) / 2;
            double yHalfRange = (data.top - data.bottom) / 2;
            double radius = std::max(xHalfRange, yHalfRange);

            // 绘制圆
            QVector<QPointF> circlePoints = createCirclePoints(centerX, centerY, radius);

            QwtPlotCurve* circleCurve = new QwtPlotCurve(QString("Field %1").arg(data.field));
            circleCurve->setSamples(circlePoints);

            // 设置圆样式（虚线）
            QPen circlePen(color, 1);
            circlePen.setStyle(Qt::DashLine);
            circleCurve->setPen(circlePen);
            circleCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
            circleCurve->setItemAttribute(QwtPlotItem::Legend, false);
            circleCurve->attach(m_plot);

            m_circleCurves.append(circleCurve);

            // 可选：绘制矩形（如果需要）
            // QVector<QPointF> rectPoints = createRectanglePoints(data.left, data.right, data.top, data.bottom);
            // QwtPlotCurve* rectCurve = new QwtPlotCurve();
            // rectCurve->setSamples(rectPoints);
            // rectCurve->setPen(QPen(color, 2));
            // rectCurve->setBrush(QBrush(fillColor));
            // rectCurve->attach(m_plot);
            // m_circleCurves.append(rectCurve);
        }

        // 创建图例项（使用一个不可见的点来创建图例）
        QwtPlotCurve* legendCurve = new QwtPlotCurve(QString("Field %1").arg(data.field));
        QVector<QPointF> legendPoint;
        legendPoint.append(QPointF(0, 0)); // 随便一个点，不显示

        legendCurve->setSamples(legendPoint);
        legendCurve->setPen(QPen(color, 2));
        legendCurve->setBrush(QBrush(fillColor));
        legendCurve->setSymbol(new QwtSymbol(QwtSymbol::NoSymbol));
        legendCurve->setItemAttribute(QwtPlotItem::Legend, true);
        legendCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
        legendCurve->attach(m_plot);

        m_legendCurves.append(legendCurve);
    }

    // 计算总体范围（包括所有圆）
    double range = maxRadius;
    range = std::max(range, std::max(std::abs(minX), std::abs(maxX)));
    range = std::max(range, std::max(std::abs(minY), std::abs(maxY)));

    // 添加边距
    double margin = range * 0.1;

    // 设置坐标轴范围
    m_plot->setAxisScale(QwtPlot::xBottom, -range - margin, range + margin);
    m_plot->setAxisScale(QwtPlot::yLeft, -range - margin, range + margin);

    // 重新绘制
    m_plot->replot();
}