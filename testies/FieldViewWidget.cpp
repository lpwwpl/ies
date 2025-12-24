//#include "fieldviewwidget.h"
//#include <QVBoxLayout>
//#include <QFile>
//#include <QTextStream>
//#include <QDebug>
//#include <QMessageBox>
//#include <cmath>
//#include <QMenu>
//#include <QAction>
//
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif
//
//FieldViewWidget::FieldViewWidget(QWidget* parent)
//    : QWidget(parent), maxMagnitude(0.0), minMagnitude(0.0)
//{
//    // 创建布局
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//
//    // 创建QCustomPlot
//    customPlot = new QCustomPlot(this);
//    layout->addWidget(customPlot);
//
//    // 设置图表
//    setupFieldViewPlot();
//    resize(QSize(640, 480));
//}
//
//FieldViewWidget::~FieldViewWidget()
//{
//}
//
//void FieldViewWidget::setupFieldViewPlot()
//{
//    // 设置交互功能
//    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//
//    // 设置坐标轴
//    customPlot->xAxis->setLabel("X");
//    customPlot->yAxis->setLabel("Y");
//
//    // 设置坐标轴范围（对称显示）
//    customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
//    customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);
//
//    // 设置网格
//    customPlot->xAxis->grid()->setVisible(true);
//    customPlot->yAxis->grid()->setVisible(true);
//    customPlot->xAxis->grid()->setSubGridVisible(true);
//    customPlot->yAxis->grid()->setSubGridVisible(true);
//
//    // 设置标题
//    customPlot->plotLayout()->insertRow(0);
//    QCPTextElement* title = new QCPTextElement(customPlot, "FOV", QFont("sans", 12, QFont::Bold));
//    customPlot->plotLayout()->addElement(0, 0, title);
//
//    // 启用上下文菜单
//    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
//
//    // 连接自定义上下文菜单
//    connect(customPlot, &QCustomPlot::customContextMenuRequested, this, [this](const QPoint& pos) {
//        QMenu menu(this);
//        QAction* resetAction = menu.addAction("重置视图");
//        QAction* autoRangeAction = menu.addAction("自动调整范围");
//
//        QAction* selectedAction = menu.exec(customPlot->mapToGlobal(pos));
//
//        if (selectedAction == resetAction) {
//            resetView();
//        }
//        else if (selectedAction == autoRangeAction) {
//            updatePlot();
//        }
//        });
//
//    // 连接双击重置视图
//    connect(customPlot, &QCustomPlot::mouseDoubleClick, this, [this](QMouseEvent* event) {
//        Q_UNUSED(event)
//            resetView();
//        });
//}
//
//bool FieldViewWidget::loadFieldDataFile(const QString& filename)
//{
//    // 清空现有数据
//    clearData();
//
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
//        return false;
//    }
//
//    QTextStream in(&file);
//    int lineNumber = 0;
//    bool hasValidData = false;
//
//    // 初始化最大值最小值
//    maxMagnitude = -1e9;
//    minMagnitude = 1e9;
//
//    while (!in.atEnd()) {
//        QString line = in.readLine().trimmed();
//        lineNumber++;
//
//        // 跳过空行和标题行
//        if (line.isEmpty() || line.startsWith("index") || line.startsWith("#")) {
//            continue;
//        }
//
//        // 解析数据行
//        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
//        if (parts.size() >= 5) {
//            bool ok;
//            FieldData data;
//
//            data.index = parts[0].toInt(&ok);
//            if (!ok) continue;
//
//            data.xField = parts[1].toDouble(&ok);
//            if (!ok) continue;
//
//            data.yField = parts[2].toDouble(&ok);
//            if (!ok) continue;
//
//            data.magnitude = parts[3].toDouble(&ok);
//            if (!ok) continue;
//
//            data.angle = parts[4].toDouble(&ok);
//            if (!ok) continue;
//
//            fieldData.append(data);
//            hasValidData = true;
//
//            // 更新最大值最小值
//            if (data.magnitude > maxMagnitude) maxMagnitude = data.magnitude;
//            if (data.magnitude < minMagnitude) minMagnitude = data.magnitude;
//        }
//    }
//
//    file.close();
//
//    if (hasValidData) {
//        updatePlot();
//        return true;
//    }
//    else {
//        QMessageBox::warning(this, "错误", "文件中没有找到有效的视场数据");
//        return false;
//    }
//}
//
//void FieldViewWidget::clearData()
//{
//    fieldData.clear();
//    maxMagnitude = 0.0;
//    minMagnitude = 0.0;
//    customPlot->clearItems();
//    customPlot->replot();
//}
//
//void FieldViewWidget::resetView()
//{
//    // 重置到默认范围
//    customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
//    customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);
//    customPlot->replot();
//}
//
//void FieldViewWidget::updatePlot()
//{
//    // 清除之前的箭头
//    customPlot->clearItems();
//
//    // 自动调整坐标轴范围
//    if (!fieldData.isEmpty()) {
//        double xMin = 1e9, xMax = -1e9, yMin = 1e9, yMax = -1e9;
//        for (const FieldData& data : fieldData) {
//            if (data.xField < xMin) xMin = data.xField;
//            if (data.xField > xMax) xMax = data.xField;
//            if (data.yField < yMin) yMin = data.yField;
//            if (data.yField > yMax) yMax = data.yField;
//        }
//
//        // 添加边距
//        double xRange = xMax - xMin;
//        double yRange = yMax - yMin;
//        double marginX = xRange * 0.1;
//        double marginY = yRange * 0.1;
//
//        // 如果数据范围很小，使用默认范围
//        if (xRange < 1e-6 || yRange < 1e-6) {
//            customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
//            customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);
//        }
//        else {
//            customPlot->xAxis->setRange(xMin - marginX, xMax + marginX);
//            customPlot->yAxis->setRange(yMin - marginY, yMax + marginY);
//        }
//    }
//    else {
//        // 没有数据时使用默认范围
//        resetView();
//        return;
//    }
//
//    // 绘制箭头
//    drawFieldArrows();
//
//    customPlot->replot();
//}
//
//void FieldViewWidget::drawFieldArrows()
//{
//    // 获取当前坐标轴范围来计算基准箭头长度
//    double xRange = customPlot->xAxis->range().size();
//    double yRange = customPlot->yAxis->range().size();
//    double baseArrowLength = 4; // 基准长度为视图范围的8%
//
//    for (const FieldData& data : fieldData) {
//        // 创建箭头
//        QCPItemLine* arrow = new QCPItemLine(customPlot);
//
//        // 计算箭头方向（角度转换为弧度）
//        double angleRad = data.angle * M_PI / 180.0;
//
//        // 箭头起点（数据点位置）
//        arrow->start->setCoords(data.xField, data.yField);
//
//        // 计算箭头长度（与magnitude成正比）
//        double arrowLength = getArrowLength(data.magnitude) * baseArrowLength;
//
//        // 箭头终点（基于角度和长度计算）
//        double endX = data.xField + arrowLength * std::cos(angleRad);
//        double endY = data.yField + arrowLength * std::sin(angleRad);
//        arrow->end->setCoords(endX, endY);
//
//        // 设置箭头样式
//        QPen arrowPen = getArrowPen(data.magnitude);
//        arrow->setPen(arrowPen);
//        arrow->setHead(QCPLineEnding::esSpikeArrow);
//
//        // 设置箭头头部大小与线条粗细成比例
//        double headSize = arrowPen.widthF() * 2.0;
//        arrow->setHead(QCPLineEnding(QCPLineEnding::esSpikeArrow, headSize, headSize * 0.8));
//    }
//}
//
//double FieldViewWidget::getArrowLength(double magnitude)
//{
//    // 根据幅值计算箭头长度比例 (0.5 - 2.0倍)
//    if (maxMagnitude <= minMagnitude) {
//        return 0.5; // 默认长度
//    }
//
//    // 归一化幅值
//    double normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
//
//    // 长度在0.1到1.0之间线性变化
//    return 0.1 + normalized * 1;
//}
//
//QPen FieldViewWidget::getArrowPen(double magnitude)
//{
//    // 根据幅值设置箭头颜色和粗细
//    // 使用蓝色系，幅值越大颜色越深/线越粗
//
//    // 归一化幅值
//    double normalized = 0.5;
//    if (maxMagnitude > minMagnitude) {
//        normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
//    }
//
//    // 计算颜色（从浅蓝到深蓝）
//    int blueValue = 100 + static_cast<int>(155 * normalized);
//    QColor arrowColor(0, 0, blueValue);
//
//    // 计算线宽（1 - 3.0）
//    double lineWidth = 1 + normalized * 2.5;
//
//    return QPen(arrowColor, lineWidth);
//}
#include "FieldViewWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QPainterPath>
#include <cmath>
#include <limits>
#include <algorithm>
#include <qwt_text.h>
#include <qwt_scale_div.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ArrowItem实现
void ArrowItem::draw(QPainter* painter,
    const QwtScaleMap& xMap, const QwtScaleMap& yMap,
    const QRectF& canvasRect) const
{
    Q_UNUSED(canvasRect);

    painter->save();

    // 将数据坐标转换为屏幕坐标
    double x = xMap.transform(m_x);
    double y = yMap.transform(m_y);

    // 计算箭头方向
    double angleRad = m_angle * M_PI / 180.0;
    double dx = m_length * cos(angleRad);
    double dy = m_length * sin(angleRad);

    // 箭头头部参数
    double arrowHeadLength = m_width * 3.0;
    double arrowHeadWidth = m_width * 2.0;

    // 创建箭头路径
    QPainterPath path;

    // 箭头起点
    path.moveTo(x, y);

    // 箭头终点
    double endX = xMap.transform(m_x + dx);
    double endY = yMap.transform(m_y + dy);
    path.lineTo(endX, endY);

    // 绘制箭头主体
    QPen pen(m_color, m_width);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    // 绘制箭头头部
    double angle = atan2(endY - y, endX - x);

    // 计算箭头头部的两个侧点
    double x1 = endX - arrowHeadLength * cos(angle) + arrowHeadWidth * sin(angle);
    double y1 = endY - arrowHeadLength * sin(angle) - arrowHeadWidth * cos(angle);

    double x2 = endX - arrowHeadLength * cos(angle) - arrowHeadWidth * sin(angle);
    double y2 = endY - arrowHeadLength * sin(angle) + arrowHeadWidth * cos(angle);

    QPainterPath arrowHead;
    arrowHead.moveTo(endX, endY);
    arrowHead.lineTo(x1, y1);
    arrowHead.lineTo(x2, y2);
    arrowHead.lineTo(endX, endY);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(m_color));
    painter->drawPath(arrowHead);

    painter->restore();
}

