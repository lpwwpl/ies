#include "ThirdAberrationPlot.h"
#include <qwt_legend.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <qwt_text.h>
#include <QPen>
#include <QSplitter>
#include <QVBoxLayout>

ThirdAberrationPlot::ThirdAberrationPlot(QWidget* parent)
    : QWidget(parent), m_multiBarChart(nullptr)
{
    setupPlot();
}

ThirdAberrationPlot::~ThirdAberrationPlot()
{
}

void ThirdAberrationPlot::setupPlot()
{

    m_toolBar_plot = new PlotBase(this);
    m_plot = m_toolBar_plot->m_plot;
    m_grid = m_toolBar_plot->m_grid;

    m_settings = m_toolBar_plot->m_settings;
    connect(m_toolBar_plot, SIGNAL(signalFitView()), this, SLOT(fitView()));
    connect(m_toolBar_plot, SIGNAL(signalZoomIn()), this, SLOT(zoomIn()));
    connect(m_toolBar_plot, SIGNAL(signalZoomOut()), this, SLOT(zoomOut()));
    connect(m_toolBar_plot, SIGNAL(signalDisplayProperties(int)), this, SLOT(slotDisplayProperties(int)));
    m_simple_browser = new QwtPropertyBrowser(m_settings, m_plot, m_grid, m_toolBar_plot->m_legend, this);
    connect(m_simple_browser, SIGNAL(signalUpdateScaleDiv()), this, SLOT(updateXY()));
    //signalXScaleAxes
    bool ret = connect(m_simple_browser, SIGNAL(signalXScaleAxes()), this, SLOT(updateXScaleAxes()));
    connect(m_simple_browser, SIGNAL(signalYScaleAxes()), this, SLOT(updateYScaleAxes()));
    // 设置画布背景
    m_plot->setCanvasBackground(Qt::white);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);

    QwtText title("三阶像差系数分组柱状图 (含SUM)");
    m_plot->setTitle("三阶像差系数分组柱状图 (含SUM)");
    m_plot->setCanvasBackground(Qt::white);
    m_plot->setAxisTitle(QwtPlot::xBottom, "表面编号");
    m_plot->setAxisTitle(QwtPlot::yLeft, "像差值");
    // 设置图表标题
    m_settings->title = title.text();

    setAutoFillBackground(true);
    setMinimumSize(900, 600);

    // 添加网格
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    grid->enableX(true);
    grid->enableY(true);
    grid->attach(m_plot);

    m_aberrationNames = QStringList{
        "球差", "子午彗差", "子午像散", "弧矢像散",
        "匹兹伐模糊", "畸变", "倍率色差(垂轴色差)",
        "轴向色差", "匹兹伐场曲"
    };

    m_plot->enableAxis(QwtPlot::xBottom, false);
    m_plot->enableAxis(QwtPlot::yLeft, false);


    // 添加网格
    m_grid->enableX(true);
    m_grid->enableY(true);
    m_grid->enableXMin(true);
    m_grid->enableYMin(true);

    // 设置主网格线样式
    QPen majorGridPen(QColor(200, 200, 200), 1, Qt::SolidLine);
    m_grid->setMajorPen(majorGridPen);

    // 设置次网格线样式
    QPen minorGridPen(QColor(220, 220, 220), 1, Qt::DotLine);
    m_grid->setMinorPen(minorGridPen);

    m_grid->attach(m_plot);

    // 设置坐标轴样式
    QPen axisPen(Qt::black, 2);
    //m_plot->setAxisPen(QwtPlot::xBottom, axisPen);
    //m_plot->setAxisPen(QwtPlot::yLeft, axisPen);

    // 设置刻度样式
    QPen tickPen(Qt::black, 1);
    //m_plot->setAxisPen(QwtPlot::xBottom, tickPen, true);
    //m_plot->setAxisPen(QwtPlot::yLeft, tickPen, true);


    // 分割器
    m_splitter = new QSplitter(this);
    m_splitter->addWidget(m_toolBar_plot);
    m_splitter->addWidget(m_simple_browser);

    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 0);
    m_simple_browser->setMinimumWidth(300);
    m_simple_browser->setMaximumWidth(400);
    m_splitter->setCollapsible(1, false);
    m_splitter->setSizes({ 900, 300 });


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_splitter);

    m_simple_browser->applyTitleSettings();

    setupInteractions();
}

