#include "DeformationWidget.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <QSplitter>
#include <QComboBox>
#include <QCheckBox>
#include <qwt_scale_engine.h>
// ==================== DataLoader 实现 ====================
std::vector<NodeData> DataLoader::load(const std::string& filename) {
    std::vector<NodeData> nodes;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        return nodes;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        NodeData nd;
        if (!(iss >> nd.x >> nd.y >> nd.z >> nd.ux >> nd.uy >> nd.uz)) {
            std::cerr << "Warning: skip malformed line: " << line << std::endl;
            continue;
        }
        nodes.push_back(nd);
    }
    std::cout << "Loaded " << nodes.size() << " raw nodes." << std::endl;
    return nodes;
}

// ==================== Interpolator 实现 ====================
Interpolator::Interpolator(const std::vector<NodeData>& nodes)
    : m_nodes(nodes), m_adaptor(nodes) {
    m_tree = new KDTree(2, m_adaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    m_tree->buildIndex();
}

Interpolator::~Interpolator() {
    delete m_tree;
}

double Interpolator::interpolate(double x, double y, int fieldType, int k, double power) const {
    double query_pt[2] = { x, y };
    std::vector<uint32_t> indices(k);
    std::vector<double> dists_sq(k);
    size_t num_found = m_tree->knnSearch(&query_pt[0], k, &indices[0], &dists_sq[0]);

    double sum_w = 0.0, sum_wv = 0.0;
    for (size_t i = 0; i < num_found; ++i) {
        double dist = std::sqrt(dists_sq[i]);
        if (dist < 1e-12) {
            const auto& nd = m_nodes[indices[i]];
            if (fieldType == 1) return nd.ux;
            if (fieldType == 2) return nd.uy;
            if (fieldType == 3) return nd.uz;
            return std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        }
        double w = 1.0 / std::pow(dist, power);
        sum_w += w;
        const auto& nd = m_nodes[indices[i]];
        double value;
        if (fieldType == 1) value = nd.ux;
        else if (fieldType == 2) value = nd.uy;
        else if (fieldType == 3) value = nd.uz;
        else value = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        sum_wv += w * value;
    }
    return (sum_w > 0) ? (sum_wv / sum_w) : 0.0;
}

// ==================== CustomRasterData 实现 ====================
CustomRasterData::CustomRasterData(const Interpolator& interp,
    double xmin, double xmax,
    double ymin, double ymax,
    int fieldType,
    double zmin, double zmax,
    bool useFixedZRange,
    double centerX, double centerY, double radius)
    : m_interp(interp),
    m_xmin(xmin), m_xmax(xmax), m_ymin(ymin), m_ymax(ymax),
    m_fieldType(fieldType),
    m_useFixedZRange(useFixedZRange),
    m_zmin(zmin), m_zmax(zmax),
    m_zRangeValid(false),
    m_centerX(centerX), m_centerY(centerY), m_radius(radius) {}

// copy() 已删除，可能导致编译错误

QwtInterval CustomRasterData::interval(Qt::Axis axis) const {
    switch (axis) {
    case Qt::XAxis:
        return QwtInterval(m_xmin, m_xmax);
    case Qt::YAxis:
        return QwtInterval(m_ymin, m_ymax);
    case Qt::ZAxis:
        if (m_useFixedZRange) {
            return QwtInterval(m_zmin, m_zmax);
        }
        if (!m_zRangeValid) {
            double minVal = 1e9, maxVal = -1e9;
            const int samplePoints = 100;
            double dx = (m_xmax - m_xmin) / samplePoints;
            double dy = (m_ymax - m_ymin) / samplePoints;
            for (int i = 0; i <= samplePoints; ++i) {
                double x = m_xmin + i * dx;
                for (int j = 0; j <= samplePoints; ++j) {
                    double y = m_ymin + j * dy;
                    double val = value(x, y);
                    if (!std::isnan(val)) {
                        if (val < minVal) minVal = val;
                        if (val > maxVal) maxVal = val;
                    }
                }
            }
            if (minVal == maxVal) {
                minVal -= 1.0;
                maxVal += 1.0;
            }
            m_zmin = minVal;
            m_zmax = maxVal;
            m_zRangeValid = true;
        }
        return QwtInterval(m_zmin, m_zmax);
    default:
        return QwtInterval();
    }
}

double CustomRasterData::value(double x, double y) const {
    if (m_radius > 0) {
        double dx = x - m_centerX;
        double dy = y - m_centerY;
        if (dx * dx + dy * dy > m_radius * m_radius)
            return std::numeric_limits<double>::quiet_NaN();
    }
    x = std::clamp(x, m_xmin, m_xmax);
    y = std::clamp(y, m_ymin, m_ymax);
    return m_interp.interpolate(x, y, m_fieldType);
}

void CustomRasterData::setFieldType(int type) {
    if (m_fieldType != type) {
        m_fieldType = type;
        m_zRangeValid = false;
    }
}

// ==================== MultiDeformationViewer 实现 ====================
MultiDeformationViewer::MultiDeformationViewer(const QString& dataFile, QWidget* parent)
    : QWidget(parent), m_interp(nullptr), m_sharedColorBar(nullptr),
    m_circleCenterX(0.0), m_circleCenterY(0.0), m_circleRadius(10.2)
{
    loadData(dataFile);
    if (m_nodes.empty()) {
        QMessageBox::critical(this, "Error", "Failed to load data file.");
        return;
    }
    computeGlobalZRange();
    setupUI();
}

MultiDeformationViewer::~MultiDeformationViewer() {
    delete m_interp;
}

void MultiDeformationViewer::loadData(const QString& dataFile) {
    std::vector<NodeData> rawNodes = DataLoader::load(dataFile.toStdString());
    if (rawNodes.empty()) return;

    double origXmin = rawNodes[0].x, origXmax = rawNodes[0].x;
    double origYmin = rawNodes[0].y, origYmax = rawNodes[0].y;
    for (const auto& nd : rawNodes) {
        origXmin = std::min(origXmin, nd.x);
        origXmax = std::max(origXmax, nd.x);
        origYmin = std::min(origYmin, nd.y);
        origYmax = std::max(origYmax, nd.y);
    }

    double xRange = origXmax - origXmin;
    double yRange = origYmax - origYmin;
    if (xRange < 1e-12) xRange = 1.0;
    if (yRange < 1e-12) yRange = 1.0;

    const double TARGET_MIN = -10.2;
    const double TARGET_MAX = 10.2;
    const double TARGET_RANGE = TARGET_MAX - TARGET_MIN;

    m_nodes.clear();
    m_nodes.reserve(rawNodes.size());
    for (auto& nd : rawNodes) {
        NodeData mapped = nd;
        mapped.x = TARGET_MIN + (nd.x - origXmin) / xRange * TARGET_RANGE;
        mapped.y = TARGET_MIN + (nd.y - origYmin) / yRange * TARGET_RANGE;
        m_nodes.push_back(mapped);
    }

    std::cout << "Mapped " << m_nodes.size() << " nodes to [-10.2, 10.2]." << std::endl;
    m_interp = new Interpolator(m_nodes);
}

void MultiDeformationViewer::computeGlobalZRange() {
    m_globalZmin = 1e9;
    m_globalZmax = -1e9;
    for (const auto& nd : m_nodes) {
        double dx = nd.x - m_circleCenterX;
        double dy = nd.y - m_circleCenterY;
        if (dx * dx + dy * dy > m_circleRadius * m_circleRadius)
            continue;
        double mag = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        double vals[4] = { mag, nd.ux, nd.uy, nd.uz };
        for (double v : vals) {
            if (v < m_globalZmin) m_globalZmin = v;
            if (v > m_globalZmax) m_globalZmax = v;
        }
    }
    if (m_globalZmin == m_globalZmax) {
        m_globalZmin -= 1.0;
        m_globalZmax += 1.0;
    }
    std::cout << "Global Z range: [" << m_globalZmin << ", " << m_globalZmax << "]" << std::endl;
}
void MultiDeformationViewer::onApplyToAllToggled(bool checked)
{
    Q_UNUSED(checked);
    // 可选：当勾选时立即将当前设置同步到所有图表（也可以保留，等待下次属性修改）
    if (checked && m_propertyBrowser)
    {
        applyAllSettingsToAllPlots();
    }
}
void MultiDeformationViewer::updateXScaleAxes(QwtPlot* plot, PlotInfo& info)
{
    plot->setAxisScale(QwtPlot::xBottom, info.m_initialXMin, info.m_initialXMax);
    plot->replot();
}
void MultiDeformationViewer::updateYScaleAxes(QwtPlot* plot, PlotInfo& info)
{
    plot->setAxisScale(QwtPlot::yLeft, info.m_initialYMin, info.m_initialYMax);
    // 重新绘制
    plot->replot();
}
void MultiDeformationViewer::syncSettingsToPlot(PlotInfo* curInfo, PlotInfo* info)
{
    QwtPlot* plot = info->plot;
    QwtPlotGrid* grid = info->grid;
    QwtLegend* legend = info->legend;

    *info = *curInfo;

    //target settings
    PlotSettings* settings = info->settings;

    if (!plot || !curInfo)
        return;

    // 背景色
    plot->setCanvasBackground(settings->backgroundColor);

    // 标题
    QwtText title(settings->title);
    title.setFont(settings->titleFont);
    title.setColor(settings->titleColor);
    plot->setTitle(title);

    // X 轴
    if (settings->xAxis.visible)
    {
        plot->enableAxis(QwtPlot::xBottom);
        QwtText xTitle(settings->xAxis.title);
        xTitle.setFont(settings->xAxis.titleFont);
        xTitle.setColor(settings->xAxis.titleColor);
        plot->setAxisTitle(QwtPlot::xBottom, xTitle);
        //plot->setAxisFont(QwtPlot::xBottom, settings->xAxis.titleFont);
        QwtScaleWidget* xScale = plot->axisWidget(QwtPlot::xBottom);
        if (xScale) {
            QPalette pal = xScale->palette();
            pal.setColor(QPalette::Text, settings->xAxis.tickColor);
            xScale->setPalette(pal);
        }
        if (!settings->xAxis.autoRange)
        {
            plot->setAxisScale(QwtPlot::xBottom, settings->xAxis.min, settings->xAxis.max, settings->xAxis.step);
            plot->replot();
        }
        else
        {
            updateXScaleAxes(plot, *info);
        }
    }
    else
    {
        plot->enableAxis(QwtPlot::xBottom, false);
    }

    // Y 轴
    if (settings->yAxis.visible)
    {
        plot->enableAxis(QwtPlot::yLeft);
        QwtText yTitle(settings->yAxis.title);
        yTitle.setFont(settings->yAxis.titleFont);
        yTitle.setColor(settings->yAxis.titleColor);
        plot->setAxisTitle(QwtPlot::yLeft, yTitle);
        //plot->setAxisFont(QwtPlot::yLeft, settings->yAxis.titleFont);
        QwtScaleWidget* yScale = plot->axisWidget(QwtPlot::yLeft);
        if (yScale) {
            QPalette pal = yScale->palette();
            pal.setColor(QPalette::Text, settings->yAxis.tickColor);
            yScale->setPalette(pal);
        }
        if (!settings->yAxis.autoRange)
        {
            plot->setAxisScale(QwtPlot::yLeft, settings->yAxis.min, settings->yAxis.max, settings->yAxis.step);
            plot->replot();
        }
        else
        {
            updateYScaleAxes(plot, *info);
        }
    }
    else
    {
        plot->enableAxis(QwtPlot::yLeft, false);
    }

    // 网格
    if (settings->gridVisible) {
        grid->enableX(true);
        grid->enableY(true);
        grid->setMajorPen(QPen(settings->gridMajorColor, 0, settings->gridMajorStyle));
        grid->setMinorPen(QPen(settings->gridMinorColor, 0, settings->gridMinorStyle));
        grid->attach(plot);
    }
    else {
        grid->detach();
    }

    // 图例
    if (settings->legend.visible) {
        if (!legend) {
            legend = new QwtLegend();
            info->legend = legend;
        }
        plot->insertLegend(legend, settings->legend.position);
        legend->setVisible(true);
        // 更新图例标签样式
        QList<QwtLegendLabel*> labels = legend->findChildren<QwtLegendLabel*>();
        for (QwtLegendLabel* label : labels) {
            label->setFont(settings->legend.font);
            QPalette pal = label->palette();
            pal.setColor(QPalette::Text, settings->legend.color);
            label->setPalette(pal);
        }
    }
    else {
        delete legend;
        plot->insertLegend(nullptr);
        legend = NULL;
        info->legend = legend;
    }

    // 原点位置
    QwtScaleDiv xDiv = plot->axisScaleDiv(QwtPlot::xBottom);
    QwtScaleDiv yDiv = plot->axisScaleDiv(QwtPlot::yLeft);
    switch (settings->origin) {
    case PlotSettings::BottomLeft:
        // 取消反转
        plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
        plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);
        // 确保范围是正序
        if (xDiv.lowerBound() > xDiv.upperBound())
        {
            plot->setAxisScale(QwtPlot::xBottom, xDiv.upperBound(), xDiv.lowerBound());
        }
        else
        {
            plot->setAxisScale(QwtPlot::xBottom, xDiv.lowerBound(), xDiv.upperBound());
        }
        if (yDiv.lowerBound() > yDiv.upperBound())
        {
            plot->setAxisScale(QwtPlot::yLeft, yDiv.upperBound(), yDiv.lowerBound());
        }
        else
        {
            plot->setAxisScale(QwtPlot::yLeft, yDiv.lowerBound(), yDiv.upperBound());
        }
        break;
    case PlotSettings::BottomRight:
        plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, true);
        plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);
        // 设置范围为从小到大的值，但引擎会负责反转
        if (xDiv.lowerBound() > xDiv.upperBound())
        {
            plot->setAxisScale(QwtPlot::xBottom, xDiv.lowerBound(), xDiv.upperBound());
        }
        else
        {
            plot->setAxisScale(QwtPlot::xBottom, xDiv.upperBound(), xDiv.lowerBound());
        }
        if (yDiv.lowerBound() > yDiv.upperBound())
        {
            plot->setAxisScale(QwtPlot::yLeft, yDiv.upperBound(), yDiv.lowerBound());
        }
        else
        {
            plot->setAxisScale(QwtPlot::yLeft, yDiv.lowerBound(), yDiv.upperBound());
        }
        break;
    case PlotSettings::Center:
        plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
        plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);
        // 简化的中心原点：取当前范围的一半作为对称范围
        {
            double xRange = xDiv.upperBound() - xDiv.lowerBound();
            double yRange = yDiv.upperBound() - yDiv.lowerBound();
            plot->setAxisScale(QwtPlot::xBottom, -xRange / 2, xRange / 2);
            plot->setAxisScale(QwtPlot::yLeft, -yRange / 2, yRange / 2);
        }
        break;
    }

    plot->replot();
}

