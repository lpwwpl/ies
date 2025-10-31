#include "MTFViewWidget.h"


#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

MTFViewer::MTFViewer(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , m_currentSelectedIndex(-1)
{
    setupUI();
    resize(QSize(640, 480));
}

void MTFViewer::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建绘图区域
    m_plot = new QCustomPlot(this);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
    m_plot->setMinimumSize(400, 300);

    // 启用上下文菜单
    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);

    // 设置坐标轴
    m_plot->xAxis->setLabel(QStringLiteral("空间频率(cycles/mm)"));
    m_plot->yAxis->setLabel(QStringLiteral("MTF值"));
    m_plot->xAxis->setRange(m_defaultXMin, m_defaultXMax);
    m_plot->yAxis->setRange(m_defaultYMin, m_defaultYMax);

    // 显示图例 - 设置为垂直分布
    m_plot->legend->setVisible(true);
    m_plot->legend->setFillOrder(QCPLegend::foRowsFirst); // 垂直排列
    m_plot->legend->setWrap(0); // 不换行，完全垂直

    layout->addWidget(m_plot);
    setLegendPosition(true);

    // 连接信号
    connect(m_plot->legend, &QCPLegend::selectionChanged, this, &MTFViewer::onSelectionChanged);
    connect(m_plot, &QCustomPlot::legendDoubleClick, this, &MTFViewer::onLegendClick);
    connect(m_plot, &QCustomPlot::mouseDoubleClick, this, &MTFViewer::onMouseDoubleClick);
    connect(m_plot, &QCustomPlot::plottableClick, this, &MTFViewer::onPlottableClick);

    // 连接自定义上下文菜单
    connect(m_plot, &QCustomPlot::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu menu(this);
        QAction* resetAction = menu.addAction("重置视图");
        QAction* legendLeftAction = menu.addAction("图例在左侧");
        QAction* legendRightAction = menu.addAction("图例在右侧");
        QAction* toggleLegendAction = menu.addAction(m_plot->legend->visible() ? "隐藏图例" : "显示图例");
        QAction* clearSelectionAction = menu.addAction("清除选择");

        QAction* selectedAction = menu.exec(m_plot->mapToGlobal(pos));

        if (selectedAction == resetAction) {
            resetView();
        }
        else if (selectedAction == legendLeftAction) {
            setLegendPosition(true);
        }
        else if (selectedAction == legendRightAction) {
            setLegendPosition(false);
        }
        else if (selectedAction == toggleLegendAction) {
            showLegend(!m_plot->legend->visible());
        }
        else if (selectedAction == clearSelectionAction) {
            clearAllSelections();
        }
        });
}

bool MTFViewer::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString(QStringLiteral("无法打开文件: %1")).arg(filename));
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
    m_currentSelectedIndex = -1;

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
            currentLine.selected = false;
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
    m_currentSelectedIndex = -1;
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

void MTFViewer::resetView()
{
    // 重置视图时显示曲线的整个x范围和y范围
    if (m_lines.isEmpty()) {
        // 如果没有数据，使用默认范围
        m_plot->xAxis->setRange(0, 200);
        m_plot->yAxis->setRange(0, 1.1);
    }
    else {
        // 计算所有可见曲线的数据范围
        double minX = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double minY = std::numeric_limits<double>::max();
        double maxY = std::numeric_limits<double>::lowest();

        bool hasVisibleData = false;

        for (const MTFLine& line : m_lines) {
            if (!line.visible || line.data.isEmpty()) continue;

            hasVisibleData = true;

            for (const QPointF& point : line.data) {
                if (point.x() < minX) minX = point.x();
                if (point.x() > maxX) maxX = point.x();
                if (point.y() < minY) minY = point.y();
                if (point.y() > maxY) maxY = point.y();
            }
        }

        if (hasVisibleData) {
            // 添加一些边距，使曲线不会紧贴坐标轴
            double xMargin = (maxX - minX) * 0.05;
            double yMargin = (maxY - minY) * 0.05;

            m_plot->xAxis->setRange(minX - xMargin, maxX + xMargin);
            m_plot->yAxis->setRange(minY - yMargin, maxY + yMargin);

            // 确保Y轴不会显示负值
            if (m_plot->yAxis->range().lower < 0) {
                m_plot->yAxis->setRange(0, m_plot->yAxis->range().upper);
            }
        }
        else {
            // 如果没有可见数据，使用默认范围
            m_plot->xAxis->setRange(0, 200);
            m_plot->yAxis->setRange(0, 1.1);
        }
    }

    m_plot->replot();
}

