//#include "MTFViewWidget.h"
//
//
//#include <QVBoxLayout>
//#include <QFile>
//#include <QTextStream>
//#include <QDebug>
//#include <QMouseEvent>
//#include <QMenu>
//#include <QAction>
//#include <QNetworkInterface>
//
//MTFViewer::MTFViewer(QWidget* parent)
//    : QWidget(parent)
//    , m_plot(nullptr)
//    , m_currentSelectedIndex(-1)
//{
//    setupUI();
//    resize(QSize(640, 480));
//}
//
//void MTFViewer::setupUI()
//{
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//
//    // 创建绘图区域
//    m_plot = new QCustomPlot(this);
//    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
//    m_plot->setMinimumSize(400, 300);
//
//    // 启用上下文菜单
//    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);
//
//    // 设置坐标轴
//    m_plot->xAxis->setLabel(QStringLiteral("空间频率(cycles/mm)"));
//    m_plot->yAxis->setLabel(QStringLiteral("MTF值"));
//    m_plot->xAxis->setRange(m_defaultXMin, m_defaultXMax);
//    m_plot->yAxis->setRange(m_defaultYMin, m_defaultYMax);
//
//    // 显示图例 - 设置为垂直分布
//    m_plot->legend->setVisible(true);
//    m_plot->legend->setFillOrder(QCPLegend::foRowsFirst); // 垂直排列
//    m_plot->legend->setWrap(0); // 不换行，完全垂直
//
//    layout->addWidget(m_plot);
//    setLegendPosition(true);
//
//    // 连接信号
//    connect(m_plot->legend, &QCPLegend::selectionChanged, this, &MTFViewer::onSelectionChanged);
//    connect(m_plot, &QCustomPlot::legendDoubleClick, this, &MTFViewer::onLegendClick);
//    connect(m_plot, &QCustomPlot::mouseDoubleClick, this, &MTFViewer::onMouseDoubleClick);
//    connect(m_plot, &QCustomPlot::plottableClick, this, &MTFViewer::onPlottableClick);
//
//    // 连接自定义上下文菜单
//    connect(m_plot, &QCustomPlot::customContextMenuRequested, this, [this](const QPoint& pos) {
//        QMenu menu(this);
//        QAction* resetAction = menu.addAction("重置视图");
//        QAction* legendLeftAction = menu.addAction("图例在左侧");
//        QAction* legendRightAction = menu.addAction("图例在右侧");
//        QAction* toggleLegendAction = menu.addAction(m_plot->legend->visible() ? "隐藏图例" : "显示图例");
//        QAction* clearSelectionAction = menu.addAction("清除选择");
//
//        QAction* selectedAction = menu.exec(m_plot->mapToGlobal(pos));
//
//        if (selectedAction == resetAction) {
//            resetView();
//        }
//        else if (selectedAction == legendLeftAction) {
//            setLegendPosition(true);
//        }
//        else if (selectedAction == legendRightAction) {
//            setLegendPosition(false);
//        }
//        else if (selectedAction == toggleLegendAction) {
//            showLegend(!m_plot->legend->visible());
//        }
//        else if (selectedAction == clearSelectionAction) {
//            clearAllSelections();
//        }
//        });
//}
//
//bool MTFViewer::loadFromFile(const QString& filename)
//{
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        emit errorOccurred(QString(QStringLiteral("无法打开文件: %1")).arg(filename));
//        return false;
//    }
//
//    QTextStream in(&file);
//    QString data = in.readAll();
//    file.close();
//
//    m_currentFile = filename;
//    return loadFromString(data);
//}
//
//bool MTFViewer::loadFromString(const QString& data)
//{
//    m_lines.clear();
//    m_currentSelectedIndex = -1;
//
//    QStringList lines = data.split('\n', Qt::SkipEmptyParts);
//
//    MTFLine currentLine;
//    bool readingData = false;
//    bool hasValidData = false;
//
//    for (const QString& line : lines) {
//        if (line.startsWith("# CHART NAME:")) {
//            // 新图表开始，如果当前有数据，保存前一条线
//            if (readingData && !currentLine.data.isEmpty()) {
//                m_lines.append(currentLine);
//                currentLine = MTFLine();
//                hasValidData = true;
//            }
//            readingData = false;
//        }
//        else if (line.startsWith("  LINE INDEX:")) {
//            // 新线条开始
//            if (readingData && !currentLine.data.isEmpty()) {
//                m_lines.append(currentLine);
//                hasValidData = true;
//            }
//            currentLine = MTFLine();
//            currentLine.index = line.split(":").last().trimmed().toInt();
//            currentLine.visible = true;
//            currentLine.selected = false;
//            readingData = false;
//        }
//        else if (line.startsWith("  LINE LABEL:")) {
//            currentLine.label = line.split(":").last().trimmed();
//        }
//        else if (line.startsWith("  LINE COLOR:")) {
//            currentLine.colorName = line.split(":").last().trimmed();
//        }
//        else if (line.startsWith("     INDEX")) {
//            // 数据表头，下一行开始是数据
//            readingData = true;
//        }
//        else if (readingData && !line.trimmed().isEmpty() && !line.startsWith("#")) {
//            // 读取数据行
//            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
//            if (parts.size() >= 3) {
//                bool ok1, ok2;
//                double x = parts[1].toDouble(&ok1);
//                double y = parts[2].toDouble(&ok2);
//                if (ok1 && ok2) {
//                    currentLine.data.append(QPointF(x, y));
//                    hasValidData = true;
//                }
//            }
//        }
//    }
//
//    // 添加最后一条线
//    if (!currentLine.data.isEmpty()) {
//        m_lines.append(currentLine);
//        hasValidData = true;
//    }
//
//    if (hasValidData) {
//        plotData();
//        emit dataLoaded(true);
//        return true;
//    }
//    else {
//        emit errorOccurred("未找到有效的MTF数据");
//        return false;
//    }
//}
//
//void MTFViewer::clearData()
//{
//    m_lines.clear();
//    m_plot->clearGraphs();
//    m_plot->replot();
//    m_currentFile.clear();
//    m_currentSelectedIndex = -1;
//}
//
//void MTFViewer::setXRange(double min, double max)
//{
//    m_plot->xAxis->setRange(min, max);
//    m_plot->replot();
//}
//
//void MTFViewer::setYRange(double min, double max)
//{
//    m_plot->yAxis->setRange(min, max);
//    m_plot->replot();
//}
//
//void MTFViewer::resetView()
//{
//    // 重置视图时显示曲线的整个x范围和y范围
//    if (m_lines.isEmpty()) {
//        // 如果没有数据，使用默认范围
//        m_plot->xAxis->setRange(0, 200);
//        m_plot->yAxis->setRange(0, 1.1);
//    }
//    else {
//        // 计算所有可见曲线的数据范围
//        double minX = std::numeric_limits<double>::max();
//        double maxX = std::numeric_limits<double>::lowest();
//        double minY = std::numeric_limits<double>::max();
//        double maxY = std::numeric_limits<double>::lowest();
//
//        bool hasVisibleData = false;
//
//        for (const MTFLine& line : m_lines) {
//            if (!line.visible || line.data.isEmpty()) continue;
//
//            hasVisibleData = true;
//
//            for (const QPointF& point : line.data) {
//                if (point.x() < minX) minX = point.x();
//                if (point.x() > maxX) maxX = point.x();
//                if (point.y() < minY) minY = point.y();
//                if (point.y() > maxY) maxY = point.y();
//            }
//        }
//
//        if (hasVisibleData) {
//            // 添加一些边距，使曲线不会紧贴坐标轴
//            double xMargin = (maxX - minX) * 0.05;
//            double yMargin = (maxY - minY) * 0.05;
//
//            m_plot->xAxis->setRange(minX - xMargin, maxX + xMargin);
//            m_plot->yAxis->setRange(minY - yMargin, maxY + yMargin);
//
//            // 确保Y轴不会显示负值
//            if (m_plot->yAxis->range().lower < 0) {
//                m_plot->yAxis->setRange(0, m_plot->yAxis->range().upper);
//            }
//        }
//        else {
//            // 如果没有可见数据，使用默认范围
//            m_plot->xAxis->setRange(0, 200);
//            m_plot->yAxis->setRange(0, 1.1);
//        }
//    }
//
//    m_plot->replot();
//}
//
//void MTFViewer::setTitle(const QString& title)
//{
//    // 清除现有的标题
//    for (int i = 0; i < m_plot->plotLayout()->elementCount(); ++i) {
//        if (auto textElement = dynamic_cast<QCPTextElement*>(m_plot->plotLayout()->elementAt(i))) {
//            m_plot->plotLayout()->remove(textElement);
//            delete textElement;
//            break;
//        }
//    }
//
//    // 添加新标题
//    if (!title.isEmpty()) {
//        m_plot->plotLayout()->insertRow(0);
//        m_plot->plotLayout()->addElement(0, 0, new QCPTextElement(m_plot, title, QFont("sans", 12, QFont::Bold)));
//    }
//    m_plot->replot();
//}
//
//void MTFViewer::plotData()
//{
//    m_plot->clearGraphs();
//
//    // 设置坐标轴标签
//    m_plot->xAxis->setLabel("空间频率 (cycles/mm)");
//    m_plot->yAxis->setLabel("MTF值");
//
//    // 默认颜色列表
//    QVector<QColor> defaultColors = {
//        QColor(255, 0, 0),     // 红色
//        QColor(0, 0, 255),     // 蓝色
//        QColor(0, 255, 0),     // 绿色
//        QColor(255, 0, 255),   // 洋红
//        QColor(0, 255, 255),   // 青色
//        QColor(255, 165, 0)    // 橙色
//    };
//
//    // 为每条线创建图形
//    for (int i = 0; i < m_lines.size(); ++i) {
//        MTFLine& line = m_lines[i];
//
//        QCPGraph* graph = m_plot->addGraph();
//        line.graph = graph; // 保存图形指针
//
//        // 准备数据
//        QVector<double> x, y;
//        for (const QPointF& point : line.data) {
//            x.append(point.x());
//            y.append(point.y());
//        }
//
//        graph->setData(x, y);
//
//        // 设置线条属性
//        QColor color = getColorFromName(line.colorName);
//        if (!color.isValid()) {
//            color = defaultColors[i % defaultColors.size()];
//        }
//
//        // 设置线条样式
//        Qt::PenStyle style = Qt::SolidLine;
//        if (i >= 3) style = Qt::DashLine; // 第4条线开始用虚线
//
//        // 设置线条粗细 - 选中状态用粗线，未选中用正常线
//        int penWidth = line.selected ? 4 : 2;
//        QPen pen(color, penWidth);
//        pen.setStyle(style);
//        graph->setPen(pen);
//
//        // 设置图例名称
//        QString legendName = QString("Line %1").arg(line.index);
//        if (!line.label.isEmpty()) {
//            legendName += ": " + line.label;
//        }
//        graph->setName(legendName);
//
//        // 设置散点样式
//        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
//
//        // 设置可选择
//        graph->setSelectable(QCP::stWhole);
//
//        // 设置可见性
//        graph->setVisible(line.visible);
//
//        // 设置图例项文本样式 - 选中状态用粗体
//        QFont legendFont = m_plot->legend->font();
//        if (line.selected) {
//            legendFont.setBold(true);
//        }
//        else {
//            legendFont.setBold(false);
//        }
//
//        // 为图例项设置字体
//        for (int j = 0; j < m_plot->legend->itemCount(); ++j) {
//            if (QCPPlottableLegendItem* item = m_plot->legend->itemWithPlottable(graph)) {
//                item->setFont(legendFont);
//                break;
//            }
//        }
//    }
//
//    // 默认视图显示整个数据范围
//    resetView();
//}
//
//void MTFViewer::setLegendPosition(bool leftSide)
//{
//    if (leftSide) {
//        m_plot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipBorderAligned);
//        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);
//    }
//    else {
//        m_plot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipBorderAligned);
//        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignTop);
//    }
//
//    // 设置图例大小，确保垂直分布时有足够空间
//    m_plot->legend->setMaximumSize(180, 300);
//
//    m_plot->replot();
//}
//
//void MTFViewer::showLegend(bool show)
//{
//    m_plot->legend->setVisible(show);
//    m_plot->replot();
//}
//
//QColor MTFViewer::getColorFromName(const QString& colorName)
//{
//    if (colorName == "RGBRed") return QColor(255, 0, 0);
//    if (colorName == "RGBGreen") return QColor(0, 255, 0);
//    if (colorName == "RGBBlue") return QColor(0, 0, 255);
//    return QColor(); // 无效颜色
//}
//
//void MTFViewer::onLegendClick(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event)
//{
//    Q_UNUSED(legend)
//        Q_UNUSED(event)
//
//        if (item) {
//            QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
//            if (plItem) {
//                // 查找对应的曲线索引
//                for (int i = 0; i < m_lines.size(); ++i) {
//                    if (m_plot->graph(i) == plItem->plottable()) {
//                        // 切换选择状态
//                        bool newSelectedState = !m_lines[i].selected;
//                        clearAllSelections();
//                        updateGraphSelection(i, newSelectedState);
//                        break;
//                    }
//                }
//            }
//        }
//}
//
//void MTFViewer::onSelectionChanged()
//{
//    // 处理图例选择变化
//    m_plot->replot();
//}
//
//void MTFViewer::onMouseDoubleClick(QMouseEvent* event)
//{
//    Q_UNUSED(event)
//        // 双击重置视图
//        resetView();
//}
//
//void MTFViewer::onPlottableClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
//{
//    Q_UNUSED(dataIndex)
//        Q_UNUSED(event)
//
//        // 查找点击的曲线
//        for (int i = 0; i < m_lines.size(); ++i) {
//            if (m_lines[i].graph == plottable) {
//                // 切换选择状态
//                bool newSelectedState = !m_lines[i].selected;
//                clearAllSelections();
//                updateGraphSelection(i, newSelectedState);
//                break;
//            }
//        }
//}
//
//void MTFViewer::updateGraphSelection(int graphIndex, bool selected)
//{
//    if (graphIndex < 0 || graphIndex >= m_lines.size()) {
//        return;
//    }
//
//    MTFLine& line = m_lines[graphIndex];
//    line.selected = selected;
//
//    if (selected) {
//        m_currentSelectedIndex = graphIndex;
//    }
//    else {
//        m_currentSelectedIndex = -1;
//    }
//
//    // 更新线条样式
//    if (line.graph) {
//        QColor color = getColorFromName(line.colorName);
//        if (!color.isValid()) {
//            QVector<QColor> defaultColors = {
//                QColor(255, 0, 0), QColor(0, 0, 255), QColor(0, 255, 0),
//                QColor(255, 0, 255), QColor(0, 255, 255), QColor(255, 165, 0)
//            };
//            color = defaultColors[graphIndex % defaultColors.size()];
//        }
//
//        // 设置线条粗细 - 选中状态用粗线，未选中用正常线
//        int penWidth = selected ? 4 : 2;
//        QPen pen = line.graph->pen();
//        pen.setWidth(penWidth);
//        pen.setColor(color);
//        line.graph->setPen(pen);
//    }
//
//    // 更新图例项样式
//    QFont legendFont = m_plot->legend->font();
//    if (selected) {
//        legendFont.setBold(true);
//    }
//    else {
//        legendFont.setBold(false);
//    }
//
//    // 为图例项设置字体
//    for (int j = 0; j < m_plot->legend->itemCount(); ++j) {
//        QCPAbstractLegendItem* item = m_plot->legend->item(j);
//        if (QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item)) {
//            if (plItem->plottable() == line.graph) {
//                plItem->setFont(legendFont);
//                break;
//            }
//        }
//    }
//
//    m_plot->replot();
//}
//
//void MTFViewer::clearAllSelections()
//{
//    for (int i = 0; i < m_lines.size(); ++i) {
//        if (m_lines[i].selected) {
//            updateGraphSelection(i, false);
//        }
//    }
//    m_currentSelectedIndex = -1;
//}
#include "MTFViewWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <cmath>
#include <limits>
#include <QPen>
#include <qwt_text.h>
#include <qwt_symbol.h>
MTFViewer::MTFViewer(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
    , m_picker(nullptr)
    , m_legend(nullptr)
    , m_currentSelectedIndex(-1)
{
    // 初始化默认颜色
    m_defaultColors = {
        QColor(255, 0, 0),     // 红色
        QColor(0, 0, 255),     // 蓝色
        QColor(0, 255, 0),     // 绿色
        QColor(255, 0, 255),   // 洋红
        QColor(0, 255, 255),   // 青色
        QColor(255, 165, 0)    // 橙色
    };

    setupUI();
    setupInteractions();
    setMinimumSize(640, 480);
}

MTFViewer::~MTFViewer()
{
    clearData();
}

void MTFViewer::setupUI()
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建绘图区域
    m_plot = new QwtPlot(this);
    m_plot->setCanvasBackground(Qt::white);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
    m_plot->setMinimumSize(400, 300);

    // 设置坐标轴
    m_plot->setAxisTitle(QwtPlot::xBottom, "空间频率 (cycles/mm)");
    m_plot->setAxisTitle(QwtPlot::yLeft, "MTF值");
    m_plot->setAxisScale(QwtPlot::xBottom, m_defaultXMin, m_defaultXMax);
    m_plot->setAxisScale(QwtPlot::yLeft, m_defaultYMin, m_defaultYMax);

    // 设置网格
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(QPen(Qt::lightGray, 0.5, Qt::SolidLine));
    grid->setMinorPen(QPen(Qt::lightGray, 0.5, Qt::DotLine));
    grid->attach(m_plot);

    // 显示图例
    m_legend = new QwtLegend();
    m_legend->setDefaultItemMode(QwtLegendData::Checkable);
    m_plot->insertLegend(m_legend, QwtPlot::RightLegend);

    // 设置图例样式
    m_legend->setFont(QFont("Helvetica", 9));
    
    layout->addWidget(m_plot);
    setLegendPosition(true);

    // 启用右键菜单
    m_plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接自定义上下文菜单
    connect(m_plot->canvas(), &QWidget::customContextMenuRequested,
        this, &MTFViewer::showContextMenu);

    // 连接双击事件
    //connect(m_plot->canvas(), &QWidget::mouseDoubleClick,
    //    this, &MTFViewer::onMouseDoubleClick);
}

