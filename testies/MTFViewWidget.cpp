#include "MTFViewWidget.h"
#include "qcustomplot.h"

#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMouseEvent>

MTFViewer::MTFViewer(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
{
    setupUI();
}

void MTFViewer::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建绘图区域
    m_plot = new QCustomPlot(this);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
    m_plot->setMinimumSize(400, 300);

    // 设置坐标轴
    m_plot->xAxis->setLabel("空间频率 (cycles/mm)");
    m_plot->yAxis->setLabel("MTF值");
    m_plot->xAxis->setRange(0, 200);
    m_plot->yAxis->setRange(0, 1.1);

    // 显示图例
    m_plot->legend->setVisible(true);
    m_plot->legend->setFillOrder(QCPLegend::foColumnsFirst);

    layout->addWidget(m_plot);

    // 连接信号
    connect(m_plot->legend, &QCPLegend::selectionChanged, this, &MTFViewer::onSelectionChanged);
}

bool MTFViewer::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString("无法打开文件: %1").arg(filename));
        return false;
    }

    QTextStream in(&file);
    QString data = in.readAll();
    file.close();

    m_currentFile = filename;
    return loadFromString(data);
}

bool MTFViewer::loadFromString(const QString& data)
{
    m_lines.clear();

    QStringList lines = data.split('\n', Qt::SkipEmptyParts);

    MTFLine currentLine;
    bool readingData = false;
    bool hasValidData = false;

    for (const QString& line : lines) {
        if (line.startsWith("# CHART NAME:")) {
            // 新图表开始，如果当前有数据，保存前一条线
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                currentLine = MTFLine();
                hasValidData = true;
            }
            readingData = false;
        }
        else if (line.startsWith("  LINE INDEX:")) {
            // 新线条开始
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                hasValidData = true;
            }
            currentLine = MTFLine();
            currentLine.index = line.split(":").last().trimmed().toInt();
            currentLine.visible = true;
            readingData = false;
        }
        else if (line.startsWith("  LINE LABEL:")) {
            currentLine.label = line.split(":").last().trimmed();
        }
        else if (line.startsWith("  LINE COLOR:")) {
            currentLine.colorName = line.split(":").last().trimmed();
        }
        else if (line.startsWith("     INDEX")) {
            // 数据表头，下一行开始是数据
            readingData = true;
        }
        else if (readingData && !line.trimmed().isEmpty() && !line.startsWith("#")) {
            // 读取数据行
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                bool ok1, ok2;
                double x = parts[1].toDouble(&ok1);
                double y = parts[2].toDouble(&ok2);
                if (ok1 && ok2) {
                    currentLine.data.append(QPointF(x, y));
                    hasValidData = true;
                }
            }
        }
    }

    // 添加最后一条线
    if (!currentLine.data.isEmpty()) {
        m_lines.append(currentLine);
        hasValidData = true;
    }

    if (hasValidData) {
        plotData();
        emit dataLoaded(true);
        return true;
    }
    else {
        emit errorOccurred("未找到有效的MTF数据");
        return false;
    }
}

void MTFViewer::clearData()
{
    m_lines.clear();
    m_plot->clearGraphs();
    m_plot->replot();
    m_currentFile.clear();
}

void MTFViewer::setXRange(double min, double max)
{
    m_plot->xAxis->setRange(min, max);
    m_plot->replot();
}

void MTFViewer::setYRange(double min, double max)
{
    m_plot->yAxis->setRange(min, max);
    m_plot->replot();
}

void MTFViewer::setTitle(const QString& title)
{
    m_plot->plotLayout()->insertRow(0);
    m_plot->plotLayout()->addElement(0, 0, new QCPTextElement(m_plot, title, QFont("sans", 12, QFont::Bold)));
    m_plot->replot();
}

void MTFViewer::plotData()
{
    m_plot->clearGraphs();

    // 设置坐标轴标签
    m_plot->xAxis->setLabel("空间频率 (cycles/mm)");
    m_plot->yAxis->setLabel("MTF值");

    // 默认颜色列表
    QVector<QColor> defaultColors = {
        QColor(255, 0, 0),     // 红色
        QColor(0, 0, 255),     // 蓝色
        QColor(0, 255, 0),     // 绿色
        QColor(255, 0, 255),   // 洋红
        QColor(0, 255, 255),   // 青色
        QColor(255, 165, 0)    // 橙色
    };

    // 为每条线创建图形
    for (int i = 0; i < m_lines.size(); ++i) {
        const MTFLine& line = m_lines[i];

        QCPGraph* graph = m_plot->addGraph();

        // 准备数据
        QVector<double> x, y;
        for (const QPointF& point : line.data) {
            x.append(point.x());
            y.append(point.y());
        }

        graph->setData(x, y);

        // 设置线条属性
        QColor color = getColorFromName(line.colorName);
        if (!color.isValid()) {
            color = defaultColors[i % defaultColors.size()];
        }

        // 设置线条样式
        Qt::PenStyle style = Qt::SolidLine;
        if (i >= 3) style = Qt::DashLine; // 第4条线开始用虚线

        QPen pen(color, 2);
        pen.setStyle(style);
        graph->setPen(pen);

        // 设置图例名称
        QString legendName = QString("曲线 %1").arg(line.index);
        if (!line.label.isEmpty()) {
            legendName += ": " + line.label;
        }
        graph->setName(legendName);

        // 设置散点样式
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

        // 设置可见性
        graph->setVisible(line.visible);
    }

    m_plot->rescaleAxes();
    m_plot->replot();
}

QColor MTFViewer::getColorFromName(const QString& colorName)
{
    if (colorName == "RGBRed") return QColor(255, 0, 0);
    if (colorName == "RGBGreen") return QColor(0, 255, 0);
    if (colorName == "RGBBlue") return QColor(0, 0, 255);
    return QColor(); // 无效颜色
}

void MTFViewer::onLegendClick(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event)
{
    Q_UNUSED(legend)
        Q_UNUSED(event)

        if (item) {
            QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
            if (plItem) {
                bool visible = !plItem->plottable()->visible();
                plItem->plottable()->setVisible(visible);

                // 更新内部数据状态
                for (int i = 0; i < m_lines.size(); ++i) {
                    if (m_plot->graph(i) == plItem->plottable()) {
                        m_lines[i].visible = visible;
                        break;
                    }
                }

                m_plot->replot();
            }
        }
}

void MTFViewer::onSelectionChanged()
{
    // 处理图例选择变化
    m_plot->replot();
}