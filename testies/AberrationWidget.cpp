//#include "AberrationWidget.h"
//#include <QFile>
//#include <QTextStream>
//#include <QDebug>
//#include <QApplication>
//
//AberrationWidget::AberrationWidget(QWidget* parent)
//    : QWidget(parent)
//{
//    setupUI();
//}
//
//
//
//void AberrationWidget::setupUI()
//{
//    m_mainLayout = new QGridLayout(this);
//    m_mainLayout->setSpacing(10);
//    m_mainLayout->setContentsMargins(10, 10, 10, 10);
//
//    setLayout(m_mainLayout);
//}
//
//bool AberrationWidget::loadDataFromFile(const QString& filename)
//{
//    QFile file(filename);
//    // 解析数据
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
//        return false;
//    }
//
//
//    dataList.clear();
//
//    QTextStream in(&file);
//    QString line;
//    AberrationData currentData;
//    bool readingData = false;
//    bool hasData = false;
//
//    // 初始化当前数据
//    currentData.lineIndex = -1;
//
//    while (!in.atEnd()) {
//        line = in.readLine().trimmed();
//
//        if (line.startsWith("# CHART NAME:")) {
//            // 如果是新数据块开始，保存前一个数据块
//            if (currentData.lineIndex != -1) {
//                dataList.append(currentData);
//                hasData = true;
//            }
//
//            // 初始化新数据块
//            currentData = AberrationData();
//            currentData.chartName = line.mid(14).trimmed();
//            readingData = false;
//        }
//        else if (line.startsWith("LINE INDEX:")) {
//            currentData.lineIndex = line.mid(12).trimmed().toInt();
//        }
//        else if (line.startsWith("LINE LABEL:")) {
//            currentData.lineLabel = line.mid(12).trimmed();
//        }
//        else if (line.startsWith("LINE COLOR:")) {
//            currentData.lineColor = line.mid(12).trimmed();
//        }
//        else if (line.startsWith("VIEW FIELD INDEX:")) {
//            currentData.viewFieldIndex = line.mid(18).trimmed().toInt();
//        }
//        else if (line.startsWith("WAVE LENGTH INDEX:")) {
//            currentData.waveLengthIndex = line.mid(19).trimmed().toInt();
//        }
//        else if (line.startsWith("WAVE LENGTH:")) {
//            currentData.waveLength = line.mid(13).trimmed().toDouble();
//        }
//        else if (line.startsWith("INDEX")) {
//            // 数据表头，下一行开始是数据
//            readingData = true;
//            // 跳过表头行
//            in.readLine();
//        }
//        else if (readingData && !line.isEmpty()) {
//            // 解析数据行
//            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
//            if (parts.size() >= 3) {
//                currentData.normalizedAperture.append(parts[1].toDouble());
//                currentData.aberration.append(parts[2].toDouble());
//            }
//        }
//    }
//
//    // 添加最后一个数据块
//    if (currentData.lineIndex != -1) {
//        dataList.append(currentData);
//        hasData = true;
//    }
//
//    file.close();
//
//
//
//    QMap<int, QVector<AberrationData>> fieldGroups;
//    for (const auto& data : dataList) {
//        fieldGroups[data.viewFieldIndex].append(data);
//    }
//    int cols = fieldGroups.size();
//    // 为每个视场创建图表
//    
//    int col_idx = 0;
//    for (auto it = fieldGroups.begin(); it != fieldGroups.end(); ++it) {
//        int fieldIndex = it.key();
//        const QVector<AberrationData>& fieldData = it.value();
//
//        QMap<QString, QVector<AberrationData>> chartName_grps = groupByChartName(fieldData);
//
//
//        int row_idx = 0;
// 
//        // 为每个波长数据添加曲线
//        for (auto& key : chartName_grps.keys()) {
//            
//            QCustomPlot* customPlot = new QCustomPlot();
//            customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
//
//
//            QVector<AberrationData> grp = chartName_grps.value(key);
//            for (auto& wave : grp)
//            {
//                QCPGraph* graph = customPlot->addGraph();
//                graph->setData(wave.normalizedAperture, wave.aberration);
//
//                QString legendName = QString("%1nm").arg(wave.waveLength);//λ=
//                graph->setName(legendName);
//                graph->setPen(QPen(getColorFromString(wave.lineColor), 1));
//                graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
//            }
//
//            customPlot->xAxis->setLabel("归一化光阑");
//            customPlot->yAxis->setLabel("光线像差 (mm)");
//            customPlot->xAxis->setRange(-1.1, 1.1);
//
//            // 自动调整Y轴范围
//            customPlot->rescaleAxes();
//
//            // 添加标题
//            QString title = QString("%1").arg(key);
//            customPlot->plotLayout()->insertRow(0);
//            customPlot->plotLayout()->addElement(0, 0,
//                new QCPTextElement(customPlot, title, QFont("sans", 12, QFont::Bold)));
//            customPlot->plotLayout()->insertRow(1);
//            customPlot->plotLayout()->addElement(1, 0, customPlot->legend);
//            //customPlot->plotLayout()->addElement(2, 2, customPlot->axisRect());
//            
//
//            customPlot->legend->setWrap(5); // 每行最多6个图例项
//            customPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
//            //// 添加图例
//            customPlot->legend->setVisible(true);
//            customPlot->legend->setFont(QFont("Helvetica", 8));
//            customPlot->legend->setMinimumSize(0, 15);   // 最小高度30
//            customPlot->legend->setMaximumSize(10000, 15); // 最大高度30
//            //customPlot->legend->setRowStretchFactor(1, 0);
//
//
//            customPlot->replot();
//
//
//            // 添加到标签页
//            QString tabName = QString("视场 %1").arg(fieldIndex);
//            m_mainLayout->addWidget(customPlot, row_idx, col_idx);
//            row_idx++;
//        }
//        col_idx++;
//    }
//
//
//    return true;
//}
//
//
//
//
//QColor AberrationWidget::getColorFromString(const QString& colorStr)
//{
//    if (colorStr == "RGBOrange") return QColor(255, 165, 0);
//    else if (colorStr == "RGBYellow") return QColor(255, 255, 0);
//    else if (colorStr == "RGBGreen") return QColor(0, 255, 0);
//    else if (colorStr == "RGBCyan") return QColor(0, 255, 255);
//    else if (colorStr == "RGBRed") return QColor(255, 0, 0);
//    else if (colorStr == "RGBBlue") return QColor(0, 0, 255);
//    else return QColor(0, 0, 0); // 默认黑色
//}
//
//
//