void MTFViewer::setupInteractions()
{
    // 创建拖拽功能
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    // 创建缩放功能
    m_magnifier = new QwtPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);

    // 创建选择功能
    m_picker = new QwtPlotPicker(
        QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand,
        QwtPicker::AlwaysOn,
        m_plot->canvas()
    );

    // 设置选择机器
    m_picker->setStateMachine(new QwtPickerDragPointMachine());

    // 设置跟踪器
    m_picker->setTrackerPen(QColor(Qt::black));
    m_picker->setTrackerFont(QFont("Arial", 10));

    // 连接点击信号
    connect(m_picker, SIGNAL(selected(const QPointF&)),
        this, SLOT(onPlotClicked(const QPointF&)));
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
    m_currentSelectedIndex = -1;

    QStringList lines = data.split('\n', QString::SkipEmptyParts);

    MTFLine currentLine;
    bool readingData = false;
    bool hasValidData = false;

    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();

        if (trimmedLine.startsWith("# CHART NAME:")) {
            // 新图表开始，如果当前有数据，保存前一条线
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                currentLine = MTFLine();
                hasValidData = true;
            }
            readingData = false;
        }
        else if (trimmedLine.startsWith("LINE INDEX:")) {
            // 新线条开始
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                hasValidData = true;
            }
            currentLine = MTFLine();
            currentLine.index = trimmedLine.split(":").last().trimmed().toInt();
            currentLine.visible = true;
            currentLine.selected = false;
            readingData = false;
        }
        else if (trimmedLine.startsWith("LINE LABEL:")) {
            currentLine.label = trimmedLine.split(":").last().trimmed();
        }
        else if (trimmedLine.startsWith("LINE COLOR:")) {
            currentLine.colorName = trimmedLine.split(":").last().trimmed();
        }
        else if (trimmedLine.startsWith("INDEX")) {
            // 数据表头，下一行开始是数据
            readingData = true;
        }
        else if (readingData && !trimmedLine.isEmpty() && !trimmedLine.startsWith("#")) {
            // 读取数据行
            QStringList parts = trimmedLine.split(' ', QString::SkipEmptyParts);
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
    // 删除所有曲线
    for (auto& line : m_lines) {
        if (line.curve) {
            line.curve->detach();
            delete line.curve;
            line.curve = nullptr;
        }
    }

    m_lines.clear();
    m_plot->replot();
    m_currentFile.clear();
    m_currentSelectedIndex = -1;
}

