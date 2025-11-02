#include "SpotDiagramWidget.h"
#include <QDebug>
#include <cmath>

SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
    : QWidget(parent), m_dataScale(20.0), m_isInitialPlot(true),
    m_rmsInfoText(nullptr), m_centerInfoText(nullptr),
    m_selectionRect(nullptr), m_contextMenu(nullptr),
    m_resetViewAction(nullptr), m_zoomToSelectionAction(nullptr),
    m_isSelecting(false)
{
    // 初始化颜色映射
    m_colorMap["RGBOrange"] = QColor(255, 165, 0);
    m_colorMap["RGBYellow"] = QColor(255, 255, 0);
    m_colorMap["RGBGreen"] = QColor(0, 255, 0);
    m_colorMap["RGBCyan"] = QColor(0, 255, 255);

    // 创建界面
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_plot = new QCustomPlot(this);
    mainLayout->addWidget(m_plot);

    // 创建右键菜单
    m_contextMenu = new QMenu(this);
    m_resetViewAction = new QAction("重置视图", this);
    m_zoomToSelectionAction = new QAction("放大到选择区域", this);

    m_contextMenu->addAction(m_zoomToSelectionAction);
    m_contextMenu->addAction(m_resetViewAction);

    connect(m_resetViewAction, &QAction::triggered, this, &SpotDiagramPlotter::resetView);
    connect(m_zoomToSelectionAction, &QAction::triggered, this, &SpotDiagramPlotter::zoomToSelection);

    setupPlot();
    resize(QSize(640, 480));
}

bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    m_spotData.clear();
    m_fieldRMS.clear();
    m_field100.clear();
    m_airyX.clear();
    m_airyY.clear();
    m_spotCenters.clear();
    m_fieldGraphs.clear();
    m_fieldLabels.clear();
    m_fieldData.clear();

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
    // 启用上部和右侧坐标轴
    m_plot->xAxis2->setVisible(true);
    m_plot->yAxis2->setVisible(true);

    // 设置图表基本属性
    m_plot->xAxis->setLabel("X (mm)");
    m_plot->yAxis->setLabel("Field Position");

    // 设置上部和右侧坐标轴的标签
    m_plot->xAxis2->setLabel("X (mm)");
    m_plot->yAxis2->setLabel("RMS Diameter");

    // 连接坐标轴范围同步
    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis2, SLOT(setRange(QCPRange)));

    // 连接坐标轴范围变化信号
    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onRangeChanged()));
    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onRangeChanged()));

    // 禁用坐标轴平移，只允许缩放
    m_plot->setInteractions(QCP::iRangeZoom);

    // 连接鼠标事件
    connect(m_plot, &QCustomPlot::mousePress, this, &SpotDiagramPlotter::onMousePress);
    connect(m_plot, &QCustomPlot::mouseMove, this, &SpotDiagramPlotter::onMouseMove);
    connect(m_plot, &QCustomPlot::mouseRelease, this, &SpotDiagramPlotter::onMouseRelease);

    // 设置标题
    m_plot->plotLayout()->insertRow(0);
    m_plot->plotLayout()->addElement(0, 0,
        new QCPTextElement(m_plot, QStringLiteral("光学系统点列图分析"), QFont("sans", 14, QFont::Bold)));
}