#include "AberrationWidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QFrame>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>
#include <QComboBox>
#include <QCheckBox>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <QSplitter>
AberrationWidget::AberrationWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_splitter(nullptr)
    , m_controlLayout(nullptr)
    , m_plotCombo(nullptr)
    , m_applyToAllCheckBox(nullptr)
    , m_propertyBrowser(nullptr)
    , m_currentPlotIndex(-1)
{
    setupUI();

}

AberrationWidget::~AberrationWidget()
{
    clearPlots();
    for (auto& info : m_plotInfos) {
        delete info->settings;
    }
    delete m_propertyBrowser;
}

void AberrationWidget::setupUI()
{
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
        this, &AberrationWidget::onPlotSelected);
    m_controlLayout->addWidget(m_plotCombo);

    // “应用到所有”复选框
    m_applyToAllCheckBox = new QCheckBox("应用到所有图表", this);
    m_applyToAllCheckBox->setEnabled(false);
    connect(m_applyToAllCheckBox, &QCheckBox::toggled,
        this, &AberrationWidget::onApplyToAllToggled);
    m_controlLayout->addWidget(m_applyToAllCheckBox);

    // 属性浏览器占位（稍后创建）
    m_propertyBrowser = nullptr;
}
void AberrationWidget::onPropertyChanged()
{
    if (!m_propertyBrowser)
        return;

    if (m_applyToAllCheckBox && m_applyToAllCheckBox->isChecked()) 
    {
        // 应用到所有图表
        applyAllSettingsToAllPlots();
    }
}
void AberrationWidget::syncSettingsToPlot(PlotInfo* curInfo,PlotInfo* info)
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
        plot->setAxisFont(QwtPlot::xBottom, settings->xAxis.tickFont);
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
        plot->setAxisFont(QwtPlot::yLeft, settings->yAxis.tickFont);
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
            updateYScaleAxes(plot,*info);
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