// FieldViewWidget实现
FieldViewWidget::FieldViewWidget(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
    , maxMagnitude(0.0)
    , minMagnitude(0.0)
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建QwtPlot
    m_plot = new QwtPlot(this);
    layout->addWidget(m_plot);

    // 设置图表
    setupFieldViewPlot();
    setupInteractions();

    setMinimumSize(640, 480);
}

FieldViewWidget::~FieldViewWidget()
{
    clearAllArrows();
}

void FieldViewWidget::setupFieldViewPlot()
{
    // 设置画布背景
    m_plot->setCanvasBackground(Qt::white);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);

    // 设置标题
    QwtText title("FOV");
    title.setFont(QFont("sans", 12, QFont::Bold));
    m_plot->setTitle(title);

    // 设置坐标轴标签
    m_plot->setAxisTitle(QwtPlot::xBottom, "X");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Y");

    // 设置坐标轴范围（对称显示）
    m_plot->setAxisScale(QwtPlot::xBottom, -m_defaultRange, m_defaultRange);
    m_plot->setAxisScale(QwtPlot::yLeft, -m_defaultRange, m_defaultRange);

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

    // 启用抗锯齿
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);

    // 设置右键菜单
    m_plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接自定义上下文菜单
    connect(m_plot->canvas(), &QWidget::customContextMenuRequested,
        this, [this](const QPoint& pos) {
            QMenu menu(this);
            QAction* resetAction = menu.addAction("重置视图");
            QAction* autoRangeAction = menu.addAction("自动调整范围");

            QAction* selectedAction = menu.exec(m_plot->canvas()->mapToGlobal(pos));

            if (selectedAction == resetAction) {
                resetView();
            }
            else if (selectedAction == autoRangeAction) {
                updatePlot();
            }
        });

    // 连接双击重置视图
    //connect(m_plot->canvas(), &QWidget::mouseDoubleClick,
    //    this, [this](QMouseEvent* event) {
    //        Q_UNUSED(event);
    //        resetView();
    //    });
}

