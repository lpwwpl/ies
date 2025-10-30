#include "fieldviewwidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <cmath>

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
}

FieldViewWidget::~FieldViewWidget()
{
}

void FieldViewWidget::setupFieldViewPlot()
{
    // 设置坐标轴
    customPlot->xAxis->setLabel("X");
    customPlot->yAxis->setLabel("Y");

    // 设置坐标轴范围（对称显示）
    double range = 50.0; // 默认范围
    customPlot->xAxis->setRange(-range, range);
    customPlot->yAxis->setRange(-range, range);

    // 设置网格
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);

    // 设置标题
    customPlot->plotLayout()->insertRow(0);
    QCPTextElement* title = new QCPTextElement(customPlot, "FOV", QFont("sans", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    // 设置等比例显示
    //customPlot->setAspectRatio(1.0);
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
        //qDebug() << "成功加载" << fieldData.size() << "个视场点";
        //qDebug() << "幅值范围:" << minMagnitude << "到" << maxMagnitude;
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
        double margin = std::max((xMax - xMin), (yMax - yMin)) * 0.1;
        customPlot->xAxis->setRange(xMin - margin, xMax + margin);
        customPlot->yAxis->setRange(yMin - margin, yMax + margin);
    }

    // 绘制箭头
    drawFieldArrows();

    customPlot->replot();
}

void FieldViewWidget::drawFieldArrows()
{
    // 箭头长度基于视场范围的比例
    double xRange = customPlot->xAxis->range().size();
    double yRange = customPlot->yAxis->range().size();
    double baseArrowLength = std::min(xRange, yRange) * 0.1;

    for (const FieldData& data : fieldData) {
        // 创建箭头
        QCPItemLine* arrow = new QCPItemLine(customPlot);

        // 计算箭头方向（角度转换为弧度）
        double angleRad = data.angle * M_PI / 180.0;

        // 箭头起点（数据点位置）
        arrow->start->setCoords(data.xField, data.yField);

        // 箭头终点（基于角度和长度计算）
        double endX = data.xField + baseArrowLength * std::cos(angleRad);
        double endY = data.yField + baseArrowLength * std::sin(angleRad);
        arrow->end->setCoords(endX, endY);

        // 设置箭头样式
        QPen arrowPen = getArrowPen(data.magnitude);
        arrow->setPen(arrowPen);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
    }
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

    // 计算线宽（1.0 - 3.0）
    double lineWidth = 1.0 + normalized * 2.0;

    return QPen(arrowColor, lineWidth);
}