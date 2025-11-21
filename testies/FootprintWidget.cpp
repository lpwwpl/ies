#include "FootprintWidget.h"
#include <QDebug>
#include <QRegularExpression>

FootprintWidget::FootprintWidget(QWidget* parent)
    : QWidget(parent)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建QCustomPlot实例
    m_customPlot = new QCustomPlot(this);
    mainLayout->addWidget(m_customPlot);

    // 设置图表
    setupPlot();

    resize(QSize(640, 640));
}

void FootprintWidget::setupPlot()
{
    // 设置图表标题
    m_customPlot->plotLayout()->insertRow(0);
    m_customPlot->plotLayout()->addElement(0, 0,
        new QCPTextElement(m_customPlot, "Footprint Limits on Surface 1", QFont("sans", 12, QFont::Bold)));

    // 设置坐标轴标签
    m_customPlot->xAxis->setLabel("X Position");
    m_customPlot->yAxis->setLabel("Y Position");

    // 设置坐标轴范围（根据数据调整）
    m_customPlot->xAxis->setRange(-10, 10);
    m_customPlot->yAxis->setRange(-85, 10);

    // 启用选择、缩放、拖动等功能
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // 显示图例
    m_customPlot->legend->setVisible(true);
    m_customPlot->legend->setFont(QFont("Helvetica", 9));
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
}

bool FootprintWidget::loadDataFromFile(const QString& filename)
{
    m_footprintData.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file:" << filename;
        return false;
    }

    QTextStream in(&file);
    QString line;
    FootprintData currentData;
    bool readingData = false;

    QRegularExpression fieldRegex("Field\\s+(\\w+)");
    QRegularExpression leftRegex("LEFT\\s+([-\\d.]+)");
    QRegularExpression rightRegex("RIGHT\\s+([-\\d.]+)");
    QRegularExpression topRegex("TOP\\s+([-\\d.]+)");
    QRegularExpression bottomRegex("BOTTOM\\s+([-\\d.]+)");
    QRegularExpression radiusRegex("RADIUS\\s+([-\\d.]+)");

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("Footprint limits on surface")) {
            readingData = true;
            currentData = FootprintData();
            continue;
        }

        if (readingData) {
            QRegularExpressionMatch match;

            if ((match = fieldRegex.match(line)).hasMatch()) {
                currentData.field = match.captured(1);
            }
            else if ((match = leftRegex.match(line)).hasMatch()) {
                currentData.left = match.captured(1).toDouble();
            }
            else if ((match = rightRegex.match(line)).hasMatch()) {
                currentData.right = match.captured(1).toDouble();
            }
            else if ((match = topRegex.match(line)).hasMatch()) {
                currentData.top = match.captured(1).toDouble();
            }
            else if ((match = bottomRegex.match(line)).hasMatch()) {
                currentData.bottom = match.captured(1).toDouble();
            }
            else if ((match = radiusRegex.match(line)).hasMatch()) {
                currentData.radius = match.captured(1).toDouble();

                // 完成一个数据块的读取
                if (!currentData.field.isEmpty()) {
                    m_footprintData.append(currentData);
                }
                readingData = false;
            }
        }
    }

    file.close();

    qDebug() << "Loaded" << m_footprintData.size() << "footprint records";
    for (const auto& data : m_footprintData) {
        qDebug() << "Field:" << data.field
            << "Left:" << data.left << "Right:" << data.right
            << "Top:" << data.top << "Bottom:" << data.bottom
            << "Radius:" << data.radius;
    }

    return !m_footprintData.isEmpty();
}

QColor FootprintWidget::getFieldColor(const QString& field)
{
    static QMap<QString, QColor> colorMap = {
        {"0", Qt::red},
        {"1", Qt::green},
        {"2", Qt::blue},
        {"3", Qt::magenta},
        {"all", Qt::darkYellow}
    };

    return colorMap.value(field, Qt::gray);
}

void FootprintWidget::plotFootprints()
{
    m_customPlot->clearPlottables();
    m_customPlot->clearItems();

    for (const FootprintData& data : m_footprintData) {
        QColor color = getFieldColor(data.field);

        // 创建矩形来表示footprint区域
        //QCPItemRect* rect = new QCPItemRect(m_customPlot);

        //// 设置矩形位置
        //rect->topLeft->setCoords(data.left, data.top);
        //rect->bottomRight->setCoords(data.right, data.bottom);

        //// 设置矩形样式
        //rect->setPen(QPen(color, 2));
        QColor fillColor = color;
        fillColor.setAlpha(50); // 半透明填充
        //rect->setBrush(QBrush(fillColor));

        // 添加半径圆（可选）
        QCPItemEllipse* ellipse = new QCPItemEllipse(m_customPlot);
        double centerX = (data.left + data.right) / 2.0;
        double centerY = (data.top + data.bottom) / 2.0;

        double x_half_range = (data.right - data.left) / 2;
        double y_half_range = (data.top - data.bottom) / 2;

        double radius = 0;
        if (x_half_range > radius)radius = x_half_range;
        if (y_half_range > radius)radius = y_half_range;
        if (data.field == "all")
        {
            centerX = 0;
            centerY = 0;
            radius = data.radius;
        }

        ellipse->topLeft->setCoords(centerX - radius, centerY + radius);
        ellipse->bottomRight->setCoords(centerX + radius, centerY - radius);
        ellipse->setPen(QPen(color, 1, Qt::DashLine));
        
        // 创建图例项（使用一个不可见的曲线来创建图例）
        QCPGraph* legendGraph = m_customPlot->addGraph();
        legendGraph->setName(QString("Field %1").arg(data.field));
        legendGraph->setPen(QPen(color, 2));
        legendGraph->setBrush(QBrush(fillColor));
        legendGraph->setLineStyle(QCPGraph::lsNone);
        legendGraph->setScatterStyle(QCPScatterStyle::ssNone);
    }
    double range = -INFINITY;
    // 重新调整坐标轴范围以适应所有数据
    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (const FootprintData& data : m_footprintData) {
        minX = qMin(minX, data.left);
        maxX = qMax(maxX, data.right);
        minY = qMin(minY, data.bottom);
        maxY = qMax(maxY, data.top);
    }
    if (fabs(minX) > range)range = fabs(minX);
    if (fabs(maxX) > range)range = fabs(maxX);
    if (fabs(minY) > range)range = fabs(minY);
    if (fabs(maxY) > range)range = fabs(maxY);
    // 添加一些边距
    double marginX = (range - minX) * 0.1;
    double marginY = (range - minY) * 0.1;
    m_customPlot->xAxis->setRange(-range - marginX, range + marginX);
    m_customPlot->yAxis->setRange(-range - marginY, range + marginY);
    m_customPlot->rescaleAxes();
    m_customPlot->replot();
}