void AberrationWidget::updateXScaleAxes(QwtPlot* plot, PlotInfo& info)
{
    plot->setAxisScale(QwtPlot::xBottom, info.m_initialXMin, info.m_initialXMax);
    plot->replot();
}
void AberrationWidget::saveInitialView(PlotInfo& info)
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
void AberrationWidget::updateYScaleAxes(QwtPlot* plot,PlotInfo& info)
{
    plot->setAxisScale(QwtPlot::yLeft, info.m_initialYMin, info.m_initialYMax);
    // 重新绘制
    plot->replot();
}

void AberrationWidget::updateLegend()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    info->legend = m_propertyBrowser->m_legend;
}
//当前的info->plot,恢复init
void AberrationWidget::updateAutoScaleX()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    updateXScaleAxes(info->plot,*info);

    m_propertyBrowser->applyXAxisSettings();
}
//当前的info->plot,恢复init
void AberrationWidget::updateAutoScaleY()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;

    updateYScaleAxes(info->plot, *info);

    m_propertyBrowser->applyYAxisSettings();
}

//当前的info->plot,根据当前范围更新
void AberrationWidget::updateX()
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
void AberrationWidget::updateY()
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
//pan,zoom对应的函数
void AberrationWidget::updateAxesSettings_noparam()
{
    if (m_currentPlotIndex < 0 || m_currentPlotIndex >= m_plotInfos.size())return;
    PlotInfo* info = m_plotInfos[m_currentPlotIndex];
    if (!info)return;
    
    updateAxesSettings(info->plot,*info);
    m_propertyBrowser->applyYAxisSettings();
    m_propertyBrowser->applyXAxisSettings();
    if (m_applyToAllCheckBox && m_applyToAllCheckBox->isChecked())
    {
        // 应用到所有图表
        applyAllSettingsToAllPlots();
    }
}
//根据info和plot更新xy
void AberrationWidget::updateAxesSettings(QwtPlot* plot, PlotInfo& info)
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
void AberrationWidget::applyAllSettingsToAllPlots()
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

        //根据index设置
        for (int j = 0; j < info->curves.size() && j < cur_info->settings->m_lines.size(); ++j) 
        {
            const MTFLine& srcLine = cur_info->settings->m_lines[j];
            MTFLine& dstLine = info->settings->m_lines[j];
            dstLine.m_style = srcLine.m_style;
            // 应用样式到实际曲线
            QwtPlotCurve* curve = info->curves[j];
            updateCurveStyle(info->plot,dstLine);
        }
    }
}

void AberrationWidget::updateCurveStyle(QwtPlot* plot,MTFLine& line)
{
    if (!line.curve) return;

    //DotLine, DashDotLine,  DashDotDotLine,
    Qt::PenStyle lineStyle = line.m_style.lineStyle;
    if (lineStyle == (Qt::CustomDashLine + 1))
    {
        line.curve->setStyle(QwtPlotCurve::Dots);
    }
    else
    {
        // 线条
        QPen pen = line.curve->pen();
        pen.setWidth(line.m_style.lineWidth);
        pen.setColor(line.m_style.lineColor);
        pen.setStyle(line.m_style.lineStyle);
        if (line.m_style.lineStyle == Qt::CustomDashLine && !line.m_style.customDashPattern.isEmpty()) 
        {
            pen.setDashPattern(line.m_style.customDashPattern);
        }
        line.curve->setPen(pen);
        line.curve->setStyle(QwtPlotCurve::Lines);
    }
    // 曲线/折线
    line.curve->setCurveAttribute(QwtPlotCurve::Fitted, line.m_style.useCurve);

    // 点符号
    if (line.curve->symbol())
    {
        line.curve->setSymbol(nullptr);
    }

    if ((line.m_style.pointStyle != QwtSymbol::NoSymbol) && (lineStyle == Qt::DotLine || (lineStyle == Qt::CustomDashLine + 1) || lineStyle == Qt::DashDotLine || lineStyle == Qt::DashDotDotLine)) {
        QwtSymbol* sym = new QwtSymbol(line.m_style.pointStyle);
        sym->setSize(line.m_style.pointSize);
        sym->setColor(line.m_style.pointColor);
        QPen symPen = sym->pen();
        symPen.setWidth(line.m_style.pointWidth);
        symPen.setColor(line.m_style.pointColor);
        sym->setPen(symPen);
        QBrush brush;
        brush.setStyle(line.m_style.pointFilled ? Qt::SolidPattern : Qt::NoBrush);
        brush.setColor(line.m_style.pointColor);
        sym->setBrush(brush);
        line.curve->setSymbol(sym);
    }
    else 
    {
        line.curve->setSymbol(nullptr);
    }

    if (line.m_style.visible)
    {
        line.curve->setVisible(true);
    }
    else
    {
        line.curve->setVisible(false);
    }
    plot->replot();
}


