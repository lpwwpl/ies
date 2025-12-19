//#include "SpotDiagramWidget.h"
//#include <QDebug>
//#include <cmath>
//
//SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
//    : QWidget(parent), m_dataScale(20.0), m_isInitialPlot(true) 
//{
//    // 初始化颜色映射
//    m_colorMap["RGBOrange"] = QColor(255, 165, 0);
//    m_colorMap["RGBYellow"] = QColor(255, 255, 0);
//    m_colorMap["RGBGreen"] = QColor(0, 255, 0);
//    m_colorMap["RGBCyan"] = QColor(0, 255, 255);
//
//    // 创建界面
//    QVBoxLayout* mainLayout = new QVBoxLayout(this);
//    m_plot = new QCustomPlot(this);
//    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);
//    // 连接自定义上下文菜单
//    connect(m_plot, &QCustomPlot::customContextMenuRequested, this, [this](const QPoint& pos) {
//        QMenu menu(this);
//        QAction* resetAction = menu.addAction("重置视图");
//        //QAction* zoomToField1Action = menu.addAction("zoom 1");
//
//        QAction* selectedAction = menu.exec(m_plot->mapToGlobal(pos));
//
//        if (selectedAction == resetAction) {
//            resetView();
//        }
//        //else if (selectedAction == zoomToField1Action) {
//        //    zoomToFieldIndex(1);
//        //}
//        });
//
//    mainLayout->addWidget(m_plot);
//
//
//    //m_plot->axisRect()->setMinimumSize(QSize(480, 480));
//    //m_plot->axisRect()->setMaximumSize(QSize(480, 480));
//    setupPlot();
//    resize(QSize(640, 480));
//}
//
//bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
//{
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        return false;
//    }
//
//    m_spotData.clear();
//    m_fieldRMS.clear();
//    m_field100.clear();
//    m_airyX.clear();
//    m_airyY.clear();
//    m_spotCenters.clear();
//    m_fieldGraphs.clear();
//    m_fieldLabels.clear();
//    m_fieldData.clear();
//
//    QTextStream in(&file);
//    bool inDataSection = false;
//
//    while (!in.atEnd()) {
//        QString line = in.readLine().trimmed();
//
//        // 检查是否进入RMS数据部分
//        if (line.contains("RMS") && !line.contains("INDEX")) {
//            parseRMSData(in);
//            continue;
//        }
//
//        // 检查是否进入100%数据部分
//        if (line.contains("100%")) {
//            parse100PercentData(in);
//            continue;
//        }
//
//        // 检查是否进入AIRY数据部分
//        if (line.contains("AIRY")) {
//            parseAiryData(in);
//            continue;
//        }
//
//        // 检查是否进入数据部分
//        if (line.startsWith("INDEX")) {
//            inDataSection = true;
//            continue;
//        }
//
//        if (line.startsWith("#") || line.isEmpty()) {
//            continue;
//        }
//
//        if (inDataSection) {
//            parseDataLine(line);
//        }
//    }
//
//    file.close();
//
//    // 按视场索引分组存储数据
//    for (const SpotData& data : m_spotData) {
//        m_fieldData[data.fieldIndex].append(data);
//    }
//
//    // 输出解析结果用于调试
//    qDebug() << "Loaded RMS values:" << m_fieldRMS;
//    qDebug() << "Loaded 100% values:" << m_field100;
//    qDebug() << "Loaded Airy X values:" << m_airyX;
//    qDebug() << "Loaded Airy Y values:" << m_airyY;
//    qDebug() << "Loaded spot data points:" << m_spotData.size();
//    qDebug() << "Field data counts:";
//    for (auto it = m_fieldData.begin(); it != m_fieldData.end(); ++it) {
//        qDebug() << "Field" << it.key() << ":" << it.value().size() << "points";
//    }
//
//    return true;
//}
//
//void SpotDiagramPlotter::parseRMSData(QTextStream& in)
//{
//    // 读取接下来的三行RMS值
//    for (int i = 0; i < 3; i++) {
//        QString line = in.readLine().trimmed();
//        bool ok;
//        double rmsValue = line.toDouble(&ok);
//        if (ok) {
//            m_fieldRMS.append(rmsValue);
//        }
//    }
//}
//
//void SpotDiagramPlotter::parse100PercentData(QTextStream& in)
//{
//    // 读取接下来的三行100%直径值
//    for (int i = 0; i < 3; i++) {
//        QString line = in.readLine().trimmed();
//        bool ok;
//        double hundredValue = line.toDouble(&ok);
//        if (ok) {
//            m_field100.append(hundredValue);
//        }
//    }
//}
//
//void SpotDiagramPlotter::parseAiryData(QTextStream& in)
//{
//    // 跳过RX RY标题行
//    in.readLine();
//
//    // 读取三行Airy直径数据
//    for (int i = 0; i < 3; i++) {
//        QString line = in.readLine().trimmed();
//        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
//        if (parts.size() >= 2) {
//            bool ok1, ok2;
//            double airyX = parts[0].toDouble(&ok1);
//            double airyY = parts[1].toDouble(&ok2);
//            if (ok1 && ok2) {
//                m_airyX.append(airyX);
//                m_airyY.append(airyY);
//            }
//        }
//    }
//}
//
//void SpotDiagramPlotter::parseDataLine(const QString& line)
//{
//    QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
//    if (parts.size() < 7) return;
//
//    bool ok;
//    SpotData data;
//
//    data.index = parts[0].toInt(&ok);
//    if (!ok) return;
//
//    data.x = parts[1].toDouble(&ok);
//    if (!ok) return;
//
//    data.y = parts[2].toDouble(&ok);
//    if (!ok) return;
//
//    data.fieldIndex = parts[3].toInt(&ok);
//    if (!ok) return;
//
//    data.waveIndex = parts[4].toInt(&ok);
//    if (!ok) return;
//
//    data.wavelength = parts[5].toDouble(&ok);
//    if (!ok) return;
//
//    data.colorName = parts[6];
//
//    m_spotData.append(data);
//}
//
//void SpotDiagramPlotter::setupPlot()
//{
//    // 启用上部和右侧坐标轴
//    m_plot->xAxis2->setVisible(true);
//    m_plot->yAxis2->setVisible(true);
//
//    // 设置图表基本属性
//    m_plot->xAxis->setLabel("X (mm)");
//    m_plot->yAxis->setLabel("Field Position");
//
//    // 设置上部和右侧坐标轴的标签
//    m_plot->xAxis2->setLabel("X (mm)");
//    m_plot->yAxis2->setLabel("RMS Diameter");
//
//    // 连接坐标轴范围同步
//    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis2, SLOT(setRange(QCPRange)));
//
//    // 连接坐标轴范围变化信号
//    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onRangeChanged()));
//    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onRangeChanged()));
//
//    // 禁用坐标轴平移，只允许缩放
//    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
//
//
//    // 设置标题
//    m_plot->plotLayout()->insertRow(0);
//    m_plot->plotLayout()->addElement(0, 0,
//        new QCPTextElement(m_plot, QStringLiteral("光学系统点列图分析"), QFont("sans", 14, QFont::Bold)));
// 
//}
//
//void SpotDiagramPlotter::plotSpotDiagrams()
//{
//    // 清除现有图形
//    m_plot->clearPlottables();
//    m_plot->clearItems();
//    m_fieldGraphs.clear();
//    m_fieldLabels.clear();
//
//    // 获取视场索引并排序
//    QList<int> fieldIndices = m_fieldData.keys();
//    std::sort(fieldIndices.begin(), fieldIndices.end());
//
//    // 只取前3个视场
//    int maxFields = qMin(3, fieldIndices.size());
//
//    // 设置X轴范围 - 根据数据范围自适应，并扩大20倍
//    double xMin = 0, xMax = 0;
//    for (const SpotData& data : m_spotData) {
//        xMin = qMin(xMin, data.x);
//        xMax = qMax(xMax, data.x);
//    }
//    xRange = qMax(fabs(xMin), fabs(xMax)) * m_dataScale; // 增加20%边距并扩大
//
//    // 设置Y轴范围 - Y轴范围是X轴范围的3倍
//    double yRange = xRange * 3;
//    yMin = -yRange / 2;
//    yMax = yRange / 2;
//
//    // 计算垂直偏移
//    QMap<int, double> verticalOffsets = calculateVerticalOffsets();
//
//    // 存储点列中心坐标
//    m_spotCenters.clear();
//
//    // 为每个视场创建图形
//    for (int i = 0; i < maxFields; i++) {
//        int fieldIndex = fieldIndices[i];
//        QString fieldName = QString("F%1").arg(fieldIndex + 1);
//
//        // 获取垂直偏移
//        double verticalOffset = verticalOffsets[fieldIndex];
//
//        // 计算点列中心
//        double centerX, centerY;
//        calculateSpotCenter(m_fieldData[fieldIndex], centerX, centerY);
//        m_spotCenters[fieldIndex] = qMakePair(centerX * m_dataScale, centerY * m_dataScale); // 中心坐标扩大
//
//        // 添加点数据到图表（数据扩大）
//        addSpotDataToPlot(m_fieldData[fieldIndex], verticalOffset, fieldName, fieldIndex);
//    }
//
//    // 设置坐标轴范围
//    m_plot->xAxis->setRange(-xRange, xRange);
//    m_plot->yAxis->setRange(yMin, yMax);
//
//    // 保存初始视图范围
//    saveInitialView();
//
//    // 添加信息到刻度
//    addInfoToTicks();
//
//
//    // 标记初始绘制完成
//    m_isInitialPlot = false;
//
//    resetView();
//    //zoomToFieldIndex(0); 
//    // 重新绘制
//    m_plot->replot();
//}
//
//void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
//    const QString& fieldName, int fieldIndex)
//{
//    if (data.isEmpty()) return;
//
//    // 按颜色分组数据
//    QMap<QString, QVector<QCPGraphData>> colorData;
//
//    for (const SpotData& spot : data) {
//        QCPGraphData point;
//        point.key = spot.x * m_dataScale; // X坐标扩大
//        point.value = spot.y * m_dataScale + verticalOffset; // Y坐标扩大并添加垂直偏移
//        colorData[spot.colorName].append(point);
//    }
//
//    // 为每种颜色创建图形
//    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
//        const QString& colorName = it.key();
//        const QVector<QCPGraphData>& points = it.value();
//
//        QCPGraph* graph = m_plot->addGraph();
//        graph->setName(fieldName + " - " + colorName);
//
//        // 设置颜色和样式
//        QColor color = m_colorMap.value(colorName, Qt::black);
//        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, color, 4));
//        graph->setLineStyle(QCPGraph::lsNone);
//
//        // 添加数据
//        graph->data()->set(points);
//
//        // 存储图形对象
//        m_fieldGraphs[fieldIndex].append(graph);
//    }
//}
//
//void SpotDiagramPlotter::calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY)
//{
//    if (data.isEmpty()) {
//        centerX = 0;
//        centerY = 0;
//        return;
//    }
//
//    double sumX = 0, sumY = 0;
//    for (const SpotData& spot : data) {
//        sumX += spot.x;
//        sumY += spot.y;
//    }
//
//    centerX = sumX / data.size();
//    centerY = sumY / data.size();
//}
//
//void SpotDiagramPlotter::addInfoToTicks()
//{
//    // 获取视场索引并排序
//    QList<int> fieldIndices = m_fieldData.keys();
//    std::sort(fieldIndices.begin(), fieldIndices.end());
//
//    // 只取前3个视场
//    int maxFields = qMin(3, fieldIndices.size());
//
//    // 计算垂直偏移
//    QMap<int, double> verticalOffsets = calculateVerticalOffsets();
//
//    // 准备刻度位置和标签
//    QVector<double> leftTickPositions;
//    QVector<QString> leftTickLabels;
//    QVector<double> rightTickPositions;
//    QVector<QString> rightTickLabels;
//
//    for (int i = 0; i < maxFields; i++) {
//        int fieldIndex = fieldIndices[i];
//        double verticalOffset = verticalOffsets[fieldIndex];
//        QPair<double, double> center = m_spotCenters[fieldIndex];
//
//        // 左侧刻度位置和标签
//        leftTickPositions.append(verticalOffset);
//        QString leftLabel = QString("F%1\nX:%2\nY:%3")
//            .arg(fieldIndex + 1)
//            .arg(center.first, 0, 'f', 4)
//            .arg(center.second, 0, 'f', 4);
//        leftTickLabels.append(leftLabel);
//
//        // 右侧刻度位置和标签
//        rightTickPositions.append(verticalOffset);
//        if (fieldIndex < m_fieldRMS.size() && fieldIndex < m_field100.size()) {
//            QString rightLabel = QString("F%1\nRMS:%2\n100%:%3")
//                .arg(fieldIndex + 1)
//                .arg(m_fieldRMS[fieldIndex] * m_dataScale, 0, 'f', 4)
//                .arg(m_field100[fieldIndex] * m_dataScale, 0, 'f', 4);
//            rightTickLabels.append(rightLabel);
//        }
//    }
//
//    // 创建自定义刻度器
//    QSharedPointer<QCPAxisTickerText> leftTicker(new QCPAxisTickerText);
//    QSharedPointer<QCPAxisTickerText> rightTicker(new QCPAxisTickerText);
//
//    // 设置左侧刻度
//    for (int i = 0; i < leftTickPositions.size(); i++) {
//        leftTicker->addTick(leftTickPositions[i], leftTickLabels[i]);
//    }
//
//    // 设置右侧刻度
//    for (int i = 0; i < rightTickPositions.size(); i++) {
//        rightTicker->addTick(rightTickPositions[i], rightTickLabels[i]);
//    }
//
//    // 应用刻度器
//    m_plot->yAxis->setTicker(leftTicker);
//    m_plot->yAxis2->setTicker(rightTicker);
//
//    // 设置刻度标签样式
//    m_plot->yAxis->setTickLabelRotation(0);
//    m_plot->yAxis->setTickLabelFont(QFont("Arial", 8));
//    m_plot->yAxis2->setTickLabelRotation(0);
//    m_plot->yAxis2->setTickLabelFont(QFont("Arial", 8));
//}
//
//void SpotDiagramPlotter::onRangeChanged()
//{
//    // 当坐标轴范围变化时，更新点列位置
//    if (!m_isInitialPlot) {
//        updateSpotPositions();
//    }
//}
//
//void SpotDiagramPlotter::updateSpotPositions()
//{
//    // 计算新的垂直偏移
//    QMap<int, double> verticalOffsets = calculateVerticalOffsets();
//
//    // 更新每个视场的点列位置
//    for (auto it = m_fieldGraphs.begin(); it != m_fieldGraphs.end(); ++it) {
//        int fieldIndex = it.key();
//        double verticalOffset = verticalOffsets[fieldIndex];
//
//        // 获取该视场的原始数据
//        const QVector<SpotData>& fieldSpotData = m_fieldData[fieldIndex];
//
//        // 重新计算该视场下所有图形的数据
//        for (QCPGraph* graph : it.value()) {
//            // 按颜色重新分组数据
//            QMap<QString, QVector<QCPGraphData>> colorData;
//
//            for (const SpotData& spot : fieldSpotData) {
//                QCPGraphData point;
//                point.key = spot.x * m_dataScale; // X坐标扩大
//                point.value = spot.y * m_dataScale + verticalOffset; // Y坐标扩大并添加新的垂直偏移
//
//                // 根据颜色分组
//                if (graph->name().contains(spot.colorName)) {
//                    colorData[spot.colorName].append(point);
//                }
//            }
//
//            // 更新图形数据
//            for (auto colorIt = colorData.begin(); colorIt != colorData.end(); ++colorIt) {
//                graph->data()->set(colorIt.value());
//            }
//        }
//    }
//
//    // 更新刻度信息
//    addInfoToTicks();
//
//    // 重新绘制
//    m_plot->replot();
//}
//
//QMap<int, double> SpotDiagramPlotter::calculateVerticalOffsets()
//{
//    QMap<int, double> verticalOffsets;
//
//    // 获取当前Y轴范围
//    yMin = m_plot->yAxis->range().lower;
//    yMax = m_plot->yAxis->range().upper;
//    double yRange = yMax - yMin;
//
//    // 获取视场索引并排序
//    QList<int> fieldIndices = m_fieldGraphs.keys();
//    std::sort(fieldIndices.begin(), fieldIndices.end());
//
//    int maxFields = fieldIndices.size();
//
//    if (maxFields == 0) return verticalOffsets;
//
//    // 专门为三个点列设计的分布算法
//    if (maxFields == 3) {
//        // 将Y轴范围分成4等份，三个点列分别位于1/4、2/4和3/4的位置
//        double segmentHeight = yRange / 4.0;
//        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;           // F1 - 底部第一个点列
//        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;       // F2 - 中间点列
//        verticalOffsets[fieldIndices[2]] = yMin + 3 * segmentHeight;       // F3 - 顶部点列
//    }
//    else if (maxFields == 2) {
//        // 两个点列的情况：分别位于1/3和2/3的位置
//        double segmentHeight = yRange / 3.0;
//        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;           // F1
//        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;       // F2
//    }
//    else if (maxFields == 1) {
//        // 单个点列位于中心
//        verticalOffsets[fieldIndices[0]] = (yMin + yMax) / 2.0;            // F1
//    }
//
//    return verticalOffsets;
//}
//
//
//
//
//void SpotDiagramPlotter::resetView()
//{
//    // 重置到初始视图
//    restoreInitialView();
//}
//
//void SpotDiagramPlotter::saveInitialView()
//{
//    // 保存初始视图范围
//    m_initialXRange = m_plot->xAxis->range();
//    m_initialYRange = m_plot->yAxis->range();
//}
//
//void SpotDiagramPlotter::restoreInitialView()
//{
//    // 恢复初始视图范围
//    m_plot->xAxis->setRange(m_initialXRange);
//    m_plot->yAxis->setRange(m_initialYRange );
//
//    // 更新点列位置
//    updateSpotPositions();
//
//    // 重新绘制
//    m_plot->replot();
//}