void MTFViewer::setTitle(const QString& title)
{
    // 清除现有的标题
    for (int i = 0; i < m_plot->plotLayout()->elementCount(); ++i) {
        if (auto textElement = dynamic_cast<QCPTextElement*>(m_plot->plotLayout()->elementAt(i))) {
            m_plot->plotLayout()->remove(textElement);
            delete textElement;
            break;
        }
    }

    // 添加新标题
    if (!title.isEmpty()) {
        m_plot->plotLayout()->insertRow(0);
        m_plot->plotLayout()->addElement(0, 0, new QCPTextElement(m_plot, title, QFont("sans", 12, QFont::Bold)));
    }
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
        MTFLine& line = m_lines[i];

        QCPGraph* graph = m_plot->addGraph();
        line.graph = graph; // 保存图形指针

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

        // 设置线条粗细 - 选中状态用粗线，未选中用正常线
        int penWidth = line.selected ? 4 : 2;
        QPen pen(color, penWidth);
        pen.setStyle(style);
        graph->setPen(pen);

        // 设置图例名称
        QString legendName = QString("Line %1").arg(line.index);
        if (!line.label.isEmpty()) {
            legendName += ": " + line.label;
        }
        graph->setName(legendName);

        // 设置散点样式
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

        // 设置可选择
        graph->setSelectable(QCP::stWhole);

        // 设置可见性
        graph->setVisible(line.visible);

        // 设置图例项文本样式 - 选中状态用粗体
        QFont legendFont = m_plot->legend->font();
        if (line.selected) {
            legendFont.setBold(true);
        }
        else {
            legendFont.setBold(false);
        }

        // 为图例项设置字体
        for (int j = 0; j < m_plot->legend->itemCount(); ++j) {
            if (QCPPlottableLegendItem* item = m_plot->legend->itemWithPlottable(graph)) {
                item->setFont(legendFont);
                break;
            }
        }
    }

    // 默认视图显示整个数据范围
    resetView();
}

void MTFViewer::setLegendPosition(bool leftSide)
{
    if (leftSide) {
        m_plot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipBorderAligned);
        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);
    }
    else {
        m_plot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipBorderAligned);
        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignTop);
    }

    // 设置图例大小，确保垂直分布时有足够空间
    m_plot->legend->setMaximumSize(180, 300);

    m_plot->replot();
}

void MTFViewer::showLegend(bool show)
{
    m_plot->legend->setVisible(show);
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
                // 查找对应的曲线索引
                for (int i = 0; i < m_lines.size(); ++i) {
                    if (m_plot->graph(i) == plItem->plottable()) {
                        // 切换选择状态
                        bool newSelectedState = !m_lines[i].selected;
                        clearAllSelections();
                        updateGraphSelection(i, newSelectedState);
                        break;
                    }
                }
            }
        }
}

void MTFViewer::onSelectionChanged()
{
    // 处理图例选择变化
    m_plot->replot();
}

void MTFViewer::onMouseDoubleClick(QMouseEvent* event)
{
    Q_UNUSED(event)
        // 双击重置视图
        resetView();
}

void MTFViewer::onPlottableClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    Q_UNUSED(dataIndex)
        Q_UNUSED(event)

        // 查找点击的曲线
        for (int i = 0; i < m_lines.size(); ++i) {
            if (m_lines[i].graph == plottable) {
                // 切换选择状态
                bool newSelectedState = !m_lines[i].selected;
                clearAllSelections();
                updateGraphSelection(i, newSelectedState);
                break;
            }
        }
}

void MTFViewer::updateGraphSelection(int graphIndex, bool selected)
{
    if (graphIndex < 0 || graphIndex >= m_lines.size()) {
        return;
    }

    MTFLine& line = m_lines[graphIndex];
    line.selected = selected;

    if (selected) {
        m_currentSelectedIndex = graphIndex;
    }
    else {
        m_currentSelectedIndex = -1;
    }

    // 更新线条样式
    if (line.graph) {
        QColor color = getColorFromName(line.colorName);
        if (!color.isValid()) {
            QVector<QColor> defaultColors = {
                QColor(255, 0, 0), QColor(0, 0, 255), QColor(0, 255, 0),
                QColor(255, 0, 255), QColor(0, 255, 255), QColor(255, 165, 0)
            };
            color = defaultColors[graphIndex % defaultColors.size()];
        }

        // 设置线条粗细 - 选中状态用粗线，未选中用正常线
        int penWidth = selected ? 4 : 2;
        QPen pen = line.graph->pen();
        pen.setWidth(penWidth);
        pen.setColor(color);
        line.graph->setPen(pen);
    }

    // 更新图例项样式
    QFont legendFont = m_plot->legend->font();
    if (selected) {
        legendFont.setBold(true);
    }
    else {
        legendFont.setBold(false);
    }

    // 为图例项设置字体
    for (int j = 0; j < m_plot->legend->itemCount(); ++j) {
        QCPAbstractLegendItem* item = m_plot->legend->item(j);
        if (QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item)) {
            if (plItem->plottable() == line.graph) {
                plItem->setFont(legendFont);
                break;
            }
        }
    }

    m_plot->replot();
}

void MTFViewer::clearAllSelections()
{
    for (int i = 0; i < m_lines.size(); ++i) {
        if (m_lines[i].selected) {
            updateGraphSelection(i, false);
        }
    }
    m_currentSelectedIndex = -1;
}