bool ThirdAberrationPlot::loadFile(const QString& fileName)
{
    if (!parseFile(fileName)) return false;

    //plot axis
    createBarChart();
    //settings
    saveInitialView();

    //??
    updateAxesSettings();

    return true;
}

void ThirdAberrationPlot::clearPlot()
{
    if (m_multiBarChart) {
        m_multiBarChart->detach();
        delete m_multiBarChart;
        m_multiBarChart = nullptr;
    }
    m_seriesData.clear();
    m_surfaceNumbers.clear();
    m_plot->enableAxis(QwtPlot::xBottom, false);
    m_plot->enableAxis(QwtPlot::yLeft, false);
    m_plot->replot();
}

bool ThirdAberrationPlot::parseFile(const QString& fileName)
{
    std::ifstream file(fileName.toStdString());
    if (!file.is_open()) return false;

    std::string line;
    std::vector<int> surfIdxList;
    std::vector<std::vector<double>> surfValsList;
    std::vector<double> sumValues;  // 存储SUM行的9个数值
    int idx=0;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        // 检测SUM行（文件中为大写"SUM"）
        if (line.find("SUM") != std::string::npos) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // 跳过 "SUM" 或前面的空格
            double val;
            sumValues.clear();
            for (int i = 0; i < 9; ++i) {
                if (iss >> val) sumValues.push_back(val);
                else break;
            }
            surfIdxList.push_back(idx+1);
            surfValsList.push_back(sumValues);
            continue;
        }

        // 解析普通表面数据（以数字开头的行）
        std::istringstream iss(line);
        double val;
        std::vector<double> vals;
        if (iss >> idx) {
            for (int i = 0; i < 9; ++i) {
                if (iss >> val) vals.push_back(val);
                else break;
            }
            if (vals.size() == 9) {
                surfIdxList.push_back(idx);
                surfValsList.push_back(vals);
            }
        }
    }
    file.close();

    if (surfIdxList.empty() && sumValues.empty()) return false;

    // 填充表面数据
    m_surfaceNumbers = surfIdxList;
    int nSurf = m_surfaceNumbers.size();
    m_seriesData.assign(nSurf, std::vector<double>(9, 0.0));
    for (int s = 0; s < nSurf; ++s) {
        for (int a = 0; a < 9; ++a) {
            m_seriesData[s][a] = surfValsList[s][a];
        }
    }

    return true;
}
void ThirdAberrationPlot::updateX()
{
    m_plot->setAxisScale(QwtPlot::xBottom, m_settings->xAxis.min, m_settings->xAxis.max, m_settings->xAxis.step);
    m_plot->replot();
}
void ThirdAberrationPlot::updateY()
{
    m_plot->setAxisScale(QwtPlot::yLeft, m_settings->yAxis.min, m_settings->yAxis.max, m_settings->yAxis.step);
    m_plot->replot();
}
void ThirdAberrationPlot::createBarChart()
{
    if (m_seriesData.empty()) return;

    // 删除旧的图表项（如果存在）
    if (m_multiBarChart) {
        m_multiBarChart->detach();
        delete m_multiBarChart;
        m_multiBarChart = nullptr;
    }

    // 1. 创建 QwtPlotMultiBarChart
    m_multiBarChart = new QwtPlotMultiBarChart();
    m_multiBarChart->setLayoutPolicy(QwtPlotMultiBarChart::AutoAdjustSamples);
    m_multiBarChart->setSpacing(0);          // 组内柱子间距，越小柱子越宽
    m_multiBarChart->setMargin(0);
    m_multiBarChart->setBaseline(0.0);
    //m_multiBarChart->setLayoutHint(1.2);

    // 2. 设置图例标题
    QList<QwtText> titles;
    for (const QString& name : m_aberrationNames) {
        titles.append(QwtText(name));
    }
    m_multiBarChart->setBarTitles(titles);
    m_multiBarChart->setLegendIconSize(QSize(10, 14));

    // 3. 为9个像差设置颜色
    QList<QColor> colors = {
        QColor(255, 127, 127), QColor(0, 192, 0), QColor(127, 127,255), QColor(255, 127, 255),
        QColor(255, 197, 127), QColor(0, 0, 192), QColor(255, 59, 60),
        QColor(1, 149, 1), QColor(208, 1, 208)
    };
    for (int i = 0; i < 9; ++i) {
        QwtColumnSymbol* symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
        symbol->setLineWidth(1);
        symbol->setFrameStyle(QwtColumnSymbol::Plain);
        symbol->setPalette(QPalette(colors[i]));
        m_multiBarChart->setSymbol(i, symbol);
    }

    // 4. 设置数据
    QVector<QVector<double>> qwtSeriesData;
    QVector<double> surface_0;
    surface_0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
    qwtSeriesData.append(surface_0);
    for (const auto& surface : m_seriesData) {
        qwtSeriesData.append(QVector<double>::fromStdVector(surface));
    }
    m_multiBarChart->setSamples(qwtSeriesData);

    // 5. 附加到绘图区
    m_multiBarChart->attach(m_plot);

    // 设置 X 轴范围，使每个表面编号位于柱子组的中心，并左右留白
    if (!m_surfaceNumbers.empty()) {
        double extraMargin = 0.4;   // 左右额外留白
        double xMin = m_surfaceNumbers.front() - 0.6 - extraMargin;
        double xMax = m_surfaceNumbers.back() + 0.6 + extraMargin;
        m_plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    }


    // 计算 Y 轴范围
    double globalMin = 0.0, globalMax = 0.0;
    for (const auto& surf : m_seriesData) {
        for (double v : surf) {
            if (v < globalMin) globalMin = v;
            if (v > globalMax) globalMax = v;
        }
    }
    if (globalMin > 0) globalMin = 0;
    double margin = (globalMax - globalMin) * 0.1;
    if (margin < 0.01) margin = 0.1;
    m_plot->setAxisScale(QwtPlot::yLeft, globalMin - margin, globalMax + margin);

    // 启用坐标轴并重绘
    m_plot->enableAxis(QwtPlot::xBottom, true);
    m_plot->enableAxis(QwtPlot::yLeft, true);
    m_plot->replot();
}


