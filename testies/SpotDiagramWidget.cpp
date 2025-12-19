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
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_canvas.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include <QTimer>
// 初始化静态成员变量
constexpr double SpotDiagramPlotter::FIELD_SPACING;
constexpr double SpotDiagramPlotter::LEFT_AXIS_OFFSET;

SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
    : QWidget(parent), m_dataScale(20.0), m_isInitialPlot(true),
    m_maxSpotXRange(0.0), m_maxSpotYRange(0.0),
    m_leftScaleDraw(nullptr), m_rightScaleDraw(nullptr)
{
    // 初始化颜色映射
    m_colorMap["RGBOrange"] = QColor(255, 165, 0);
    m_colorMap["RGBYellow"] = QColor(255, 255, 0);
    m_colorMap["RGBGreen"] = QColor(0, 255, 0);
    m_colorMap["RGBCyan"] = QColor(0, 255, 255);

    // 创建界面
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_plot = new QwtPlot(this);
    m_plot->setCanvasBackground(Qt::white);

    // 设置右键菜单策略
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu menu(this);
        QAction* resetAction = menu.addAction("重置视图");
        QAction* selectedAction = menu.exec(mapToGlobal(pos));

        if (selectedAction == resetAction) {
            resetView();
        }
        });

    mainLayout->addWidget(m_plot);

    // 设置初始大小
    resize(QSize(640, 480));

    // 设置图表
    setupPlot();
}

SpotDiagramPlotter::~SpotDiagramPlotter()
{
    clearAllCurves();
    if (m_leftScaleDraw) delete m_leftScaleDraw;
    if (m_rightScaleDraw) delete m_rightScaleDraw;
}

bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // 清除现有数据
    m_spotData.clear();
    m_fieldRMS.clear();
    m_field100.clear();
    m_airyX.clear();
    m_airyY.clear();
    m_spotCenters.clear();
    clearAllCurves();
    m_fieldData.clear();
    m_originalFieldCenters.clear();

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

    return true;
}

void SpotDiagramPlotter::parseRMSData(QTextStream& in)
{
    // 读取接下来的三行RMS值
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
    // 读取接下来的三行100%直径值
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

    // 读取三行Airy直径数据
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

void SpotDiagramPlotter::setupPlot()
{
    // 启用右侧坐标轴
    m_plot->enableAxis(QwtPlot::yRight, true);

    // 设置网格
    m_grid = new QwtPlotGrid();
    m_grid->setPen(QPen(Qt::gray, 0, Qt::DotLine));
    m_grid->attach(m_plot);

    // 添加缩放功能 - 右键拖拽矩形放大
    m_zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
        m_plot->canvas());
    m_zoomer->setRubberBandPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
    m_zoomer->setTrackerPen(QPen(Qt::darkBlue));
    m_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    m_zoomer->setTrackerMode(QwtPicker::ActiveOnly);

    // 设置鼠标模式：右键用于拖拽矩形放大
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton, Qt::AltModifier);

    // 连接缩放信号
    connect(m_zoomer, &QwtPlotZoomer::zoomed, this, &SpotDiagramPlotter::handleZoomed);

    // 设置缩放器的缩放栈最大深度
    m_zoomer->setMaxStackDepth(10);

    // 添加平移功能 - 左键移动
    m_panner = new QwtPlotPanner(m_plot->canvas());
    // 设置左键为平移
    m_panner->setMouseButton(Qt::LeftButton);

    // 连接平移信号
    connect(m_panner, &QwtPlotPanner::panned, this, &SpotDiagramPlotter::handlePanned);

    // 设置自动重绘
    m_plot->setAutoReplot(true);

    // 安装事件过滤器
    m_plot->canvas()->installEventFilter(this);
}

QString SpotDiagramPlotter::getFieldName(int fieldIndex) const
{
    switch (fieldIndex) {
    case 0: return "F1";
    case 1: return "F1";
    case 2: return "F2";
    default: return QString("视场%1").arg(fieldIndex + 1);
    }
}

