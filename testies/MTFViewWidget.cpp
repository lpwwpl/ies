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
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <cmath>
#include <limits>
#include <QPen>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_plot_renderer.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>

MTFViewer::MTFViewer(QWidget* parent)
    : QWidget(parent)
    , m_toolBar_plot(nullptr)
    , m_splitter(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
    , m_picker(nullptr)
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
    setMinimumSize(800, 600);

    //QCoreApplication::setOrganizationName("MySoft");
    QCoreApplication::setApplicationName("MTFViewer");

    //m_simple_browser->m_grid = m_grid;
    //m_simple_browser->m_legend = m_legend;

    //loadSettings();      // 加载保存的设置
    //applySettings();     // 应用到绘图
}

MTFViewer::~MTFViewer()
{
    //saveSettings();
    clearData();
}

void MTFViewer::setupUI()
{
    // 属性浏览器
    //m_browser = new QwtPropertyBrowser(this);
    //m_browser->setPlot(m_plot);
    //connect(m_browser, &QwtPropertyBrowser::propertyChanged, this, &MTFViewer::syncPlotFromBrowser);
    m_toolBar_plot = new PlotBase(this);
    m_plot = m_toolBar_plot->m_plot;
    m_grid = m_toolBar_plot->m_grid;
    m_legend = m_toolBar_plot->m_legend;
    m_settings = m_toolBar_plot->m_settings;
    connect(m_toolBar_plot, SIGNAL(signalFitView()), this, SLOT(fitView()));
    connect(m_toolBar_plot, SIGNAL(signalZoomIn()), this, SLOT(zoomIn()));
    connect(m_toolBar_plot, SIGNAL(signalZommOut()), this, SLOT(zoomOut()));

    m_simple_browser = new QwtPropertyBrowser(m_settings, m_plot,m_grid,m_legend, this);
    //m_simple_browser = new SimplePropertyBrowser(m_settings,m_plot,this);
    // 分割器
    m_splitter = new QSplitter(this);
    m_splitter->addWidget(m_toolBar_plot);
    m_splitter->addWidget(m_simple_browser);

    m_splitter->setStretchFactor(0, 1); 
    m_splitter->setStretchFactor(1, 0); 
    m_simple_browser->setMinimumWidth(300);
    m_simple_browser->setMaximumWidth(400);
    m_splitter->setCollapsible(1,false);
    m_splitter->setSizes({ 900, 300 });


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_splitter);

    // 右键菜单
    m_plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_plot->canvas(), &QWidget::customContextMenuRequested,
        this, &MTFViewer::showContextMenu);
    // 双击重置
    m_plot->canvas()->installEventFilter(this);
}
void MTFViewer::setupInteractions()
{
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    m_magnifier = new QwtPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);

    m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand,
        QwtPicker::AlwaysOn,
        m_plot->canvas());
    m_picker->setStateMachine(new QwtPickerDragPointMachine());
    m_picker->setTrackerPen(QColor(Qt::black));
    m_picker->setTrackerFont(QFont("Arial", 10));
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
    return loadFromString(data);
}