#include "SpotDiagramWidget.h"
#include <cmath>
#include <QResizeEvent>

SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
    : QWidget(parent), m_dataScale(20.0)
{
    // 初始化颜色映射
    m_colorMap["RGBOrange"] = QColor(255, 165, 0);
    m_colorMap["RGBYellow"] = QColor(255, 255, 0);
    m_colorMap["RGBGreen"] = QColor(0, 255, 0);
    m_colorMap["RGBCyan"] = QColor(0, 255, 255);

    // 创建界面
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_plot = new QwtPlot(this);
    m_plot->setCanvasBackground(Qt::white);
    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_plot, &QwtPlot::customContextMenuRequested,
        this, &SpotDiagramPlotter::showContextMenu);

    mainLayout->addWidget(m_plot);
    resize(QSize(640, 480));

    setupPlot();
}

SpotDiagramPlotter::~SpotDiagramPlotter()
{
    clearPlot();
}

void SpotDiagramPlotter::setupPlot()
{
    // 设置标题
    QwtText title("光学系统点列图分析");
    title.setFont(QFont("sans", 14, QFont::Bold));
    m_plot->setTitle(title);

    // 设置坐标轴标签
    m_plot->setAxisTitle(QwtPlot::xBottom, "X (mm)");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Field Position");

    // 启用右侧坐标轴
    m_plot->enableAxis(QwtPlot::yRight);
    m_plot->setAxisTitle(QwtPlot::yRight, "RMS Diameter");

    // 设置坐标轴字体
    QFont axisFont("Arial", 10);
    m_plot->setAxisFont(QwtPlot::xBottom, axisFont);
    m_plot->setAxisFont(QwtPlot::yLeft, axisFont);
    m_plot->setAxisFont(QwtPlot::yRight, axisFont);

    // 添加网格
    m_grid = new QwtPlotGrid();
    m_grid->enableXMin(true);
    m_grid->enableYMin(true);
    m_grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
    m_grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
    m_grid->attach(m_plot);

    // 创建缩放器
    m_zoomer = new QwtPlotZoomer(m_plot->canvas());
    m_zoomer->setRubberBandPen(QPen(Qt::blue));
    m_zoomer->setTrackerPen(QPen(Qt::blue));
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);

    // 创建平移器
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::MiddleButton);

    // 设置图例
    setupLegend();
}