void ThirdAberrationPlot::setupInteractions()
{
    m_panner = new MyPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    m_magnifier = new MyPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);

    connect(m_panner, &MyPlotPanner::panFinished, this, &ThirdAberrationPlot::updateAxesSettings);
    connect(m_magnifier, &MyPlotMagnifier::zoomed, this, &ThirdAberrationPlot::updateAxesSettings);
}

void ThirdAberrationPlot::zoomOut()
{
    m_initialXMin = m_initialXMin * 1.1;
    m_initialXMax = m_initialXMax * 1.1;
    m_initialYMax = m_initialYMax * 1.1;
    m_initialYMin = m_initialYMin * 1.1;
    // 恢复初始视图范围
    m_plot->setAxisScale(QwtPlot::xBottom, m_initialXMin, m_initialXMax);
    m_plot->setAxisScale(QwtPlot::yLeft, m_initialYMin, m_initialYMax);
    m_plot->setAxisScale(QwtPlot::yRight, m_initialYMin, m_initialYMax);

    m_current_factor = m_current_factor * 1.1;

    // 重新绘制
    m_plot->replot();

    updateAxesSettings();
}

void ThirdAberrationPlot::fitView()
{
    autoScaleAxes();
    m_plot->replot();
}

void ThirdAberrationPlot::zoomIn()
{
    m_initialXMin = m_initialXMin * 0.9;
    m_initialXMax = m_initialXMax * 0.9;
    m_initialYMax = m_initialYMax * 0.9;
    m_initialYMin = m_initialYMin * 0.9;
    // 恢复初始视图范围
    m_plot->setAxisScale(QwtPlot::xBottom, m_initialXMin, m_initialXMax);
    m_plot->setAxisScale(QwtPlot::yLeft, m_initialYMin, m_initialYMax);
    m_plot->setAxisScale(QwtPlot::yRight, m_initialYMin, m_initialYMax);
    // 更新点列位置
    m_current_factor = m_current_factor * 0.9;
    // 重新绘制
    m_plot->replot();

    updateAxesSettings();
}