void SpotDiagramPlotter::clearAllCurves()
{
    // 清除所有曲线
    for (auto it = m_fieldCurves.begin(); it != m_fieldCurves.end(); ++it) {
        for (QwtPlotCurve* curve : it.value()) {
            curve->detach();
            delete curve;
        }
    }
    m_fieldCurves.clear();

    // 清除所有标记
    for (auto it = m_fieldMarkers.begin(); it != m_fieldMarkers.end(); ++it) {
        for (QwtPlotMarker* marker : it.value()) {
            marker->detach();
            delete marker;
        }
    }
    m_fieldMarkers.clear();
}

void SpotDiagramPlotter::calculateSpotDataRange()
{
    m_maxSpotXRange = 0.0;
    m_maxSpotYRange = 0.0;

    // 对于每个视场，计算其点列相对于中心的最大X和Y偏离
    for (auto it = m_fieldData.begin(); it != m_fieldData.end(); ++it) {
        const QVector<SpotData>& data = it.value();
        if (data.isEmpty()) continue;

        // 计算该视场的中心
        double centerX, centerY;
        calculateSpotCenter(data, centerX, centerY);

        // 计算该视场点列的最大X和Y偏离
        double maxXRangeForField = 0.0;
        double maxYRangeForField = 0.0;
        for (const SpotData& spot : data) {
            double dx = fabs(spot.x - centerX);
            double dy = fabs(spot.y - centerY);
            maxXRangeForField = qMax(maxXRangeForField, dx);
            maxYRangeForField = qMax(maxYRangeForField, dy);
        }

        // 更新全局最大范围
        m_maxSpotXRange = qMax(m_maxSpotXRange, maxXRangeForField);
        m_maxSpotYRange = qMax(m_maxSpotYRange, maxYRangeForField);
    }

    // 如果没有数据，设置一个默认范围
    if (m_maxSpotXRange <= 0) m_maxSpotXRange = 1.0;
    if (m_maxSpotYRange <= 0) m_maxSpotYRange = 1.0;
}