void SpotDiagramPlotter::plotSpotDiagrams()
{
    // 清除现有图形
    m_plot->clearPlottables();
    m_plot->clearItems();
    m_fieldGraphs.clear();
    m_fieldLabels.clear();

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
    double xRange = qMax(fabs(xMin), fabs(xMax)) * 1.2 * m_dataScale; // 增加20%边距并扩大

    // 设置Y轴范围 - Y轴范围是X轴范围的3倍
    double yRange = xRange * 3.0;
    double yMin = -yRange / 2;
    double yMax = yRange / 2;

    // 计算垂直偏移
    QMap<int, double> verticalOffsets = calculateVerticalOffsets();

    // 存储点列中心坐标
    m_spotCenters.clear();

    // 为每个视场创建图形
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        QString fieldName = QString("F%1").arg(fieldIndex + 1);

        // 获取垂直偏移
        double verticalOffset = verticalOffsets[fieldIndex];

        // 计算点列中心
        double centerX, centerY;
        calculateSpotCenter(m_fieldData[fieldIndex], centerX, centerY);
        m_spotCenters[fieldIndex] = qMakePair(centerX * m_dataScale, centerY * m_dataScale); // 中心坐标扩大

        // 添加点数据到图表（数据扩大）
        addSpotDataToPlot(m_fieldData[fieldIndex], verticalOffset, fieldName, fieldIndex);
    }

    // 设置坐标轴范围
    m_plot->xAxis->setRange(-xRange, xRange);
    m_plot->yAxis->setRange(yMin, yMax);

    // 保存初始视图范围
    saveInitialView();

    // 添加RMS和100%直径信息到图例区域（值扩大）
    addRMSInfoToPlot();

    // 添加中心坐标信息到图表左侧
    addCenterInfoToPlot(m_spotCenters);

    // 标记初始绘制完成
    m_isInitialPlot = false;

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
    const QString& fieldName, int fieldIndex)
{
    if (data.isEmpty()) return;

    // 按颜色分组数据
    QMap<QString, QVector<QCPGraphData>> colorData;

    for (const SpotData& spot : data) {
        QCPGraphData point;
        point.key = spot.x * m_dataScale; // X坐标扩大
        point.value = spot.y * m_dataScale + verticalOffset; // Y坐标扩大并添加垂直偏移
        colorData[spot.colorName].append(point);
    }

    // 为每种颜色创建图形
    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
        const QString& colorName = it.key();
        const QVector<QCPGraphData>& points = it.value();

        QCPGraph* graph = m_plot->addGraph();
        graph->setName(fieldName + " - " + colorName);

        // 设置颜色和样式
        QColor color = m_colorMap.value(colorName, Qt::black);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, color, 4));
        graph->setLineStyle(QCPGraph::lsNone);

        // 添加数据
        graph->data()->set(points);

        // 存储图形对象
        m_fieldGraphs[fieldIndex].append(graph);
    }

    // 添加视场标签
    QCPItemText* fieldLabel = new QCPItemText(m_plot);
    fieldLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    fieldLabel->position->setType(QCPItemPosition::ptAxisRectRatio);

    // 计算标签在Y轴上的相对位置
    double yMin = m_plot->yAxis->range().lower;
    double yMax = m_plot->yAxis->range().upper;
    double yRange = yMax - yMin;
    double relativeY = (verticalOffset - yMin) / yRange;

    fieldLabel->position->setCoords(0.02, relativeY);
    fieldLabel->setText(fieldName);
    fieldLabel->setFont(QFont("Arial", 9, QFont::Bold));
    fieldLabel->setColor(Qt::darkBlue);

    // 存储标签对象
    m_fieldLabels[fieldIndex] = fieldLabel;
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

void SpotDiagramPlotter::addRMSInfoToPlot()
{
    // 如果已存在RMS信息文本，先删除
    if (m_rmsInfoText) {
        m_plot->removeItem(m_rmsInfoText);
        delete m_rmsInfoText;
        m_rmsInfoText = nullptr;
    }

    // 添加RMS和100%直径信息到图表右上角（值扩大）
    m_rmsInfoText = new QCPItemText(m_plot);
    m_rmsInfoText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
    m_rmsInfoText->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_rmsInfoText->position->setCoords(0.98, 0.02);

    QString infoStr = "RMS / 100% Diameter:\n";
    for (int i = 0; i < m_fieldRMS.size() && i < m_field100.size(); i++) {
        infoStr += QString("F%1: %2 / %3\n")
            .arg(i + 1)
            .arg(m_fieldRMS[i] * m_dataScale, 0, 'f', 6) // RMS值扩大
            .arg(m_field100[i] * m_dataScale, 0, 'f', 6); // 100%直径值扩大
    }

    m_rmsInfoText->setText(infoStr);
    m_rmsInfoText->setFont(QFont("Arial", 9));
    m_rmsInfoText->setColor(Qt::darkBlue);
}