void SpotDiagramPlotter::setupLegend()
{
    QwtLegend* legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    m_plot->insertLegend(legend, QwtPlot::RightLegend);

    // 连接图例选中事件
    connect(legend, &QwtLegend::checked,
        [this](const QVariant& itemInfo, bool on, int index) {
            QwtPlotItem* plotItem = m_plot->infoToItem(itemInfo);
            if (plotItem) {
                plotItem->setVisible(on);
                m_plot->replot();
            }
        });
}

bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    clearPlot();
    m_spotData.clear();
    m_fieldRMS.clear();
    m_field100.clear();
    m_airyX.clear();
    m_airyY.clear();
    m_spotCenters.clear();
    m_fieldMarkers.clear();
    m_fieldLabels.clear();
    m_fieldData.clear();
    m_originalData.clear();

    QTextStream in(&file);
    bool inDataSection = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // 检查是否进入RMS数据部分
        if (line.contains("RMS") && !line.contains("INDEX")) {
            parseRMSData(in);
            continue;
        }

        // 检查是否进入100%数据部分
        if (line.contains("100%")) {
            parse100PercentData(in);
            continue;
        }

        // 检查是否进入AIRY数据部分
        if (line.contains("AIRY")) {
            parseAiryData(in);
            continue;
        }

        // 检查是否进入数据部分
        if (line.startsWith("INDEX")) {
            inDataSection = true;
            continue;
        }

        if (line.startsWith("#") || line.isEmpty()) {
            continue;
        }

        if (inDataSection) {
            parseDataLine(line);
        }
    }

    file.close();

    // 按视场索引分组存储数据
    for (const SpotData& data : m_spotData) {
        m_fieldData[data.fieldIndex].append(data);
    }

    // 输出解析结果用于调试
    qDebug() << "Loaded RMS values:" << m_fieldRMS;
    qDebug() << "Loaded 100% values:" << m_field100;
    qDebug() << "Loaded Airy X values:" << m_airyX;
    qDebug() << "Loaded Airy Y values:" << m_airyY;
    qDebug() << "Loaded spot data points:" << m_spotData.size();
    qDebug() << "Field data counts:";
    for (auto it = m_fieldData.begin(); it != m_fieldData.end(); ++it) {
        qDebug() << "Field" << it.key() << ":" << it.value().size() << "points";
    }

    return true;
}