void FieldViewWidget::setupInteractions()
{
    // 创建拖拽功能 (对应 QCP::iRangeDrag)
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    // 创建缩放功能 (对应 QCP::iRangeZoom)
    m_magnifier = new QwtPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);
}

void FieldViewWidget::clearAllArrows()
{
    for (auto arrow : m_arrowItems) {
        arrow->detach();
        delete arrow;
    }
    m_arrowItems.clear();
}

bool FieldViewWidget::loadFieldDataFile(const QString& filename)
{
    // 清空现有数据
    clearData();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
        return false;
    }

    QTextStream in(&file);
    int lineNumber = 0;
    bool hasValidData = false;

    // 初始化最大值最小值
    maxMagnitude = -1e9;
    minMagnitude = 1e9;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // 跳过空行和标题行
        if (line.isEmpty() || line.startsWith("index") || line.startsWith("#")) {
            continue;
        }

        // 解析数据行
        QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (parts.size() >= 5) {
            bool ok;
            FieldData data;

            data.index = parts[0].toInt(&ok);
            if (!ok) continue;

            data.xField = parts[1].toDouble(&ok);
            if (!ok) continue;

            data.yField = parts[2].toDouble(&ok);
            if (!ok) continue;

            data.magnitude = parts[3].toDouble(&ok);
            if (!ok) continue;

            data.angle = parts[4].toDouble(&ok);
            if (!ok) continue;

            fieldData.append(data);
            hasValidData = true;

            // 更新最大值最小值
            if (data.magnitude > maxMagnitude) maxMagnitude = data.magnitude;
            if (data.magnitude < minMagnitude) minMagnitude = data.magnitude;
        }
    }

    file.close();

    if (hasValidData) {
        updatePlot();
        return true;
    }
    else {
        QMessageBox::warning(this, "错误", "文件中没有找到有效的视场数据");
        return false;
    }
}