void MTFViewer::setXRange(double min, double max)
{
    m_plot->setAxisScale(QwtPlot::xBottom, min, max);
    m_plot->replot();
}

void MTFViewer::setYRange(double min, double max)
{
    m_plot->setAxisScale(QwtPlot::yLeft, min, max);
    m_plot->replot();
}

void MTFViewer::resetView()
{
    if (m_lines.isEmpty()) {
        // 如果没有数据，使用默认范围
        m_plot->setAxisScale(QwtPlot::xBottom, m_defaultXMin, m_defaultXMax);
        m_plot->setAxisScale(QwtPlot::yLeft, m_defaultYMin, m_defaultYMax);
    }
    else {
        // 使用自动缩放
        autoScaleAxes();
    }

    m_plot->replot();
}

void MTFViewer::autoScaleAxes()
{
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

        // 确保边距不为零
        if (xMargin <= 0) xMargin = maxX * 0.1;
        if (yMargin <= 0) yMargin = maxY * 0.1;

        if (xMargin <= 0) xMargin = 1.0;
        if (yMargin <= 0) yMargin = 0.1;

        m_plot->setAxisScale(QwtPlot::xBottom, minX - xMargin, maxX + xMargin);
        m_plot->setAxisScale(QwtPlot::yLeft, std::max(0.0, minY - yMargin), maxY + yMargin);
    }
    else {
        // 如果没有可见数据，使用默认范围
        m_plot->setAxisScale(QwtPlot::xBottom, m_defaultXMin, m_defaultXMax);
        m_plot->setAxisScale(QwtPlot::yLeft, m_defaultYMin, m_defaultYMax);
    }
}