void SpotDiagramPlotter::parseRMSData(QTextStream& in)
{
    for (int i = 0; i < 3; i++) {
        QString line = in.readLine().trimmed();
        bool ok;
        double rmsValue = line.toDouble(&ok);
        if (ok) {
            m_fieldRMS.append(rmsValue);
        }
    }
}

void SpotDiagramPlotter::parse100PercentData(QTextStream& in)
{
    for (int i = 0; i < 3; i++) {
        QString line = in.readLine().trimmed();
        bool ok;
        double hundredValue = line.toDouble(&ok);
        if (ok) {
            m_field100.append(hundredValue);
        }
    }
}

void SpotDiagramPlotter::parseAiryData(QTextStream& in)
{
    // 跳过RX RY标题行
    in.readLine();

    for (int i = 0; i < 3; i++) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool ok1, ok2;
            double airyX = parts[0].toDouble(&ok1);
            double airyY = parts[1].toDouble(&ok2);
            if (ok1 && ok2) {
                m_airyX.append(airyX);
                m_airyY.append(airyY);
            }
        }
    }
}

void SpotDiagramPlotter::parseDataLine(const QString& line)
{
    QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
    if (parts.size() < 7) return;

    bool ok;
    SpotData data;

    data.index = parts[0].toInt(&ok);
    if (!ok) return;

    data.x = parts[1].toDouble(&ok);
    if (!ok) return;

    data.y = parts[2].toDouble(&ok);
    if (!ok) return;

    data.fieldIndex = parts[3].toInt(&ok);
    if (!ok) return;

    data.waveIndex = parts[4].toInt(&ok);
    if (!ok) return;

    data.wavelength = parts[5].toDouble(&ok);
    if (!ok) return;

    data.colorName = parts[6];

    m_spotData.append(data);
}