void FieldViewWidget::clearData()
{
    fieldData.clear();
    maxMagnitude = 0.0;
    minMagnitude = 0.0;
    clearAllArrows();
    m_plot->replot();
}

void FieldViewWidget::resetView()
{
    // 重置到默认范围
    m_plot->setAxisScale(QwtPlot::xBottom, -m_defaultRange, m_defaultRange);
    m_plot->setAxisScale(QwtPlot::yLeft, -m_defaultRange, m_defaultRange);
    m_plot->replot();
}

void FieldViewWidget::updatePlot()
{
    // 清除之前的箭头
    clearAllArrows();

    // 自动调整坐标轴范围
    if (!fieldData.isEmpty()) {
        double xMin = 1e9, xMax = -1e9, yMin = 1e9, yMax = -1e9;
        for (const FieldData& data : fieldData) {
            if (data.xField < xMin) xMin = data.xField;
            if (data.xField > xMax) xMax = data.xField;
            if (data.yField < yMin) yMin = data.yField;
            if (data.yField > yMax) yMax = data.yField;
        }

        // 添加边距
        double xRange = xMax - xMin;
        double yRange = yMax - yMin;
        double marginX = xRange * 0.1;
        double marginY = yRange * 0.1;

        // 如果数据范围很小，使用默认范围
        if (xRange < 1e-6 || yRange < 1e-6) {
            m_plot->setAxisScale(QwtPlot::xBottom, -m_defaultRange, m_defaultRange);
            m_plot->setAxisScale(QwtPlot::yLeft, -m_defaultRange, m_defaultRange);
        }
        else {
            m_plot->setAxisScale(QwtPlot::xBottom, xMin - marginX, xMax + marginX);
            m_plot->setAxisScale(QwtPlot::yLeft, yMin - marginY, yMax + marginY);
        }
    }
    else {
        // 没有数据时使用默认范围
        resetView();
        return;
    }

    // 绘制箭头
    drawFieldArrows();

    m_plot->replot();
}

void FieldViewWidget::drawFieldArrows()
{
    // 获取当前坐标轴范围来计算基准箭头长度
    double xRange = m_plot->axisScaleDiv(QwtPlot::xBottom).range();
    double yRange = m_plot->axisScaleDiv(QwtPlot::yLeft).range();
    double baseArrowLength = qMin(xRange, yRange) * 0.08; // 基准长度为视图范围的8%

    for (const FieldData& data : fieldData) {
        // 计算箭头长度（与magnitude成正比）
        double arrowLength = getArrowLength(data.magnitude) * baseArrowLength;

        // 获取箭头颜色
        QColor arrowColor = getArrowColor(data.magnitude);

        // 获取箭头线宽
        QPen arrowPen = getArrowPen(data.magnitude);

        // 创建自定义箭头项
        ArrowItem* arrow = new ArrowItem(
            data.xField, data.yField, data.angle,
            arrowLength, arrowColor, arrowPen.widthF()
        );

        arrow->attach(m_plot);
        m_arrowItems.append(arrow);
    }
}

double FieldViewWidget::getArrowLength(double magnitude)
{
    // 根据幅值计算箭头长度比例 (0.5 - 2.0倍)
    if (maxMagnitude <= minMagnitude) {
        return 0.5; // 默认长度
    }

    // 归一化幅值
    double normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);

    // 长度在0.5到2.0之间线性变化
    return 0.0 + normalized * 1.2;
}

QPen FieldViewWidget::getArrowPen(double magnitude)
{
    // 根据幅值设置箭头线宽（1 - 3.0）
    double lineWidth = 1.0;
    if (maxMagnitude > minMagnitude) {
        double normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
        lineWidth = 1.0 + normalized * 2.0;
    }

    QColor arrowColor = getArrowColor(magnitude);
    return QPen(arrowColor, lineWidth);
}

QColor FieldViewWidget::getArrowColor(double magnitude)
{
    // 根据幅值设置箭头颜色
    // 使用蓝色系，幅值越大颜色越深

    // 归一化幅值
    double normalized = 0.5;
    if (maxMagnitude > minMagnitude) {
        normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
    }

    // 计算颜色（从浅蓝到深蓝）
    int blueValue = 100 + static_cast<int>(155 * normalized);
    int greenValue = static_cast<int>(100 * (1.0 - normalized));
    return QColor(0, greenValue, blueValue);
}