bool MTFViewer::loadFromString(const QString& data)
{
    m_settings->m_lines.clear();
    m_currentSelectedIndex = -1;

    QStringList lines = data.split('\n', QString::SkipEmptyParts);
    MTFLine currentLine;
    bool readingData = false;
    bool hasValidData = false;

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("# CHART NAME:")) {
            if (readingData && !currentLine.data.isEmpty()) {
                m_settings->m_lines[currentLine.index]= (currentLine);
                currentLine = MTFLine();
                hasValidData = true;
            }
            readingData = false;
        }
        else if (trimmed.startsWith("LINE INDEX:")) {
            if (readingData && !currentLine.data.isEmpty()) {
                m_settings->m_lines[currentLine.index] = (currentLine);
                hasValidData = true;
            }
            currentLine = MTFLine();
            currentLine.index = trimmed.split(":").last().trimmed().toInt();
            currentLine.m_style.visible = true;
            readingData = false;
        }
        else if (trimmed.startsWith("LINE LABEL:")) {
            currentLine.label = trimmed.split(":").last().trimmed();
        }
        else if (trimmed.startsWith("LINE COLOR:")) {
            currentLine.colorName = trimmed.split(":").last().trimmed();
            currentLine.m_style.lineColor = getColorFromName(currentLine.colorName);
            currentLine.m_style.pointColor = currentLine.m_style.lineColor;
        }
        else if (trimmed.startsWith("INDEX")) {
            readingData = true;
        }
        else if (readingData && !trimmed.isEmpty() && !trimmed.startsWith("#")) {
            QStringList parts = trimmed.split(' ', QString::SkipEmptyParts);
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

    if (!currentLine.data.isEmpty()) {
        m_settings->m_lines[currentLine.index] = (currentLine);
        hasValidData = true;
    }

    if (hasValidData) {   
        plotData();
        m_simple_browser->updateLineCombo();
        m_simple_browser->setCurrentCurve(0);
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
    for (auto& line : m_settings->m_lines) {
        if (line.curve) {
            line.curve->detach();
            delete line.curve;
            line.curve = nullptr;
        }
    }
    m_settings->m_lines.clear();
    m_plot->replot();
    m_currentSelectedIndex = -1;
}

void MTFViewer::plotData()
{
    // 清除旧曲线
    for (auto& line : m_settings->m_lines) {
        if (line.curve) {
            line.curve->detach();
            delete line.curve;
            line.curve = nullptr;
        }
    }

    // 创建新曲线
    for (int i = 0; i < m_settings->m_lines.size(); ++i) {
        MTFLine& line = m_settings->m_lines[i];
        line.curve = new QwtPlotCurve();
        line.curve->setSamples(line.data);
        line.curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        line.curve->setTitle(QString("Line %1%2").arg(line.index)
            .arg(line.label.isEmpty() ? "" : ": " + line.label));
        line.curve->setItemAttribute(QwtPlotItem::Legend, line.m_style.legendVisible);
        line.curve->setVisible(line.m_style.visible);
        line.curve->attach(m_plot);
        m_simple_browser->updateCurveStyle(i);  // 应用样式
    }

    // 图例点击连接
    if (m_legend) {
        disconnect(m_legend, SIGNAL(checked(const QVariant&, bool, int)),
            this, SLOT(onLegendChecked(const QVariant&, bool, int)));
        connect(m_legend, SIGNAL(checked(const QVariant&, bool, int)),
            this, SLOT(onLegendChecked(const QVariant&, bool, int)));
    }

    autoScaleAxes();
    m_simple_browser->updateLegendItemsStyle();
    m_plot->replot();
    //syncBrowserFromPlot();
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
        for (int i = 0; i < m_settings->m_lines.size(); ++i) {
            if (m_settings->m_lines[i].curve == curve) {
                updateCurveSelection(i, on);
                break;
            }
        }

        m_plot->replot();
    }
}


void MTFViewer::onPlotClicked(const QPointF& pos)
{
    double minDist = std::numeric_limits<double>::max();
    QwtPlotCurve* nearest = nullptr;
    int nearestIdx = -1;

    for (int i = 0; i < m_settings->m_lines.size(); ++i) {
        if (!m_settings->m_lines[i].m_style.visible || !m_settings->m_lines[i].curve) continue;
        QwtPlotCurve* curve = m_settings->m_lines[i].curve;
        for (int j = 0; j < curve->dataSize(); ++j) {
            QPointF p = curve->sample(j);
            double dist = hypot(p.x() - pos.x(), p.y() - pos.y());
            if (dist < minDist) {
                minDist = dist;
                nearest = curve;
                nearestIdx = i;
            }
        }
    }

    if (nearest && minDist < 0.5) 
    {
        bool newSel = !m_settings->m_lines[nearestIdx].selected;
        clearAllSelections();
        updateCurveSelection(nearestIdx, newSel);
    }
    else {
        m_simple_browser->setCurrentCurve(0);
    }
}