void SpotDiagramPlotter::plotSpotDiagrams()
{
    clearPlot();

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    // 只取前3个视场
    int maxFields = qMin(3, fieldIndices.size());

    // 设置X轴范围 - 根据数据范围自适应，并扩大20倍
    double xMin = 0, xMax = 0;
    for (const SpotData& data : m_spotData) {
        xMin = qMin(xMin, data.x);
        xMax = qMax(xMax, data.x);
    }
    xRange = qMax(fabs(xMin), fabs(xMax)) * m_dataScale;

    // 设置Y轴范围 - Y轴范围是X轴范围的3倍
    double yRange = xRange * 3;
    yMin = -yRange / 2;
    yMax = yRange / 2;

    // 计算垂直偏移
    QMap<int, double> verticalOffsets = calculateVerticalOffsets();

    // 存储原始数据（用于动态更新）
    m_originalData.clear();

    // 为每个视场创建图形
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        QString fieldName = QString("F%1").arg(fieldIndex + 1);

        // 获取垂直偏移
        double verticalOffset = verticalOffsets[fieldIndex];

        // 计算点列中心
        double centerX, centerY;
        calculateSpotCenter(m_fieldData[fieldIndex], centerX, centerY);
        m_spotCenters[fieldIndex] = qMakePair(centerX * m_dataScale, centerY * m_dataScale);

        // 存储原始数据
        for (const SpotData& spot : m_fieldData[fieldIndex]) {
            QPointF point(spot.x, spot.y);
            m_originalData[fieldIndex][spot.colorName].append(point);
        }

        // 添加点数据到图表
        addSpotDataToPlot(m_fieldData[fieldIndex], verticalOffset, fieldName, fieldIndex);
    }

    // 设置坐标轴范围
    m_plot->setAxisScale(QwtPlot::xBottom, -xRange, xRange);
    m_plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);

    // 保存初始视图范围
    saveInitialView();

    // 添加信息到刻度
    addInfoToTicks();

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
    const QString& fieldName, int fieldIndex)
{
    if (data.isEmpty()) return;

    // 按颜色分组数据
    QMap<QString, QVector<QPointF>> colorData;

    for (const SpotData& spot : data) {
        QPointF point;
        point.setX(spot.x * m_dataScale);
        point.setY(spot.y * m_dataScale + verticalOffset);
        colorData[spot.colorName].append(point);
    }

    // 为每种颜色创建曲线
    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
        const QString& colorName = it.key();
        const QVector<QPointF>& points = it.value();

        // 创建曲线
        QwtPlotCurve* curve = new QwtPlotCurve(fieldName + " - " + colorName);
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        curve->setStyle(QwtPlotCurve::NoCurve);
        // 设置颜色
        QColor color = m_colorMap.value(colorName, Qt::black);
        curve->setPen(QPen(color));

        // 设置符号（点）
        QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse,
            QBrush(color),
            QPen(color, 1),
            QSize(6, 6));
        curve->setSymbol(symbol);

        // 设置数据
        QVector<QPointF> plotPoints;
        for (const QPointF& point : points) {
            plotPoints.append(point);
        }
        curve->setSamples(plotPoints);

        // 附加到图表
        curve->attach(m_plot);
        m_curves.append(curve);
    }
}