void SpotDiagramPlotter::plotSpotDiagrams()
{
    // 清除现有图形
    clearAllCurves();

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    // 只取前3个视场
    int maxFields = qMin(3, fieldIndices.size());

    // 计算点列的实际范围
    calculateSpotDataRange();

    // 设置坐标轴范围
    // 每个点列在Y方向上的范围（从中心到边缘）
    double singleSpotYRange = m_maxSpotYRange * m_dataScale;

    // 每个点列在X方向上的范围
    double singleSpotXRange = m_maxSpotXRange * m_dataScale;

    // Y轴：3组点列，每组Y中心间隔3.4
    // 第一个点列中心在Y=0，第二个在Y=3.4，第三个在Y=6.8
    // Y轴范围需要包含所有点列
    double yMin = -singleSpotYRange;  // 第一个点列的最低点
    double yMax = (maxFields - 1) * FIELD_SPACING + singleSpotYRange;  // 最后一个点列的最高点

    // X轴：为Y轴范围的1/3
    double totalYRange = yMax - yMin;
    double totalXRange = totalYRange / 3.0;
    double xMin = -totalXRange / 2.0;
    double xMax = totalXRange / 2.0;

    // 存储点列中心坐标
    m_spotCenters.clear();
    m_originalFieldCenters.clear();

    // 为每个视场创建图形
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        QString fieldName = getFieldName(fieldIndex);

        // 计算垂直偏移：每个点列的Y中心位置
        double verticalOffset = i * FIELD_SPACING;

        // 保存原始的中心位置
        m_originalFieldCenters[fieldIndex] = verticalOffset;

        // 计算点列中心（原始中心坐标）
        double centerX, centerY;
        calculateSpotCenter(m_fieldData[fieldIndex], centerX, centerY);
        m_spotCenters[fieldIndex] = qMakePair(centerX * m_dataScale, centerY * m_dataScale); // 中心坐标扩大

        // 添加点数据到图表（数据扩大并居中）
        addSpotDataToPlot(m_fieldData[fieldIndex], verticalOffset, fieldName, fieldIndex, centerX, centerY);
    }

    // 设置坐标轴范围
    m_plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    m_plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    m_plot->setAxisScale(QwtPlot::yRight, yMin, yMax);

    // 保存初始视图范围
    saveInitialView();

    // 添加信息到刻度
    addInfoToTicks();

    // 同步左右Y轴刻度
    synchronizeAxisTicks();

    // 标记初始绘制完成
    m_isInitialPlot = false;

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
    const QString& fieldName, int fieldIndex, double centerX, double centerY)
{
    if (data.isEmpty()) return;

    // 按颜色分组数据
    QMap<QString, QVector<QPointF>> colorData;

    for (const SpotData& spot : data) {
        QPointF point;
        // 点列居中：将每个点减去其视场中心
        point.setX((spot.x - centerX) * m_dataScale); // X坐标减去中心并扩大
        // Y坐标减去中心并扩大，然后添加垂直偏移
        point.setY((spot.y - centerY) * m_dataScale + verticalOffset);
        colorData[spot.colorName].append(point);
    }

    // 为每种颜色创建曲线
    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
        const QString& colorName = it.key();
        const QVector<QPointF>& points = it.value();

        QwtPlotCurve* curve = new QwtPlotCurve(fieldName + " - " + colorName);

        // 设置数据
        QPolygonF polygon;
        for (const QPointF& point : points) {
            polygon.append(point);
        }
        curve->setSamples(polygon);

        // 设置颜色和样式
        QColor color = m_colorMap.value(colorName, Qt::black);
        curve->setPen(QPen(color, 0, Qt::NoPen)); // 无连线
        curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
            QBrush(color),
            QPen(color, 1),
            QSize(4, 4)));

        // 附加到图表
        curve->attach(m_plot);

        // 存储曲线对象
        m_fieldCurves[fieldIndex].append(curve);
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

    // 准备左侧刻度位置和标签
    m_currentLeftTickLabels.clear();
    m_currentRightTickLabels.clear();

    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = i * FIELD_SPACING;  // 每个点列的Y中心位置
        QPair<double, double> center = m_spotCenters[fieldIndex];

        // 左侧刻度标签 - 显示视场名称和原始中心坐标（加上偏移量）
        // 左侧Y坐标加上偏移量 LEFT_AXIS_OFFSET
        QString leftLabel = QString("%1\nX:%2\nY:%3")
            .arg(getFieldName(fieldIndex))
            .arg(center.first, 0, 'f', 4)
            .arg(center.second + LEFT_AXIS_OFFSET, 0, 'f', 4); // Y坐标加上偏移量
        m_currentLeftTickLabels[verticalOffset] = leftLabel;

        // 右侧刻度标签 - 显示RMS和100%直径
        if (fieldIndex < m_fieldRMS.size() && fieldIndex < m_field100.size()) {
            QString rightLabel = QString("%1\nRMS:%2\n100%:%3")
                .arg(getFieldName(fieldIndex))
                .arg(m_fieldRMS[fieldIndex] * m_dataScale, 0, 'f', 4)
                .arg(m_field100[fieldIndex] * m_dataScale, 0, 'f', 4);
            m_currentRightTickLabels[verticalOffset] = rightLabel;
        }
    }
}

void SpotDiagramPlotter::synchronizeAxisTicks()
{
    // 删除旧的刻度绘制器
    if (m_leftScaleDraw) {
        delete m_leftScaleDraw;
        m_leftScaleDraw = nullptr;
    }
    if (m_rightScaleDraw) {
        delete m_rightScaleDraw;
        m_rightScaleDraw = nullptr;
    }

    // 创建新的刻度绘制器
    m_leftScaleDraw = new MultiLineScaleDraw(m_currentLeftTickLabels);
    m_rightScaleDraw = new MultiLineScaleDraw(m_currentRightTickLabels);

    // 设置刻度绘制器
    m_plot->setAxisScaleDraw(QwtPlot::yLeft, m_leftScaleDraw);
    m_plot->setAxisScaleDraw(QwtPlot::yRight, m_rightScaleDraw);

    // 设置刻度标签样式
    m_plot->axisWidget(QwtPlot::yLeft)->setFont(QFont("Arial", 8));
    m_plot->axisWidget(QwtPlot::yRight)->setFont(QFont("Arial", 8));

    // 设置刻度对齐方式
    m_leftScaleDraw->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_rightScaleDraw->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 创建只包含点列中心位置的刻度划分
    QList<double> majorTicks;
    QList<double> minorTicks; // 空列表，不显示次要刻度

    // 只添加点列中心位置作为主刻度
    for (double tickPos : m_currentLeftTickLabels.keys()) {
        majorTicks.append(tickPos);
    }

    // 设置刻度划分
    double yMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    double yMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    QwtScaleDiv yScaleDiv(yMin, yMax, minorTicks, minorTicks, majorTicks);

    m_plot->setAxisScaleDiv(QwtPlot::yLeft, yScaleDiv);
    m_plot->setAxisScaleDiv(QwtPlot::yRight, yScaleDiv);
}