void AberrationWidget::onPlotSelected(int index)
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
            this, &AberrationWidget::onPropertyChanged);
        m_controlLayout->addWidget(m_propertyBrowser);
    }

    // 更新已有浏览器的内部指针
    m_propertyBrowser->m_settings = info->settings;
    m_propertyBrowser->m_plot = info->plot;
    m_propertyBrowser->m_grid = info->grid;
    m_propertyBrowser->m_legend = info->legend;

    // 刷新界面
    m_propertyBrowser->InitSetupUI();
    m_propertyBrowser->updateLineCombo();
    m_propertyBrowser->updateItemCombo();

}
void AberrationWidget::onApplyToAllToggled(bool checked)
{
    Q_UNUSED(checked);
    // 可选：当勾选时立即将当前设置同步到所有图表（也可以保留，等待下次属性修改）
    if (checked && m_propertyBrowser) 
    {
        applyAllSettingsToAllPlots();
    }
}
void AberrationWidget::clearPlots()
{
    // 清除所有曲线（由各 PlotInfo 管理）
    for (auto& info : m_plotInfos) 
    {
        qDeleteAll(info->curves);
        info->curves.clear();
        delete info->plot;
        delete info->grid;
        // legend 由 plot 管理，不需要手动 delete
    }
    m_plotInfos.clear();

    // 清除布局中的部件
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) 
    {
        delete item->widget();
        delete item;
    }

    // 重置组合框和属性浏览器
    if (m_plotCombo) 
    {
        m_plotCombo->clear();
        m_plotCombo->setEnabled(false);
    }
    if (m_applyToAllCheckBox)
        m_applyToAllCheckBox->setEnabled(false);
    if (m_propertyBrowser) 
    {
        delete m_propertyBrowser;
        m_propertyBrowser = nullptr;
    }
    m_currentPlotIndex = -1;
}