void ThirdAberrationPlot::saveInitialView()
{
    // 保存初始视图范围
    QwtScaleDiv xScaleDiv = m_plot->axisScaleDiv(QwtPlot::xBottom);
    QwtScaleDiv yScaleDiv = m_plot->axisScaleDiv(QwtPlot::yLeft);

    m_initialXMin = xScaleDiv.lowerBound();
    m_initialXMax = xScaleDiv.upperBound();
    m_initialYMin = yScaleDiv.lowerBound();
    m_initialYMax = yScaleDiv.upperBound();

    m_initialXMin_orig = xScaleDiv.lowerBound();
    m_initialXMax_orig = xScaleDiv.upperBound();
    m_initialYMin_orig = yScaleDiv.lowerBound();
    m_initialYMax_orig = yScaleDiv.upperBound();

    m_current_factor = 1;
}

void ThirdAberrationPlot::slotDisplayProperties(int value)
{
    m_simple_browser->setVisible(value);
}

void ThirdAberrationPlot::updateAxesSettings()
{
    m_settings->xAxis.min = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    m_settings->xAxis.max = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
    m_settings->yAxis.min = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    m_settings->yAxis.max = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
    QwtScaleDiv temp = m_plot->axisScaleDiv(QwtPlot::xBottom);
    QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
    m_settings->xAxis.step = ticks[1] - ticks[0];
    temp = m_plot->axisScaleDiv(QwtPlot::yLeft);
    ticks = temp.ticks(QwtScaleDiv::MajorTick);
    m_settings->yAxis.step = ticks[1] - ticks[0];

    m_simple_browser->blockSignals(true);
    m_simple_browser->applyXAxisSettings();
    m_simple_browser->applyYAxisSettings();
    m_simple_browser->blockSignals(false);

    m_initialXMin = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    m_initialXMax = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();

    m_initialYMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    m_initialYMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
}
void ThirdAberrationPlot::autoScaleAxes()
{
    m_initialXMin = m_initialXMin_orig;
    m_initialXMax = m_initialXMax_orig;
    m_initialYMin = m_initialYMin_orig;
    m_initialYMax = m_initialYMax_orig;

    m_plot->setAxisScale(QwtPlot::xBottom, m_initialXMin, m_initialXMax);
    m_plot->setAxisScale(QwtPlot::yLeft, m_initialYMin, m_initialYMax);
    m_plot->setAxisScale(QwtPlot::yRight, m_initialYMin, m_initialYMax);

    m_current_factor = 1;

    // 重新绘制
    m_plot->replot();


    updateAxesSettings();
}

void ThirdAberrationPlot::updateXScaleAxes()
{
    m_initialXMin = m_initialXMin_orig * m_current_factor;
    m_initialXMax = m_initialXMax_orig * m_current_factor;
    // 恢复初始视图范围
    m_plot->setAxisScale(QwtPlot::xBottom, m_initialXMin, m_initialXMax);

    // 重新绘制
    m_plot->replot();
    updateAxesSettings();
}
void ThirdAberrationPlot::updateYScaleAxes()
{
    m_initialYMin = m_initialYMin_orig * m_current_factor;
    m_initialYMax = m_initialYMax_orig * m_current_factor;

    m_plot->setAxisScale(QwtPlot::yLeft, m_initialYMin, m_initialYMax);
    m_plot->setAxisScale(QwtPlot::yRight, m_initialYMin, m_initialYMax);

    // 重新绘制
    m_plot->replot();
    updateAxesSettings();
}