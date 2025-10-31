#include "SpotDiagramWidget.h"
#include <QDebug>
#include <cmath>

SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
    : QWidget(parent)
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

    setupPlot();
    resize(QSize(640, 480));
}

bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "无法打开文件:" << filename;
        return false;
    }

    m_spotData.clear();
    QTextStream in(&file);
    bool dataSection = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("INDEX")) {
            dataSection = true;
            continue;
        }

        if (line.startsWith("#") || line.isEmpty()) {
            continue;
        }

        if (dataSection) {
            parseDataLine(line);
        }
    }

    file.close();
    //qDebug() << "成功加载" << m_spotData.size() << "个数据点";
    return true;
}

void SpotDiagramPlotter::parseDataLine(const QString& line)
{
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
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
    m_plot->xAxis->setLabel("X(mm)");
    m_plot->yAxis->setLabel("postion");

    // 设置上部和右侧坐标轴的标签
    m_plot->xAxis2->setLabel("X(mm)");
    m_plot->yAxis2->setLabel("RMS");

    // 连接坐标轴范围，使上部和下部、左侧和右侧同步
    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis2, SLOT(setRange(QCPRange)));

    // 启用选择缩放和拖动
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

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

    // 按视场索引分组
    QMap<int, QVector<SpotData>> fieldData;
    for (const SpotData& data : m_spotData) {
        fieldData[data.fieldIndex].append(data);
    }

    // 为每个视场创建图形，垂直分布
    QList<int> fieldIndices = fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    // 只取前3个视场
    int maxFields = qMin(3, fieldIndices.size());

    // 设置X轴范围
    double xRange = 0.025; // X轴范围

    // 设置Y轴范围 - 增加范围以提供更多空间
    double yRange = xRange * 6.0; // 进一步增加Y轴范围

    // 计算垂直偏移 - 使点列图在Y轴上均匀分布且居中
    QVector<double> verticalOffsets;
    if (maxFields == 1) {
        verticalOffsets << 0;
    }
    else if (maxFields == 2) {
        verticalOffsets << yRange / 4 << -yRange / 4;
    }
    else { // maxFields == 3
        verticalOffsets << yRange / 3 << 0 << -yRange / 3;
    }

    // 创建自定义的Y轴刻度标签
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    // 存储每个视场的RMS值
    QMap<int, double> fieldRMS;

    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = verticalOffsets[i];
        QString fieldName = QString("%1").arg(fieldIndex);

        // 添加Y轴刻度标签
        textTicker->addTick(verticalOffset, fieldName);

        // 计算RMS并存储
        double rmsX, rmsY, rmsRadius;
        calculateRMS(fieldData[fieldIndex], rmsX, rmsY, rmsRadius);
        fieldRMS[fieldIndex] = rmsRadius;

        addSpotDataToPlot(fieldData[fieldIndex], verticalOffset, fieldName);
    }

    // 设置Y轴使用文本刻度
    m_plot->yAxis->setTicker(textTicker);

    // 为右侧Y轴创建RMS刻度标签
    QSharedPointer<QCPAxisTickerText> rmsTicker(new QCPAxisTickerText);
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = verticalOffsets[i];
        QString rmsLabel = QString("%1").arg(fieldRMS[fieldIndex], 0, 'f', 6);
        rmsTicker->addTick(verticalOffset, rmsLabel);
    }
    m_plot->yAxis2->setTicker(rmsTicker);

    // 设置坐标轴范围 - 使点列图在Y轴上居中显示
    m_plot->xAxis->setRange(-xRange, xRange);
    m_plot->yAxis->setRange(-yRange / 2, yRange / 2);

    // 添加图例
    //m_plot->legend->setVisible(true);
    //m_plot->legend->setFont(QFont("Arial", 9));
    //m_plot->legend->setRowSpacing(-3);

    // 重新绘制
    m_plot->replot();
}

void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset, const QString& fieldName)
{
    if (data.isEmpty()) return;

    // 按颜色分组数据
    QMap<QString, QVector<QCPGraphData>> colorData;

    for (const SpotData& spot : data) {
        QCPGraphData point;
        point.key = spot.x;
        point.value = spot.y + verticalOffset; // 添加垂直偏移
        colorData[spot.colorName].append(point);
    }

    // 为每种颜色创建图形
    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
        const QString& colorName = it.key();
        const QVector<QCPGraphData>& points = it.value();

        QCPGraph* graph = m_plot->addGraph();
        graph->setName(colorName);

        // 设置颜色和样式
        QColor color = m_colorMap.value(colorName, Qt::black);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, color, 4));
        graph->setLineStyle(QCPGraph::lsNone);
        //graph->setSelectable(false);

        // 添加数据
        graph->data()->set(points);
    }

    // 计算并绘制RMS圆
    double rmsX, rmsY, rmsRadius;
    calculateRMS(data, rmsX, rmsY, rmsRadius);
    m_rmsValues[data.first().fieldIndex] = rmsRadius;

    // 创建RMS圆
    QCPItemEllipse* rmsCircle = new QCPItemEllipse(m_plot);
    rmsCircle->topLeft->setCoords(-rmsRadius, verticalOffset + rmsRadius);
    rmsCircle->bottomRight->setCoords(rmsRadius, verticalOffset - rmsRadius);
    rmsCircle->setPen(QPen(Qt::blue, 1, Qt::DashLine));
    rmsCircle->setSelectable(false);

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
}

void SpotDiagramPlotter::calculateRMS(const QVector<SpotData>& data, double& rmsX, double& rmsY, double& rmsRadius)
{
    if (data.isEmpty()) {
        rmsX = rmsY = rmsRadius = 0;
        return;
    }

    // 计算平均值
    double sumX = 0, sumY = 0;
    for (const SpotData& spot : data) {
        sumX += spot.x;
        sumY += spot.y;
    }
    double meanX = sumX / data.size();
    double meanY = sumY / data.size();

    // 计算RMS
    double sumVarX = 0, sumVarY = 0;
    for (const SpotData& spot : data) {
        sumVarX += (spot.x - meanX) * (spot.x - meanX);
        sumVarY += (spot.y - meanY) * (spot.y - meanY);
    }

    rmsX = std::sqrt(sumVarX / data.size());
    rmsY = std::sqrt(sumVarY / data.size());
    rmsRadius = std::sqrt(rmsX * rmsX + rmsY * rmsY);
}