void MultiDeformationViewer::applyAllSettingsToAllPlots()
{
    if (!m_propertyBrowser || m_currentPlotIndex < 0)
        return;
    PlotInfo* cur_info = m_plotInfos[m_currentPlotIndex];
    for (int i = 0; i < m_plotInfos.size(); ++i)
    {
        //重复更新没事
        if (i == m_currentPlotIndex)
            continue;
        PlotInfo* info = m_plotInfos[i];
        // 将当前设置同步到目标图表
        syncSettingsToPlot(cur_info, info);
    }
}
void MultiDeformationViewer::setupUI() {

    // 左侧图表区域
    QWidget* plotContainer = new QWidget(this);
    m_mainLayout = new QGridLayout(plotContainer);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    plotContainer->setLayout(m_mainLayout);

    QWidget* controlWidget = new QWidget(this);
    m_controlLayout = new QVBoxLayout(controlWidget);
    m_controlLayout->setSpacing(5);
    m_controlLayout->setContentsMargins(5, 5, 5, 5);
    controlWidget->setLayout(m_controlLayout);

    m_splitter = new QSplitter(this);
    m_splitter->addWidget(plotContainer);
    m_splitter->addWidget(controlWidget);

    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 0);
    m_splitter->setCollapsible(1, false);
    m_splitter->setSizes({ 900, 300 });

    QHBoxLayout* mainHLayout = new QHBoxLayout(this);
    setLayout(mainHLayout);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->addWidget(m_splitter);

    // 图表选择下拉框
    m_plotCombo = new QComboBox(this);
    m_plotCombo->setEnabled(false);
    connect(m_plotCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MultiDeformationViewer::onPlotSelected);
    m_controlLayout->addWidget(m_plotCombo);

    // “应用到所有”复选框
    m_applyToAllCheckBox = new QCheckBox("应用到所有图表", this);
    m_applyToAllCheckBox->setEnabled(false);
    connect(m_applyToAllCheckBox, &QCheckBox::toggled,
        this, &MultiDeformationViewer::onApplyToAllToggled);
    m_controlLayout->addWidget(m_applyToAllCheckBox);

    // 属性浏览器占位（稍后创建）
    m_propertyBrowser = nullptr;

    // /////////////////////////////////////////////////////////////////////
    setWindowTitle("2D Deformation Maps - Normalized to [-12.2, 12.2]");

    const double AXIS_MIN = -12.2;
    const double AXIS_MAX = 12.2;

    m_plotX = createPlot("X Deformation (mm)", 1, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX,true);
    m_plotY = createPlot("Y Deformation (mm)", 2, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX,false);
    m_plotZ = createPlot("Z Deformation (mm)", 3, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX,true);
    m_plotMag = createPlot("Total Deformation (mm)", 0, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX,false);

    m_mainLayout->addWidget(m_plotX, 0, 0);
    m_mainLayout->addWidget(m_plotY, 0, 1);
    m_mainLayout->addWidget(m_plotZ, 1, 0);
    m_mainLayout->addWidget(m_plotMag, 1, 1);

    // ========== 共享颜色条（分为6段，显示分界点数值） ==========
    m_sharedColorBar = new MyScaleWidget(QwtScaleDraw::LeftScale, this);
    m_sharedColorBar->setColorBarEnabled(true);
    m_sharedColorBar->setColorBarWidth(20);
    m_sharedColorBar->setTitle("Deformation (mm)");


    QwtLinearColorMap* colorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
    colorMap->addColorStop(0.33, QColor(0, 255, 255));
    colorMap->addColorStop(0.67, QColor(255, 255, 0));
    m_sharedColorBar->setColorMap(QwtInterval(m_globalZmin, m_globalZmax), colorMap);

    // 将颜色条刻度分为6段，共7个分界点
    QList<double> majorTicks;
    double step = (m_globalZmax - m_globalZmin) / 5.0;
    for (int i = 0; i <= 5; ++i) {
        majorTicks.append(m_globalZmin + i * step);
    }
    majorTicks.last() = m_globalZmax;

    QwtScaleDiv scaleDiv(m_globalZmin, m_globalZmax,
        QList<double>(),  // minor ticks (none)
        QList<double>(),  // medium ticks (none)
        majorTicks);      // major ticks
    m_sharedColorBar->setScaleDiv(scaleDiv);

    QwtScaleDraw* scaleDraw = m_sharedColorBar->scaleDraw();
    //scaleDraw->setLabelFormat("%.3f");
    scaleDraw->setTickLength(QwtScaleDiv::MajorTick, 6);
    scaleDraw->setTickLength(QwtScaleDiv::MinorTick, 0);
    scaleDraw->setTickLength(QwtScaleDiv::MediumTick, 0);
    scaleDraw->enableComponent(QwtScaleDraw::Backbone, false);

    m_mainLayout->addWidget(m_sharedColorBar, 0, 2, 2, 1);


    if (!m_plotInfos.isEmpty())
    {
        for (int i = 0; i < m_plotInfos.size(); ++i)
        {
            QString title = m_plotInfos[i]->plot->title().text();
            m_plotCombo->addItem(title, i);
        }
        m_plotCombo->setEnabled(true);
        m_applyToAllCheckBox->setEnabled(true);
        m_currentPlotIndex = 0;
        m_plotCombo->setCurrentIndex(0);
        onPlotSelected(0);  // 创建属性浏览器并绑定第一个图表
    }
}