void SpotDiagramPlotter::calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY)
{
    if (data.isEmpty()) {
        centerX = 0;
        centerY = 0;
        return;
    }

    double sumX = 0, sumY = 0;
    for (const SpotData& spot : data) {
        sumX += spot.x;
        sumY += spot.y;
    }

    centerX = sumX / data.size();
    centerY = sumY / data.size();
}

void SpotDiagramPlotter::addInfoToTicks()
{
    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    // 只取前3个视场
    int maxFields = qMin(3, fieldIndices.size());

    // 计算垂直偏移
    QMap<int, double> verticalOffsets = calculateVerticalOffsets();

    // 清除旧标签
    for (auto marker : m_markers) {
        delete marker;
    }
    m_markers.clear();

    // 添加左侧标签
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = verticalOffsets[fieldIndex];
        QPair<double, double> center = m_spotCenters[fieldIndex];

        QString label = QString("F%1\nX:%2\nY:%3")
            .arg(fieldIndex + 1)
            .arg(center.first, 0, 'f', 4)
            .arg(center.second, 0, 'f', 4);

        // 创建文本标记
        QwtPlotMarker* marker = new QwtPlotMarker();
        marker->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QwtText text(label);
        text.setFont(QFont("Arial", 8));
        marker->setLabel(text);

        marker->setValue(0, verticalOffset);
        marker->setSpacing(5);

        marker->attach(m_plot);
        m_markers.append(marker);
    }

    // 添加右侧标签
    //for (int i = 0; i < maxFields; i++) {
    //    int fieldIndex = fieldIndices[i];
    //    double verticalOffset = verticalOffsets[fieldIndex];

    //    if (fieldIndex < m_fieldRMS.size() && fieldIndex < m_field100.size()) {
    //        QString label = QString("F%1\nRMS:%2\n100%:%3")
    //            .arg(fieldIndex + 1)
    //            .arg(m_fieldRMS[fieldIndex] * m_dataScale, 0, 'f', 4)
    //            .arg(m_field100[fieldIndex] * m_dataScale, 0, 'f', 4);

    //        // 创建文本标记
    //        QwtPlotMarker* marker = new QwtPlotMarker();
    //        marker->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //        QwtText text(label);
    //        text.setFont(QFont("Arial", 8));
    //        marker->setLabel(text);

    //        marker->setValue(0, verticalOffset);
    //        marker->setSpacing(5);

    //        marker->attach(m_plot);
    //        m_markers.append(marker);
    //    }
    //}
}