bool AberrationWidget::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    // 解析数据
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
        return false;
    }

    dataList.clear();

    QTextStream in(&file);
    QString line;
    AberrationData currentData;
    bool readingData = false;
    bool hasData = false;

    // 初始化当前数据
    currentData.lineIndex = -1;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("# CHART NAME:")) {
            // 如果是新数据块开始，保存前一个数据块
            if (currentData.lineIndex != -1) {
                dataList.append(currentData);
                hasData = true;
            }

            // 初始化新数据块
            currentData = AberrationData();
            currentData.chartName = line.mid(14).trimmed();
            readingData = false;
        }
        else if (line.startsWith("LINE INDEX:")) {
            currentData.lineIndex = line.mid(12).trimmed().toInt();
        }
        else if (line.startsWith("LINE LABEL:")) {
            currentData.lineLabel = line.mid(12).trimmed();
        }
        else if (line.startsWith("LINE COLOR:")) {
            currentData.lineColor = line.mid(12).trimmed();
        }
        else if (line.startsWith("VIEW FIELD INDEX:")) {
            currentData.viewFieldIndex = line.mid(18).trimmed().toInt();
        }
        else if (line.startsWith("WAVE LENGTH INDEX:")) {
            currentData.waveLengthIndex = line.mid(19).trimmed().toInt();
        }
        else if (line.startsWith("WAVE LENGTH:")) {
            currentData.waveLength = line.mid(13).trimmed().toDouble();
        }
        else if (line.startsWith("INDEX")) {
            // 数据表头，下一行开始是数据
            readingData = true;
            // 跳过表头行
            in.readLine();
        }
        else if (readingData && !line.isEmpty()) {
            // 解析数据行
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            if (parts.size() >= 3) {
                currentData.normalizedAperture.append(parts[1].toDouble());
                currentData.aberration.append(parts[2].toDouble());
            }
        }
    }

    // 添加最后一个数据块
    if (currentData.lineIndex != -1) {
        dataList.append(currentData);
        hasData = true;
    }

    file.close();

    if (!hasData) {
        QMessageBox::warning(this, "错误", "文件中没有有效数据");
        return false;
    }

    // 清除之前的图表
    clearPlots();

    // 按视场分组
    QMap<int, QVector<AberrationData>> fieldGroups;
    for (const auto& data : dataList) {
        fieldGroups[data.viewFieldIndex].append(data);
    }
    int cols = fieldGroups.size();

    // 为每个视场创建图表
    int col_idx = 0;
    for (auto it = fieldGroups.begin(); it != fieldGroups.end(); ++it) 
    {
        int fieldIndex = it.key();
        const QVector<AberrationData>& fieldData = it.value();
        // 按chartName分组
        QMap<QString, QVector<AberrationData>> chartName_grps = groupByChartName(fieldData);

        int row_idx = 0;

        // 为每个chartName创建图表
        for (auto& key : chartName_grps.keys()) 
        {
            // 创建QwtPlot
            QwtPlot* plot = new QwtPlot();
            plot->setCanvasBackground(Qt::white);
            plot->setMinimumSize(400, 300);
            // 设置标题
            plot->setTitle(QString("%1 - 视场 %2").arg(key).arg(fieldIndex));
            // 设置坐标轴标签
            plot->setAxisTitle(QwtPlot::xBottom, "归一化光阑");
            plot->setAxisTitle(QwtPlot::yLeft, "光线像差 (mm)");

            // 设置X轴范围
            plot->setAxisScale(QwtPlot::xBottom, -1.1, 1.1);

            // 添加网格
            QwtPlotGrid* grid = new QwtPlotGrid();
            grid->enableX(true);
            grid->enableY(true);
            grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
            grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
            grid->attach(plot);

            QVector<AberrationData> grp = chartName_grps.value(key);
            double minY = std::numeric_limits<double>::max();
            double maxY = std::numeric_limits<double>::lowest();

            PlotSettings* settings = new PlotSettings();
            // 为每个波长数据添加曲线
            int index = 0;
            for (auto& wave : grp) 
            {
                QwtPlotCurve* curve = new QwtPlotCurve();

                // 设置曲线名称（图例中显示）
                QString legendName = QString("%1nm").arg(wave.waveLength);
                curve->setTitle(legendName);
                // 设置曲线数据
                curve->setSamples(wave.normalizedAperture, wave.aberration);

                // 设置曲线颜色
                QColor curveColor = getColorFromString(wave.lineColor);
                curve->setPen(QPen(curveColor, 2));
                curve->setRenderHint(QwtPlotItem::RenderAntialiased);
                // 设置符号（圆圈）
                QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse);
                symbol->setSize(5);
                symbol->setColor(curveColor);
                symbol->setPen(QPen(curveColor, 1));
                curve->setSymbol(symbol);
                // 设置曲线属性
                //curve->setItemAttribute(QwtPlotItem::Legend, true);
                //curve->setItemAttribute(QwtPlotItem::AutoScale, true);
                // 附加到图表
                curve->attach(plot);
                // 记录曲线对象
                m_curves.append(curve);
               
                ////////////////同步curve到缓存setting///////////////////////////
                MTFLine current_line;
                current_line.m_style.lineColor = curveColor;
                current_line.m_style.pointColor = curveColor;
                current_line.m_style.lineWidth = 2;
                current_line.m_style.pointWidth = 1;
                current_line.m_style.pointSize = 5;
                current_line.m_style.lineStyle = Qt::DotLine;
                current_line.m_style.pointStyle = QwtSymbol::Ellipse;
                current_line.m_style.fillPointStyle = eFillEllipse;
                current_line.curve = curve;
                current_line.index = index;              
                settings->m_lines[index] = current_line;
                index++;
                // 更新Y轴范围
                for (double val : wave.aberration) 
                {
                    if (val < minY) minY = val;
                    if (val > maxY) maxY = val;
                }
            }

            // 设置Y轴范围（留一些边距）
            double margin = (maxY - minY) * 0.1;
            if (margin == 0) margin = 0.1; // 防止所有值相同的情况
            plot->setAxisScale(QwtPlot::yLeft, minY - margin, maxY + margin);

            // 添加图例（可点击）
            QwtLegend* legend = new QwtLegend();
            legend->setDefaultItemMode(QwtLegendData::Checkable); // 使图例项可点击
            plot->insertLegend(legend, QwtPlot::BottomLegend);

            // 设置图例位置和最大高度
            plot->plotLayout()->setLegendPosition(QwtPlot::BottomLegend);

            // 连接图例点击信号
            connect(legend, SIGNAL(clicked(const QVariant&, int)),
                this, SLOT(onLegendClicked(const QVariant&)));

            // 设置交互功能（拖拽和缩放）
            setupPlotInteractions(plot);
            
            // 初始化默认值（根据需要设置）
            settings->gridMajorStyle = Qt::DotLine;
            settings->gridMajorColor = Qt::gray;
            settings->gridMinorStyle = Qt::DotLine;
            settings->gridMinorColor = Qt::lightGray;
            settings->gridVisible = true;
            settings->origin = PlotSettings::BottomLeft;
            settings->title = plot->title().text();
            settings->titleFont = QFont();
            settings->titleColor = Qt::black;
            settings->backgroundColor = Qt::white;
            settings->legend.visible = true;
            settings->legend.position = QwtPlot::BottomLegend;
            settings->xAxis.visible = true;
            settings->xAxis.title = "归一化光阑";
            settings->xAxis.autoRange = true;
            settings->xAxis.min = -1.1;
            settings->xAxis.max = 1.1;
            settings->xAxis.step = 0.2;
            settings->yAxis.visible = true;
            settings->yAxis.title = "光线像差 (mm)";
            settings->yAxis.autoRange = true;  // 自动范围由数据决定
            settings->yAxis.min = 0;
            settings->yAxis.max = 1;
            settings->yAxis.step = 0.1;        

            // 自动调整布局
            plot->updateLayout();
            // 重绘图表
            plot->replot();

            // 添加到网格布局
            m_mainLayout->addWidget(plot, row_idx, col_idx);
            m_plots.append(plot);

            PlotInfo* info = new PlotInfo();
            info->plot = plot;
            info->grid = grid;
            info->legend = legend;
            info->settings = settings;
            info->curves = m_curves;
            //缓存
            info->settings->xAxis.min = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            info->settings->xAxis.max = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
            info->settings->yAxis.min = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            info->settings->yAxis.max = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
            saveInitialView(*info);
            m_plotInfos.append(info);

            m_curves.clear();

            row_idx++;
        }
        col_idx++;
    }
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
    return true;
}