void SpotDiagramPlotter::reapplyScaleDraws()
{
    // 重新应用自定义刻度绘制
    if (m_leftScaleDraw && m_rightScaleDraw) {
        m_plot->setAxisScaleDraw(QwtPlot::yLeft, m_leftScaleDraw);
        m_plot->setAxisScaleDraw(QwtPlot::yRight, m_rightScaleDraw);
        m_plot->replot();
    }
}

void SpotDiagramPlotter::updateAxisTicks()
{
    // 获取当前Y轴范围
    double yMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    double yMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    // 创建只包含点列中心位置的刻度划分
    QList<double> majorTicks;
    QList<double> minorTicks; // 空列表，不显示次要刻度

    // 只添加在范围内的点列中心位置作为主刻度
    for (double tickPos : m_currentLeftTickLabels.keys()) {
        if (tickPos >= yMin && tickPos <= yMax) {
            majorTicks.append(tickPos);
        }
    }

    // 设置刻度划分
    QwtScaleDiv yScaleDiv(yMin, yMax, minorTicks, minorTicks, majorTicks);

    m_plot->setAxisScaleDiv(QwtPlot::yLeft, yScaleDiv);
    m_plot->setAxisScaleDiv(QwtPlot::yRight, yScaleDiv);

    // 重新应用自定义刻度绘制
    reapplyScaleDraws();
}

void SpotDiagramPlotter::handleZoomed(const QRectF&)
{
    // 缩放完成后更新刻度显示
    updateLabelsForCurrentView();
}

void SpotDiagramPlotter::handlePanned(int dx, int dy)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);

    // 平移完成后更新刻度显示
    updateLabelsForCurrentView();
}

void SpotDiagramPlotter::updateTickLabelPositions()
{
    // 获取当前Y轴范围
    double currentYMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    double currentYMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    int maxFields = qMin(3, fieldIndices.size());

    // 计算当前Y轴的中点位置，用于确定哪些标签应该显示
    double currentYCenter = (currentYMin + currentYMax) / 2.0;

    // 更新标签位置
    QMap<double, QString> newLeftTickLabels;
    QMap<double, QString> newRightTickLabels;

    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];

        // 原始中心位置
        double originalCenter = m_originalFieldCenters[fieldIndex];

        // 计算当前视场中心应该显示的位置
        // 如果视场中心在当前可见范围内，就显示该标签
        if (originalCenter >= currentYMin && originalCenter <= currentYMax) {
            QPair<double, double> center = m_spotCenters[fieldIndex];

            // 左侧标签
            QString leftLabel = QString("%1\nX:%2\nY:%3")
                .arg(getFieldName(fieldIndex))
                .arg(center.first, 0, 'f', 4)
                .arg(center.second + LEFT_AXIS_OFFSET, 0, 'f', 4);
            newLeftTickLabels[originalCenter] = leftLabel;

            // 右侧标签
            if (fieldIndex < m_fieldRMS.size() && fieldIndex < m_field100.size()) {
                QString rightLabel = QString("%1\nRMS:%2\n100%:%3")
                    .arg(getFieldName(fieldIndex))
                    .arg(m_fieldRMS[fieldIndex] * m_dataScale, 0, 'f', 4)
                    .arg(m_field100[fieldIndex] * m_dataScale, 0, 'f', 4);
                newRightTickLabels[originalCenter] = rightLabel;
            }
        }
    }

    m_currentLeftTickLabels = newLeftTickLabels;
    m_currentRightTickLabels = newRightTickLabels;
}