void MTFViewer::setTitle(const QString& title)
{
    QwtText plotTitle(title);
    plotTitle.setFont(QFont("sans", 12, QFont::Bold));
    m_plot->setTitle(plotTitle);
    m_plot->replot();
}

void MTFViewer::plotData()
{
    // 清除现有的曲线
    for (auto& line : m_lines) {
        if (line.curve) {
            line.curve->detach();
            delete line.curve;
            line.curve = nullptr;
        }
    }

    // 为每条线创建曲线
    for (int i = 0; i < m_lines.size(); ++i) {
        MTFLine& line = m_lines[i];

        // 准备数据
        QVector<QPointF> points = line.data;

        // 创建曲线
        line.curve = new QwtPlotCurve();//QString("Field %1").arg(i)

        // 设置数据
        line.curve->setSamples(points);

        // 设置线条属性
        QColor color = getColorFromName(line.colorName);
        if (!color.isValid()) {
            color = m_defaultColors[i % m_defaultColors.size()];
        }

        // 设置线条样式
        Qt::PenStyle style = Qt::SolidLine;
        if (i >= 3) style = Qt::DashLine; // 第4条线开始用虚线

        // 设置线条粗细
        int penWidth = line.selected ? 4 : 2;
        QPen pen(color, penWidth, style);
        line.curve->setPen(pen);
        line.curve->setRenderHint(QwtPlotItem::RenderAntialiased);

        // 设置图例名称
        QString legendName = QString("Line %1").arg(line.index);
        if (!line.label.isEmpty()) {
            legendName += ": " + line.label;
        }
        line.curve->setTitle(legendName);
        line.curve->setItemAttribute(QwtPlotItem::Legend, true);

        // 设置可见性
        line.curve->setVisible(line.visible);

        // 附加到图表
        line.curve->attach(m_plot);
    }

    // 连接图例点击信号
    if (m_legend) {
        bool ret = connect(m_legend, SIGNAL(checked(const QVariant&, bool , int )),
            this, SLOT(onLegendChecked(const QVariant&, bool, int)));

        ret = false;
    }

    // 默认视图显示整个数据范围
    autoScaleAxes();
    m_plot->replot();
}