void MultiDeformationViewer::onPropertyChanged()
{
    if (!m_propertyBrowser)
        return;

    if (m_applyToAllCheckBox && m_applyToAllCheckBox->isChecked())
    {
        // 应用到所有图表
        applyAllSettingsToAllPlots();
    }
}
//当前的info->plot,根据当前范围更新
void MultiDeformationViewer::updateX()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    info->settings->xAxis.min = info->plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    info->settings->xAxis.max = info->plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
    QwtScaleDiv temp = info->plot->axisScaleDiv(QwtPlot::xBottom);
    QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
    info->settings->xAxis.step = ticks[1] - ticks[0];

    m_propertyBrowser->applyXAxisSettings();
}
//当前的info->plot,根据当前范围更新
void MultiDeformationViewer::updateY()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;
    info->settings->yAxis.min = info->plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    info->settings->yAxis.max = info->plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
    QwtScaleDiv temp = info->plot->axisScaleDiv(QwtPlot::yLeft);
    QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
    info->settings->yAxis.step = ticks[1] - ticks[0];

    m_propertyBrowser->applyYAxisSettings();
}
void MultiDeformationViewer::updateLegend()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    info->legend = m_propertyBrowser->m_legend;
}

void MultiDeformationViewer::onPlotSelected(int index)
{
    if (index < 0 || index >= m_plotInfos.size())
        return;

    m_currentPlotIndex = index;
    PlotInfo* info = m_plotInfos[index];

    if (!m_propertyBrowser)
    {
        // 创建属性浏览器，绑定当前图表的 settings 和控件
        m_propertyBrowser = new QwtPropertyBrowser(info->settings,
            info->plot,
            info->grid,
            info->legend,
            this);
        connect(m_propertyBrowser, SIGNAL(signalUpdateScaleDiv()), this, SLOT(updateXY()));
        connect(m_propertyBrowser, SIGNAL(signalYScaleAxes()), this, SLOT(updateAutoScaleY()));
        connect(m_propertyBrowser, SIGNAL(signalXScaleAxes()), this, SLOT(updateAutoScaleX()));
        connect(m_propertyBrowser, SIGNAL(signalUpdateLegend()), this, SLOT(updateLegend()));
        connect(m_propertyBrowser, &QwtPropertyBrowser::propertyChanged,
            this, &MultiDeformationViewer::onPropertyChanged);
        m_controlLayout->addWidget(m_propertyBrowser);

        m_propertyBrowser->m_browser->removeProperty(m_propertyBrowser->group_curve);
        m_propertyBrowser->m_browser->removeProperty(m_propertyBrowser->group_item);
    }

    // 更新已有浏览器的内部指针
    m_propertyBrowser->m_settings = info->settings;
    m_propertyBrowser->m_plot = info->plot;
    m_propertyBrowser->m_grid = info->grid;
    m_propertyBrowser->m_legend = info->legend;

    // 刷新界面
    m_propertyBrowser->InitSetupUI();
}