void SpotDiagramPlotter::updateSpotPositions()
{
    // 计算新的垂直偏移
    QMap<int, double> verticalOffsets = calculateVerticalOffsets();

    // 更新每个视场的点列位置
    for (auto it = m_originalData.begin(); it != m_originalData.end(); ++it) {
        int fieldIndex = it.key();
        double verticalOffset = verticalOffsets[fieldIndex];

        // 遍历该视场下的所有颜色组
        for (auto colorIt = it.value().begin(); colorIt != it.value().end(); ++colorIt) {
            const QString& colorName = colorIt.key();
            const QVector<QPointF>& originalPoints = colorIt.value();

            // 创建新的点集
            QVector<QPointF> newPoints;
            for (const QPointF& point : originalPoints) {
                QPointF newPoint;
                newPoint.setX(point.x() * m_dataScale);
                newPoint.setY(point.y() * m_dataScale + verticalOffset);
                newPoints.append(newPoint);
            }

            // 更新对应的曲线
            for (QwtPlotCurve* curve : m_curves) {
                if (curve->title().text().contains(QString("F%1").arg(fieldIndex + 1)) &&
                    curve->title().text().contains(colorName)) {
                    curve->setSamples(newPoints);
                    break;
                }
            }
        }
    }

    // 更新标签位置
    addInfoToTicks();

    // 重新绘制
    m_plot->replot();
}