void MTFViewer::updateCurveSelection(int idx, bool selected)
{
    if (idx < 0 || idx >= m_settings->m_lines.size()) return;
    m_settings->m_lines[idx].selected = selected;
    m_currentSelectedIndex = selected ? idx : -1;
    if (m_settings->m_lines[idx].curve) {
        QPen pen = m_settings->m_lines[idx].curve->pen();
        pen.setWidth(selected ? 4 : m_settings->m_lines[idx].m_style.lineWidth);
        m_settings->m_lines[idx].curve->setPen(pen);
        updateLegendSelection(m_settings->m_lines[idx].curve, selected);
    }
    m_simple_browser->setCurrentCurve(idx);
    m_plot->replot();
}

void MTFViewer::clearAllSelections()
{
    for (int i = 0; i < m_settings->m_lines.size(); ++i) {
        if (m_settings->m_lines[i].selected) updateCurveSelection(i, false);
    }
    m_currentSelectedIndex = -1;
}

void MTFViewer::updateLegendSelection(QwtPlotCurve* curve, bool selected)
{
    if (!m_legend || !curve) return;
    QList<QwtLegendLabel*> labels = m_legend->findChildren<QwtLegendLabel*>();
    for (QwtLegendLabel* label : labels) {
        if (label->text() == curve->title().text()) {
            QFont f = label->font();
            f.setBold(selected);
            label->setFont(f);
        }
    }
}

void MTFViewer::setXRange(double min, double max)
{
    m_settings->xAxis.min = min;
    m_settings->xAxis.max = max;
    m_settings->xAxis.autoRange = false;
    m_plot->setAxisScale(QwtPlot::xBottom, min, max);
    m_plot->replot();
}

void MTFViewer::setYRange(double min, double max)
{
    m_settings->yAxis.min = min;
    m_settings->yAxis.max = max;
    m_settings->yAxis.autoRange = false;
    m_plot->setAxisScale(QwtPlot::yLeft, min, max);
    m_plot->replot();
}



void MTFViewer::autoScaleAxes()
{
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    bool hasData = false;
    for (const MTFLine& line : m_settings->m_lines) {
        if (!line.m_style.visible || line.data.isEmpty()) continue;
        hasData = true;
        for (const QPointF& p : line.data) {
            if (p.x() < minX) minX = p.x();
            if (p.x() > maxX) maxX = p.x();
            if (p.y() < minY) minY = p.y();
            if (p.y() > maxY) maxY = p.y();
        }
    }
    if (hasData) {
        double xMargin = (maxX - minX) * 0.05;
        double yMargin = (maxY - minY) * 0.05;
        if (xMargin <= 0) xMargin = 1.0;
        if (yMargin <= 0) yMargin = 0.1;

        m_plot->setAxisScale(QwtPlot::xBottom, minX - xMargin, maxX + xMargin);
        m_plot->setAxisScale(QwtPlot::yLeft, std::max(0.0, minY - yMargin), maxY + yMargin);
        QwtScaleDiv temp = m_plot->axisScaleDiv(QwtPlot::xBottom);
        QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
        m_settings->xAxis.min = minX - xMargin;
        m_settings->xAxis.max = maxX + xMargin;

        m_settings->xAxis.step = ticks[1]-ticks[0];


        m_settings->yAxis.min = minY - yMargin;
        m_settings->yAxis.max = maxY + yMargin;
        temp = m_plot->axisScaleDiv(QwtPlot::yLeft);
        ticks = temp.ticks(QwtScaleDiv::MajorTick);
        m_settings->yAxis.step = ticks[1] - ticks[0];

    }
    else {
        m_settings->xAxis.min = m_defaultXMin;
        m_settings->xAxis.max = m_defaultXMax;

        m_settings->yAxis.min = m_defaultYMin;
        m_settings->yAxis.max = m_defaultYMax;
    }
    m_simple_browser->applyXAxisSettings();
    m_simple_browser->applyYAxisSettings();
}

