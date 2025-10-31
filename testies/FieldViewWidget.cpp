#include "fieldviewwidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <QMenu>
#include <QAction>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FieldViewWidget::FieldViewWidget(QWidget* parent)
    : QWidget(parent), maxMagnitude(0.0), minMagnitude(0.0)
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建QCustomPlot
    customPlot = new QCustomPlot(this);
    layout->addWidget(customPlot);

    // 设置图表
    setupFieldViewPlot();
    resize(QSize(640, 480));
}

FieldViewWidget::~FieldViewWidget()
{
}

void FieldViewWidget::setupFieldViewPlot()
{
    // 设置交互功能
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // 设置坐标轴
    customPlot->xAxis->setLabel("X");
    customPlot->yAxis->setLabel("Y");

    // 设置坐标轴范围（对称显示）
    customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
    customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);

    // 设置网格
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);

    // 设置标题
    customPlot->plotLayout()->insertRow(0);
    QCPTextElement* title = new QCPTextElement(customPlot, "FOV", QFont("sans", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    // 启用上下文菜单
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接自定义上下文菜单
    connect(customPlot, &QCustomPlot::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu menu(this);
        QAction* resetAction = menu.addAction("重置视图");
        QAction* autoRangeAction = menu.addAction("自动调整范围");

        QAction* selectedAction = menu.exec(customPlot->mapToGlobal(pos));

        if (selectedAction == resetAction) {
            resetView();
        }
        else if (selectedAction == autoRangeAction) {
            updatePlot();
        }
        });

    // 连接双击重置视图
    connect(customPlot, &QCustomPlot::mouseDoubleClick, this, [this](QMouseEvent* event) {
        Q_UNUSED(event)
            resetView();
        });
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
        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
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
    customPlot->clearItems();
    customPlot->replot();
}

void FieldViewWidget::resetView()
{
    // 重置到默认范围
    customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
    customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);
    customPlot->replot();
}

void FieldViewWidget::updatePlot()
{
    // 清除之前的箭头
    customPlot->clearItems();

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
            customPlot->xAxis->setRange(-m_defaultRange, m_defaultRange);
            customPlot->yAxis->setRange(-m_defaultRange, m_defaultRange);
        }
        else {
            customPlot->xAxis->setRange(xMin - marginX, xMax + marginX);
            customPlot->yAxis->setRange(yMin - marginY, yMax + marginY);
        }
    }
    else {
        // 没有数据时使用默认范围
        resetView();
        return;
    }

    // 绘制箭头
    drawFieldArrows();

    customPlot->replot();
}

void FieldViewWidget::drawFieldArrows()
{
    // 获取当前坐标轴范围来计算基准箭头长度
    double xRange = customPlot->xAxis->range().size();
    double yRange = customPlot->yAxis->range().size();
    double baseArrowLength = 4; // 基准长度为视图范围的8%

    for (const FieldData& data : fieldData) {
        // 创建箭头
        QCPItemLine* arrow = new QCPItemLine(customPlot);

        // 计算箭头方向（角度转换为弧度）
        double angleRad = data.angle * M_PI / 180.0;

        // 箭头起点（数据点位置）
        arrow->start->setCoords(data.xField, data.yField);

        // 计算箭头长度（与magnitude成正比）
        double arrowLength = getArrowLength(data.magnitude) * baseArrowLength;

        // 箭头终点（基于角度和长度计算）
        double endX = data.xField + arrowLength * std::cos(angleRad);
        double endY = data.yField + arrowLength * std::sin(angleRad);
        arrow->end->setCoords(endX, endY);

        // 设置箭头样式
        QPen arrowPen = getArrowPen(data.magnitude);
        arrow->setPen(arrowPen);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        // 设置箭头头部大小与线条粗细成比例
        double headSize = arrowPen.widthF() * 2.0;
        arrow->setHead(QCPLineEnding(QCPLineEnding::esSpikeArrow, headSize, headSize * 0.8));
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

    // 长度在0.1到1.0之间线性变化
    return 0.1 + normalized * 1;
}

QPen FieldViewWidget::getArrowPen(double magnitude)
{
    // 根据幅值设置箭头颜色和粗细
    // 使用蓝色系，幅值越大颜色越深/线越粗

    // 归一化幅值
    double normalized = 0.5;
    if (maxMagnitude > minMagnitude) {
        normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
    }

    // 计算颜色（从浅蓝到深蓝）
    int blueValue = 100 + static_cast<int>(155 * normalized);
    QColor arrowColor(0, 0, blueValue);

    // 计算线宽（1 - 3.0）
    double lineWidth = 1 + normalized * 2.5;

    return QPen(arrowColor, lineWidth);
}