QMap<int, double> SpotDiagramPlotter::calculateVerticalOffsets()
{
    QMap<int, double> verticalOffsets;

    // 获取当前Y轴范围
    yMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    yMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
    double yRange = yMax - yMin;

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    int maxFields = qMin(3, fieldIndices.size());

    if (maxFields == 0) return verticalOffsets;

    // 专门为三个点列设计的分布算法
    if (maxFields == 3) {
        double segmentHeight = yRange / 4.0;
        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;
        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;
        verticalOffsets[fieldIndices[2]] = yMin + 3 * segmentHeight;
    }
    else if (maxFields == 2) {
        double segmentHeight = yRange / 3.0;
        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;
        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;
    }
    else if (maxFields == 1) {
        verticalOffsets[fieldIndices[0]] = (yMin + yMax) / 2.0;
    }

    return verticalOffsets;
}

void SpotDiagramPlotter::resetView()
{
    restoreInitialView();
}

void SpotDiagramPlotter::showContextMenu(const QPoint& pos)
{
    QMenu menu(this);
    QAction* resetAction = menu.addAction("重置视图");

    QAction* selectedAction = menu.exec(m_plot->mapToGlobal(pos));

    if (selectedAction == resetAction) {
        resetView();
    }
}

void SpotDiagramPlotter::saveInitialView()
{
    double xMin = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
    double xMax = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
    double yMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    double yMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    m_initialView = QRectF(xMin, yMin, xMax - xMin, yMax - yMin);
}

void SpotDiagramPlotter::restoreInitialView()
{
    if (!m_initialView.isEmpty()) {
        m_plot->setAxisScale(QwtPlot::xBottom,
            m_initialView.left(),
            m_initialView.left() + m_initialView.width());
        m_plot->setAxisScale(QwtPlot::yLeft,
            m_initialView.top(),
            m_initialView.top() + m_initialView.height());

        // 更新点列位置
        updateSpotPositions();

        // 重新绘制
        m_plot->replot();
    }
}

void SpotDiagramPlotter::clearPlot()
{
    // 清除曲线
    for (QwtPlotCurve* curve : m_curves) {
        curve->detach();
        delete curve;
    }
    m_curves.clear();

    // 清除标记
    for (QwtPlotMarker* marker : m_markers) {
        marker->detach();
        delete marker;
    }
    m_markers.clear();

    // 清除其他项目
    m_fieldMarkers.clear();
    m_fieldLabels.clear();
}