void SpotDiagramPlotter::addCenterInfoToPlot(const QMap<int, QPair<double, double>>& spotCenters)
{
    // 如果已存在中心坐标信息文本，先删除
    if (m_centerInfoText) {
        m_plot->removeItem(m_centerInfoText);
        delete m_centerInfoText;
        m_centerInfoText = nullptr;
    }

    // 添加中心坐标信息到图表左侧
    m_centerInfoText = new QCPItemText(m_plot);
    m_centerInfoText->setPositionAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_centerInfoText->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_centerInfoText->position->setCoords(0.02, 0.02);

    QString centerInfoStr = "Spot Centers (X, Y):\n";
    QList<int> fieldIndices = spotCenters.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    for (int i = 0; i < fieldIndices.size(); i++) {
        int fieldIndex = fieldIndices[i];
        QPair<double, double> center = spotCenters[fieldIndex];
        centerInfoStr += QString("F%1: (%2, %3)\n")
            .arg(fieldIndex + 1)
            .arg(center.first, 0, 'f', 6)
            .arg(center.second, 0, 'f', 6);
    }

    m_centerInfoText->setText(centerInfoStr);
    m_centerInfoText->setFont(QFont("Arial", 9));
    m_centerInfoText->setColor(Qt::darkRed);
}

void SpotDiagramPlotter::onRangeChanged()
{
    // 当坐标轴范围变化时，更新点列位置
    if (!m_isInitialPlot) {
        updateSpotPositions();
    }
}

void SpotDiagramPlotter::updateSpotPositions()
{
    // 计算新的垂直偏移
    QMap<int, double> verticalOffsets = calculateVerticalOffsets();

    // 更新每个视场的点列位置
    for (auto it = m_fieldGraphs.begin(); it != m_fieldGraphs.end(); ++it) {
        int fieldIndex = it.key();
        double verticalOffset = verticalOffsets[fieldIndex];

        // 获取该视场的原始数据
        const QVector<SpotData>& fieldSpotData = m_fieldData[fieldIndex];

        // 重新计算该视场下所有图形的数据
        for (QCPGraph* graph : it.value()) {
            // 按颜色重新分组数据
            QMap<QString, QVector<QCPGraphData>> colorData;

            for (const SpotData& spot : fieldSpotData) {
                QCPGraphData point;
                point.key = spot.x * m_dataScale; // X坐标扩大
                point.value = spot.y * m_dataScale + verticalOffset; // Y坐标扩大并添加新的垂直偏移

                // 根据颜色分组
                if (graph->name().contains(spot.colorName)) {
                    colorData[spot.colorName].append(point);
                }
            }

            // 更新图形数据
            for (auto colorIt = colorData.begin(); colorIt != colorData.end(); ++colorIt) {
                graph->data()->set(colorIt.value());
            }
        }

        // 更新视场标签位置
        if (m_fieldLabels.contains(fieldIndex)) {
            QCPItemText* fieldLabel = m_fieldLabels[fieldIndex];

            // 计算标签在Y轴上的相对位置
            double yMin = m_plot->yAxis->range().lower;
            double yMax = m_plot->yAxis->range().upper;
            double yRange = yMax - yMin;
            double relativeY = (verticalOffset - yMin) / yRange;

            fieldLabel->position->setCoords(0.02, relativeY);
        }
    }

    // 重新绘制
    m_plot->replot();
}

QMap<int, double> SpotDiagramPlotter::calculateVerticalOffsets()
{
    QMap<int, double> verticalOffsets;

    // 获取当前Y轴范围
    double yMin = m_plot->yAxis->range().lower;
    double yMax = m_plot->yAxis->range().upper;
    double yRange = yMax - yMin;

    // 获取视场索引并排序
    QList<int> fieldIndices = m_fieldGraphs.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    int maxFields = fieldIndices.size();

    if (maxFields == 0) return verticalOffsets;

    // 专门为三个点列设计的分布算法
    if (maxFields == 3) {
        // 将Y轴范围分成4等份，三个点列分别位于1/4、2/4和3/4的位置
        double segmentHeight = yRange / 4.0;
        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;           // F1 - 底部第一个点列
        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;       // F2 - 中间点列
        verticalOffsets[fieldIndices[2]] = yMin + 3 * segmentHeight;       // F3 - 顶部点列
    }
    else if (maxFields == 2) {
        // 两个点列的情况：分别位于1/3和2/3的位置
        double segmentHeight = yRange / 3.0;
        verticalOffsets[fieldIndices[0]] = yMin + segmentHeight;           // F1
        verticalOffsets[fieldIndices[1]] = yMin + 2 * segmentHeight;       // F2
    }
    else if (maxFields == 1) {
        // 单个点列位于中心
        verticalOffsets[fieldIndices[0]] = (yMin + yMax) / 2.0;            // F1
    }

    return verticalOffsets;
}