void AberrationWidget::setupPlotInteractions(QwtPlot* plot)
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

    connect(panner, &MyPlotPanner::panFinished, this, &AberrationWidget::updateAxesSettings_noparam);
    connect(magnifier, &MyPlotMagnifier::zoomed, this, &AberrationWidget::updateAxesSettings_noparam);

    // 3. 右键菜单用于重置视图（可选功能）
    plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AberrationWidget::onLegendClicked(const QVariant& itemInfo)
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

QColor AberrationWidget::getColorFromString(const QString& colorStr)
{
    if (colorStr == "RGBOrange") return QColor(255, 165, 0);
    else if (colorStr == "RGBYellow") return QColor(255, 255, 0);
    else if (colorStr == "RGBGreen") return QColor(0, 255, 0);
    else if (colorStr == "RGBCyan") return QColor(0, 255, 255);
    else if (colorStr == "RGBRed") return QColor(255, 0, 0);
    else if (colorStr == "RGBBlue") return QColor(0, 0, 255);
    else if (colorStr.contains("nm")) {
        // 处理波长值，根据波长返回不同颜色
        QString numStr = colorStr;
        numStr.replace("nm", "").trimmed();
        bool ok;
        double wavelength = numStr.toDouble(&ok);
        if (ok) {
            // 根据波长返回颜色（简化版）
            if (wavelength < 480) return QColor(0, 0, 255);    // 蓝色
            else if (wavelength < 510) return QColor(0, 255, 255); // 青色
            else if (wavelength < 550) return QColor(0, 255, 0);   // 绿色
            else if (wavelength < 590) return QColor(255, 255, 0); // 黄色
            else if (wavelength < 630) return QColor(255, 165, 0); // 橙色
            else return QColor(255, 0, 0); // 红色
        }
    }
    return QColor(0, 0, 0); // 默认黑色
}