void MultiDeformationViewer::onLegendClicked(const QVariant& itemInfo)
{
    // 获取发送信号的图例
    QwtLegend* legend = qobject_cast<QwtLegend*>(sender());
    if (!legend) return;

    // 获取对应的plot
    QwtPlot* plot = nullptr;
    for (auto* p : m_plots) {
        if (p->legend() == legend) {
            plot = p;
            break;
        }
    }

    if (!plot) return;

    // 找到对应的曲线
    QwtPlotItem* item = plot->infoToItem(itemInfo);
    if (item && item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
        QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);

        // 切换曲线的可见性
        bool visible = !curve->isVisible();
        curve->setVisible(visible);

        // 更新图例显示
        QList<QWidget*> legendWidgets = legend->legendWidgets(itemInfo);
        for (QWidget* w : legendWidgets) {
            if (QwtLegendLabel* label = qobject_cast<QwtLegendLabel*>(w)) {
                label->setChecked(visible);
            }
        }

        // 重新调整坐标轴范围
        if (visible) {
            // 如果显示曲线，重新计算范围
            plot->setAutoReplot(false);
            plot->updateAxes();
            plot->setAutoReplot(true);
        }

        plot->replot();
    }
}

//当前的info->plot,恢复init
void MultiDeformationViewer::updateAutoScaleX()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    updateXScaleAxes(info->plot, *info);

    m_propertyBrowser->applyXAxisSettings();
}
//当前的info->plot,恢复init
void MultiDeformationViewer::updateAutoScaleY()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    updateYScaleAxes(info->plot, *info);

    m_propertyBrowser->applyYAxisSettings();
}
void MultiDeformationViewer::saveInitialView(PlotInfo& info)
{
    // 保存初始视图范围
    info.m_initialXMin = info.settings->xAxis.min;
    info.m_initialXMax = info.settings->xAxis.max;
    info.m_initialYMin = info.settings->yAxis.min;
    info.m_initialYMax = info.settings->yAxis.max;

    info.m_initialXMin_orig = info.m_initialXMin;
    info.m_initialXMax_orig = info.m_initialXMax;
    info.m_initialYMin_orig = info.m_initialYMin;
    info.m_initialYMax_orig = info.m_initialYMax;

    info.m_current_factor = 1;
}
void MultiDeformationViewer::setupPlotInteractions(QwtPlot* plot)
{
    if (!plot) return;

    // 1. 拖拽功能 - 对应 QCP::iRangeDrag
    MyPlotPanner* panner = new MyPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::LeftButton);
    panner->setEnabled(true);

    // 2. 缩放功能 - 对应 QCP::iRangeZoom
    MyPlotMagnifier* magnifier = new MyPlotMagnifier(plot->canvas());
    magnifier->setMouseButton(Qt::NoButton);  // 不使用鼠标按钮，仅用滚轮
    magnifier->setWheelFactor(1.1);  // 滚轮缩放因子
    magnifier->setZoomInKey(Qt::Key_Plus, Qt::ControlModifier);
    magnifier->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);
    magnifier->setEnabled(true);

    connect(panner, &MyPlotPanner::panFinished, this, &MultiDeformationViewer::updateAxesSettings_noparam);
    connect(magnifier, &MyPlotMagnifier::zoomed, this, &MultiDeformationViewer::updateAxesSettings_noparam);

    // 3. 右键菜单用于重置视图（可选功能）
    plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);
}
//根据info和plot更新xy
void MultiDeformationViewer::updateAxesSettings(QwtPlot* plot, PlotInfo& info)
{
    if (!info.settings->xAxis.autoRange)
    {
        info.settings->xAxis.min = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
        info.settings->xAxis.max = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
        QwtScaleDiv temp = plot->axisScaleDiv(QwtPlot::xBottom);
        QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
        info.settings->xAxis.step = ticks[1] - ticks[0];
    }
    else
    {
        plot->setAxisScale(QwtPlot::xBottom, info.m_initialXMin, info.m_initialXMax);
        plot->replot();
    }
    if (!info.settings->yAxis.autoRange)
    {
        info.settings->yAxis.min = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
        info.settings->yAxis.max = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
        QwtScaleDiv temp = plot->axisScaleDiv(QwtPlot::yLeft);
        QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
        info.settings->yAxis.step = ticks[1] - ticks[0];
    }
    else
    {
        plot->setAxisScale(QwtPlot::yLeft, info.m_initialYMin, info.m_initialYMax);
        plot->replot();
    }
}
//pan,zoom对应的函数
void MultiDeformationViewer::updateAxesSettings_noparam()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    updateAxesSettings(info->plot, *info);
    m_propertyBrowser->applyYAxisSettings();
    m_propertyBrowser->applyXAxisSettings();
    if (m_applyToAllCheckBox && m_applyToAllCheckBox->isChecked())
    {
        // 应用到所有图表
        applyAllSettingsToAllPlots();
    }
}
QwtPlot* MultiDeformationViewer::createPlot(const QString& title, int fieldType,
    double xmin, double xmax,
    double ymin, double ymax, bool isYTitle) {
    QwtPlot* plot = new QwtPlot(this);
    plot->setTitle(title);
    plot->setAxisTitle(QwtPlot::xBottom, "X坐标(mm)");
    if (isYTitle)
        plot->setAxisTitle(QwtPlot::yLeft, "Y坐标(mm)");
    else
        plot->setAxisTitle(QwtPlot::yLeft, "");
    plot->setCanvasBackground(Qt::white);

    plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);

    CustomRasterData* rasterData = new CustomRasterData(*m_interp,
        xmin, xmax, ymin, ymax,
        fieldType, m_globalZmin, m_globalZmax, true,
        m_circleCenterX, m_circleCenterY, m_circleRadius);
    QwtPlotSpectrogram* spectrogram = new QwtPlotSpectrogram();
    spectrogram->setData(rasterData);
    QwtLinearColorMap* colorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
    colorMap->addColorStop(0.33, QColor(0, 255, 255));
    colorMap->addColorStop(0.67, QColor(255, 255, 0));
    spectrogram->setColorMap(colorMap);
    spectrogram->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    spectrogram->attach(plot);

    plot->enableAxis(QwtPlot::yRight, false);

    std::vector<double> values;
    std::vector<NodeData> validNodes;
    for (const auto& nd : m_nodes) {
        double dx = nd.x - m_circleCenterX;
        double dy = nd.y - m_circleCenterY;
        if (dx * dx + dy * dy > m_circleRadius * m_circleRadius)
            continue;
        validNodes.push_back(nd);
        double val;
        if (fieldType == 1)      val = nd.ux;
        else if (fieldType == 2) val = nd.uy;
        else if (fieldType == 3) val = nd.uz;
        else                     val = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        values.push_back(val);
    }
    if (!values.empty()) {
        auto maxIt = std::max_element(values.begin(), values.end());
        auto minIt = std::min_element(values.begin(), values.end());
        size_t maxIdx = std::distance(values.begin(), maxIt);
        size_t minIdx = std::distance(values.begin(), minIt);
        double maxVal = *maxIt;
        double minVal = *minIt;

        double xRange = xmax - xmin;
        double yRange = ymax - ymin;
        addMarkers(plot, values, validNodes, xRange, yRange, maxIdx, minIdx, maxVal, minVal);
        addInfoBox(plot, (int)validNodes.size(), minVal, maxVal,
            xmin + 0.02 * xRange, ymax - 0.05 * yRange);
    }


    // 添加网格
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
    grid->attach(plot);
    QwtLegend* legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable); // 使图例项可点击
    plot->insertLegend(legend, QwtPlot::BottomLegend);

    // 连接图例点击信号
    connect(legend, SIGNAL(clicked(const QVariant&, int)),
        this, SLOT(onLegendClicked(const QVariant&)));
    PlotSettings* settings = new PlotSettings();

    plot->replot();

    settings->gridMajorStyle = Qt::DotLine;
    settings->gridMajorColor = Qt::gray;
    settings->gridMinorStyle = Qt::DotLine;
    settings->gridMinorColor = Qt::lightGray;
    settings->gridVisible = true;
    settings->origin = PlotSettings::BottomLeft;
    settings->title = plot->title().text();
    settings->titleFont = plot->title().font();
    settings->titleColor = plot->title().color();
    settings->backgroundColor = plot->canvasBackground().color();
    settings->legend.visible = true;
    settings->legend.position = QwtPlot::BottomLegend;
    settings->xAxis.visible = true;
    settings->xAxis.title = plot->axisTitle(QwtPlot::xBottom).text();
    settings->xAxis.titleFont = plot->axisTitle(QwtPlot::xBottom).font();
    settings->xAxis.autoRange = true;
    settings->xAxis.step = 1;
    settings->yAxis.titleFont = plot->axisTitle(QwtPlot::yLeft).font();
    settings->yAxis.visible = true;
    settings->yAxis.title = plot->axisTitle(QwtPlot::yLeft).text();
    settings->yAxis.autoRange = true;  // 自动范围由数据决定
    settings->yAxis.step = 1;
    //缓存
    settings->xAxis.min = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    settings->xAxis.max = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
    settings->yAxis.min = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    settings->yAxis.max = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();


    PlotInfo* info = new PlotInfo();
    info->plot = plot;
    info->grid = grid;
    info->legend = legend;
    info->settings = settings;

    saveInitialView(*info);
    m_plotInfos.append(info);

    return plot;
}