void SpotDiagramPlotter::updateLabelsForCurrentView()
{
    // 更新标签位置
    updateTickLabelPositions();

    // 更新刻度显示
    updateAxisTicks();

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::updateSpotPositions()
{
    // 获取当前Y轴范围
    double currentYMin = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
    double currentYMax = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
    double currentYRange = currentYMax - currentYMin;

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldCurves.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    int maxFields = qMin(3, fieldIndices.size());

    // 计算原始的Y轴总范围
    double singleSpotYRange = m_maxSpotYRange * m_dataScale;
    double originalYMin = -singleSpotYRange;
    double originalYMax = (maxFields - 1) * FIELD_SPACING + singleSpotYRange;
    double originalYRange = originalYMax - originalYMin;

    // 计算缩放因子
    double yScaleFactor = currentYRange / originalYRange;

    // 计算垂直偏移（考虑当前缩放因子）
    QMap<int, double> verticalOffsets;
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double originalOffset = i * FIELD_SPACING;
        verticalOffsets[fieldIndex] = currentYMin + (originalOffset - originalYMin) * yScaleFactor;
    }

    // 更新每个视场的点列位置
    for (auto it = m_fieldCurves.begin(); it != m_fieldCurves.end(); ++it) {
        int fieldIndex = it.key();
        double verticalOffset = verticalOffsets[fieldIndex];

        // 获取该视场的原始数据
        const QVector<SpotData>& fieldSpotData = m_fieldData[fieldIndex];

        // 计算该视场的中心
        double centerX, centerY;
        calculateSpotCenter(fieldSpotData, centerX, centerY);

        // 重新计算该视场下所有曲线的数据
        for (QwtPlotCurve* curve : it.value()) {
            // 按颜色重新分组数据
            QMap<QString, QVector<QPointF>> colorData;

            for (const SpotData& spot : fieldSpotData) {
                QPointF point;
                // 点列居中：将每个点减去其视场中心
                point.setX((spot.x - centerX) * m_dataScale); // X坐标减去中心并扩大
                point.setY((spot.y - centerY) * m_dataScale + verticalOffset); // Y坐标减去中心并扩大，然后添加新的垂直偏移

                // 根据曲线名称判断颜色
                if (curve->title().text().contains(spot.colorName)) {
                    colorData[spot.colorName].append(point);
                }
            }

            // 更新曲线数据
            for (auto colorIt = colorData.begin(); colorIt != colorData.end(); ++colorIt) {
                QPolygonF polygon;
                for (const QPointF& point : colorIt.value()) {
                    polygon.append(point);
                }
                curve->setSamples(polygon);
            }
        }
    }

    // 更新标签位置
    updateTickLabelPositions();

    // 更新刻度显示
    updateAxisTicks();
}

bool SpotDiagramPlotter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_plot->canvas()) {
        // 处理鼠标释放事件，确保平移完成后更新标签
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                // 左键释放，可能是平移结束
                QTimer::singleShot(50, this, &SpotDiagramPlotter::updateLabelsForCurrentView);
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SpotDiagramPlotter::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    QAction* resetAction = menu.addAction("重置视图");

    QAction* selectedAction = menu.exec(event->globalPos());

    if (selectedAction == resetAction) {
        resetView();
    }
}

void SpotDiagramPlotter::resetView()
{
    // 重置到初始视图
    restoreInitialView();

    // 重新应用自定义刻度绘制
    reapplyScaleDraws();
}

void SpotDiagramPlotter::saveInitialView()
{
    // 保存初始视图范围
    QwtScaleDiv xScaleDiv = m_plot->axisScaleDiv(QwtPlot::xBottom);
    QwtScaleDiv yScaleDiv = m_plot->axisScaleDiv(QwtPlot::yLeft);

    m_initialXMin = xScaleDiv.lowerBound();
    m_initialXMax = xScaleDiv.upperBound();
    m_initialYMin = yScaleDiv.lowerBound();
    m_initialYMax = yScaleDiv.upperBound();
}

void SpotDiagramPlotter::restoreInitialView()
{
    // 恢复初始视图范围
    m_plot->setAxisScale(QwtPlot::xBottom, m_initialXMin, m_initialXMax);
    m_plot->setAxisScale(QwtPlot::yLeft, m_initialYMin, m_initialYMax);
    m_plot->setAxisScale(QwtPlot::yRight, m_initialYMin, m_initialYMax);

    // 更新点列位置
    updateSpotPositions();

    // 重新绘制
    m_plot->replot();
}