void MTFViewer::setLegendPosition(bool leftSide)
{
    if (leftSide) {
        m_plot->insertLegend(m_legend, QwtPlot::LeftLegend);
    }
    else {
        m_plot->insertLegend(m_legend, QwtPlot::RightLegend);
    }

    // 重新连接图例信号

    if (m_legend) {
        disconnect(m_legend, SIGNAL(checked(const QVariant&, bool, int)),
            this, SLOT(onLegendChecked(const QVariant&, bool, int)));
        connect(m_legend, SIGNAL(checked(const QVariant&, bool, int)),
            this, SLOT(onLegendChecked(const QVariant&, bool,int)));
    }

    m_plot->replot();
}

void MTFViewer::showLegend(bool show)
{
    if (m_legend) {
        m_legend->setVisible(show);
        //if (!show)
        //{

        //}
        //else
        //{

        //}
        m_plot->replot();      
    }
}

QColor MTFViewer::getColorFromName(const QString& colorName)
{
    if (colorName == "RGBRed") return QColor(255, 0, 0);
    if (colorName == "RGBGreen") return QColor(0, 255, 0);
    if (colorName == "RGBBlue") return QColor(0, 0, 255);
    if (colorName == "RGBOrange") return QColor(255, 165, 0);
    if (colorName == "RGBYellow") return QColor(255, 255, 0);
    if (colorName == "RGBCyan") return QColor(0, 255, 255);
    return QColor(); // 无效颜色
}