void SpotDiagramPlotter::contextMenuEvent(QContextMenuEvent* event)
{
    // 显示右键菜单
    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos());
    }
}

void SpotDiagramPlotter::onMousePress(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        // 开始选择矩形
        m_isSelecting = true;
        m_selectionStart = event->pos();
        m_selectionEnd = event->pos();

        // 创建选择矩形
        if (!m_selectionRect) {
            m_selectionRect = new QCPItemRect(m_plot);
            m_selectionRect->setPen(QPen(Qt::blue, 1, Qt::DashLine));
            m_selectionRect->setBrush(QBrush(QColor(0, 0, 255, 50)));
        }

        drawSelectionRect(QRect(m_selectionStart, m_selectionEnd));
    }
}

void SpotDiagramPlotter::onMouseMove(QMouseEvent* event)
{
    if (m_isSelecting && event->buttons() & Qt::RightButton) {
        // 更新选择矩形
        m_selectionEnd = event->pos();
        drawSelectionRect(QRect(m_selectionStart, m_selectionEnd));
        m_plot->replot();
    }
}

void SpotDiagramPlotter::onMouseRelease(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton && m_isSelecting) {
        m_isSelecting = false;

        // 移除选择矩形
        if (m_selectionRect) {
            m_plot->removeItem(m_selectionRect);
            //delete m_selectionRect;
            m_selectionRect = nullptr;
        }

        m_plot->replot();
    }
}

void SpotDiagramPlotter::drawSelectionRect(const QRect& rect)
{
    if (!m_selectionRect) return;

    // 将屏幕坐标转换为图表坐标
    double x1, y1, x2, y2;

    x1 = m_plot->xAxis->pixelToCoord(rect.left()/*, x1*/);
    x2 = m_plot->xAxis->pixelToCoord(rect.right());
    y1 = m_plot->yAxis->pixelToCoord(rect.top());
    y2 = m_plot->yAxis->pixelToCoord(rect.bottom());

    // 设置矩形位置
    m_selectionRect->topLeft->setCoords(x1, y1);
    m_selectionRect->bottomRight->setCoords(x2, y2);
}

void SpotDiagramPlotter::resetView()
{
    // 重置到初始视图
    restoreInitialView();
}

void SpotDiagramPlotter::zoomToSelection()
{
    if (!m_isSelecting || m_selectionStart == m_selectionEnd) return;

    // 计算选择区域
    QRect selectionRect = QRect(m_selectionStart, m_selectionEnd).normalized();

    // 将屏幕坐标转换为图表坐标
    double x1, y1, x2, y2;
    x1 = m_plot->xAxis->pixelToCoord(selectionRect.left()/*, x1*/);
    x2 = m_plot->xAxis->pixelToCoord(selectionRect.right());
    y1 = m_plot->yAxis->pixelToCoord(selectionRect.top());
    y2 = m_plot->yAxis->pixelToCoord(selectionRect.bottom());

    // 设置新的坐标轴范围
    m_plot->xAxis->setRange(x1, x2);
    m_plot->yAxis->setRange(y1, y2);

    // 更新点列位置
    updateSpotPositions();

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::saveInitialView()
{
    // 保存初始视图范围
    m_initialXRange = m_plot->xAxis->range();
    m_initialYRange = m_plot->yAxis->range();
}

void SpotDiagramPlotter::restoreInitialView()
{
    // 恢复初始视图范围
    m_plot->xAxis->setRange(m_initialXRange);
    m_plot->yAxis->setRange(m_initialYRange);

    // 更新点列位置
    updateSpotPositions();

    // 重新绘制
    m_plot->replot();
}