void MultiDeformationViewer::addMarkers(QwtPlot* plot, const std::vector<double>& values,
    const std::vector<NodeData>& nodes,
    double xRange, double yRange,
    size_t maxIdx, size_t minIdx,
    double maxVal, double minVal) {
    if (values.empty()) return;

    QwtPlotMarker* maxMarker = new QwtPlotMarker();
    maxMarker->setValue(nodes[maxIdx].x, nodes[maxIdx].y);
    maxMarker->setSymbol(new QwtSymbol(QwtSymbol::Triangle, QBrush(Qt::red), QPen(Qt::white, 2), QSize(12, 12)));
    maxMarker->attach(plot);

    QwtPlotMarker* minMarker = new QwtPlotMarker();
    minMarker->setValue(nodes[minIdx].x, nodes[minIdx].y);
    minMarker->setSymbol(new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::blue), QPen(Qt::white, 2), QSize(12, 12)));
    minMarker->attach(plot);

    double offsetX = 0.02 * xRange;
    double offsetY = 0.02 * yRange;
    QwtPlotMarker* maxLabel = new QwtPlotMarker();
    maxLabel->setValue(nodes[maxIdx].x + offsetX, nodes[maxIdx].y + offsetY);
    maxLabel->setLabel(QwtText(QString(" Max: %1").arg(maxVal, 0, 'f', 3)));
    maxLabel->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    maxLabel->attach(plot);

    QwtPlotMarker* minLabel = new QwtPlotMarker();
    minLabel->setValue(nodes[minIdx].x + offsetX, nodes[minIdx].y - offsetY);
    minLabel->setLabel(QwtText(QString(" Min: %1").arg(minVal, 0, 'f', 3)));
    minLabel->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    minLabel->attach(plot);
}

void MultiDeformationViewer::addInfoBox(QwtPlot* plot, int nodeCount,
    double minVal, double maxVal,
    double xPos, double yPos) {
    QString infoText = QString("Nodes: %1\nRange: [%2, %3]")
        .arg(nodeCount)
        .arg(minVal, 0, 'f', 3)
        .arg(maxVal, 0, 'f', 3);
    QwtPlotMarker* infoBox = new QwtPlotMarker();
    infoBox->setValue(xPos, yPos);
    QwtText text(infoText);
    text.setColor(Qt::white);
    text.setBackgroundBrush(QColor(0, 0, 0, 180));
    text.setBorderPen(QPen(Qt::gray, 1));
    text.setFont(QFont("Arial", 9));
    infoBox->setLabel(text);
    infoBox->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoBox->attach(plot);
}