void MTFViewer::onLegendChecked(const QVariant& itemInfo, bool on, int)
{
    // 查找对应的曲线
    QwtPlotItem* item = m_plot->infoToItem(itemInfo);
    if (item && item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
        QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);

        // 更新曲线可见性
        //curve->set(on);
  

        // 更新数据中的可见性
        for (int i = 0; i < m_lines.size(); ++i) {
            if (m_lines[i].curve == curve) {
                updateCurveSelection(i,on);
                //m_lines[i].selected = on;
                break;
            }
        }

        m_plot->replot();
    }
}

void MTFViewer::onPlotClicked(const QPointF& pos)
{
    // 查找最近的曲线
    double minDistance = std::numeric_limits<double>::max();
    QwtPlotCurve* nearestCurve = nullptr;
    int nearestIndex = -1;

    for (int i = 0; i < m_lines.size(); ++i) {
        if (!m_lines[i].visible) continue;

        QwtPlotCurve* curve = m_lines[i].curve;
        if (!curve) continue;

        // 计算曲线到点击位置的距离
        // 简化处理：只检查数据点
        for (int j = 0; j < curve->dataSize(); ++j) {
            QPointF point = curve->sample(j);
            double distance = sqrt(pow(point.x() - pos.x(), 2) + pow(point.y() - pos.y(), 2));

            if (distance < minDistance) {
                minDistance = distance;
                nearestCurve = curve;
                nearestIndex = i;
            }
        }
    }

    // 如果距离足够近，选择该曲线
    if (nearestCurve && minDistance < 5.0) { // 5像素阈值
        // 切换选择状态
        bool newSelected = !m_lines[nearestIndex].selected;
        clearAllSelections();
        updateCurveSelection(nearestIndex, newSelected);
    }
}