void MTFViewer::setTitle(const QString& title)
{
    m_settings->title = title;
    m_simple_browser->applyTitleSettings();
}

void MTFViewer::setLegendPosition(bool leftSide)
{
    m_settings->legend.position = leftSide ? QwtPlot::LeftLegend : QwtPlot::RightLegend;
    m_simple_browser->applyLegendSettings();
}

void MTFViewer::showLegend(bool show)
{
    m_settings->legend.visible = show;
    m_simple_browser->applyLegendSettings();
}

QColor MTFViewer::getColorFromName(const QString& colorName)
{
    if (colorName == "RGBRed") return QColor(255, 0, 0);
    if (colorName == "RGBGreen") return QColor(0, 255, 0);
    if (colorName == "RGBBlue") return QColor(0, 0, 255);
    if (colorName == "RGBOrange") return QColor(255, 165, 0);
    if (colorName == "RGBYellow") return QColor(255, 255, 0);
    if (colorName == "RGBCyan") return QColor(0, 255, 255);
    return QColor();
}


bool MTFViewer::eventFilter(QObject* object, QEvent* event)
{
    if (object && object == m_plot) // 判断事件属主对象
    {
        switch (event->type())
        {
        case QEvent::Wheel:
        {
            m_settings->xAxis.min = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            m_settings->xAxis.max = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();

            m_settings->yAxis.min = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            m_settings->yAxis.max = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

            m_simple_browser->applyXAxisSettings();
            m_simple_browser->applyYAxisSettings();
            break;
        }
        default:;
        }
    }
    return QObject::eventFilter(object, event);
}

void MTFViewer::zoomIn()
{
    m_plot->setAxisScale(QwtPlot::xBottom,
        m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound() * 0.9,
        m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound() * 0.9);
    m_plot->setAxisScale(QwtPlot::yLeft,
        m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound() * 0.9,
        m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound() * 0.9);
    m_plot->replot();

    m_settings->xAxis.min = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    m_settings->xAxis.max = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();

    m_settings->yAxis.min = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    m_settings->yAxis.max = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    m_simple_browser->applyXAxisSettings();
    m_simple_browser->applyYAxisSettings();
}

void MTFViewer::zoomOut()
{
    m_plot->setAxisScale(QwtPlot::xBottom,
        m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound() * 1.1,
        m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound() * 1.1);
    m_plot->setAxisScale(QwtPlot::yLeft,
        m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound() * 1.1,
        m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound() * 1.1);
    m_plot->replot();

    m_settings->xAxis.min = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    m_settings->xAxis.max = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();

    m_settings->yAxis.min = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    m_settings->yAxis.max = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    m_simple_browser->applyXAxisSettings();
    m_simple_browser->applyYAxisSettings();
}

void MTFViewer::fitView()
{
    autoScaleAxes();
    m_plot->replot();
}

void MTFViewer::showContextMenu(const QPoint& pos)
{
    QMenu menu(this);
    menu.addAction(tr("重置视图"), this, &MTFViewer::fitView);
    menu.addAction(tr("图例在左侧"), [this]() { setLegendPosition(true); });
    menu.addAction(tr("图例在右侧"), [this]() { setLegendPosition(false); });
    menu.addAction(m_legend->isVisible() ? tr("隐藏图例") : tr("显示图例"),
        [this]() { showLegend(!m_legend->isVisible()); });
    menu.addAction(tr("清除选择"), this, &MTFViewer::clearAllSelections);
    menu.addSeparator();
    menu.addAction(tr("保存SVG..."), [this]() {
        QString fn = QFileDialog::getSaveFileName(this, tr("保存SVG"), "", tr("SVG (*.svg)"));
        if (!fn.isEmpty()) m_toolBar_plot->saveAsSVG(fn);
        });
    menu.exec(m_plot->canvas()->mapToGlobal(pos));
}