void MTFViewer::onMouseDoubleClick()
{
    resetView();
}

void MTFViewer::showContextMenu(const QPoint& pos)
{
    QMenu menu(this);
    QAction* resetAction = menu.addAction("重置视图");
    QAction* legendLeftAction = menu.addAction("图例在左侧");
    QAction* legendRightAction = menu.addAction("图例在右侧");

    QAction* toggleLegendAction = nullptr;
    if (m_legend) {
        toggleLegendAction = menu.addAction(m_legend->isVisible() ? "隐藏图例" : "显示图例");
    }

    QAction* clearSelectionAction = menu.addAction("清除选择");

    QAction* selectedAction = menu.exec(m_plot->canvas()->mapToGlobal(pos));

    if (selectedAction == resetAction) {
        resetView();
    }
    else if (selectedAction == legendLeftAction) {
        setLegendPosition(true);
    }
    else if (selectedAction == legendRightAction) {
        setLegendPosition(false);
    }
    else if (selectedAction == toggleLegendAction && m_legend) {
        showLegend(!m_legend->isVisible());
    }
    else if (selectedAction == clearSelectionAction) {
        clearAllSelections();
    }
}

void MTFViewer::updateCurveSelection(int curveIndex, bool selected)
{
    if (curveIndex < 0 || curveIndex >= m_lines.size()) {
        return;
    }

    MTFLine& line = m_lines[curveIndex];
    line.selected = selected;

    if (selected) {
        m_currentSelectedIndex = curveIndex;
    }
    else {
        m_currentSelectedIndex = -1;
    }

    // 更新线条样式
    if (line.curve) {
        QColor color = getColorFromName(line.colorName);
        if (!color.isValid()) {
            color = m_defaultColors[curveIndex % m_defaultColors.size()];
        }

        // 设置线条粗细
        int penWidth = selected ? 4 : 2;
        QPen pen = line.curve->pen();
        pen.setWidth(penWidth);
        pen.setColor(color);
        line.curve->setPen(pen);

        // 更新图例项样式
        updateLegendSelection(line.curve, selected);
    }

    m_plot->replot();
}

void MTFViewer::clearAllSelections()
{
    for (int i = 0; i < m_lines.size(); ++i) {
        if (m_lines[i].selected) {
            updateCurveSelection(i, false);
        }
    }
    m_currentSelectedIndex = -1;
}

void MTFViewer::updateLegendSelection(QwtPlotCurve* curve, bool selected)
{
    if (!m_legend || !curve) return;

    // 更新图例项的字体样式
    QList<QWidget*> legendWidgets = m_legend->legendWidgets(curve->plot()->itemToInfo(curve));
    for (QWidget* w : legendWidgets) {
        if (QwtLegendLabel* label = qobject_cast<QwtLegendLabel*>(w)) {
            QFont font = label->font();
            font.setBold(selected);
            label->setFont(font);
        }
    }
}