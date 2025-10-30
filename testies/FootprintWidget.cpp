#include "FootprintWidget.h"
#include <QDebug>
#include <QRegularExpression>

FootprintWidget::FootprintWidget(QWidget* parent)
    : QWidget(parent)
{
    // ����������
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // ����QCustomPlotʵ��
    m_customPlot = new QCustomPlot(this);
    mainLayout->addWidget(m_customPlot);

    // ����ͼ��
    setupPlot();
}

void FootprintWidget::setupPlot()
{
    // ����ͼ�����
    m_customPlot->plotLayout()->insertRow(0);
    m_customPlot->plotLayout()->addElement(0, 0,
        new QCPTextElement(m_customPlot, "Footprint Limits on Surface 1", QFont("sans", 12, QFont::Bold)));

    // �����������ǩ
    m_customPlot->xAxis->setLabel("X Position");
    m_customPlot->yAxis->setLabel("Y Position");

    // ���������᷶Χ���������ݵ�����
    m_customPlot->xAxis->setRange(-10, 10);
    m_customPlot->yAxis->setRange(-85, 10);

    // ����ѡ�����š��϶��ȹ���
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // ��ʾͼ��
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

                // ���һ�����ݿ�Ķ�ȡ
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
        {"0", Qt::blue},
        {"1", Qt::red},
        {"2", Qt::green},
        {"3", Qt::magenta},
        {"all", Qt::black}
    };

    return colorMap.value(field, Qt::gray);
}

void FootprintWidget::plotFootprints()
{
    m_customPlot->clearPlottables();
    m_customPlot->clearItems();

    for (const FootprintData& data : m_footprintData) {
        QColor color = getFieldColor(data.field);

        // ������������ʾfootprint����
        QCPItemRect* rect = new QCPItemRect(m_customPlot);

        // ���þ���λ��
        rect->topLeft->setCoords(data.left, data.top);
        rect->bottomRight->setCoords(data.right, data.bottom);

        // ���þ�����ʽ
        rect->setPen(QPen(color, 2));
        QColor fillColor = color;
        fillColor.setAlpha(50); // ��͸�����
        rect->setBrush(QBrush(fillColor));

        // ��Ӱ뾶Բ����ѡ��
        QCPItemEllipse* ellipse = new QCPItemEllipse(m_customPlot);
        double centerX = (data.left + data.right) / 2.0;
        double centerY = (data.top + data.bottom) / 2.0;
        ellipse->topLeft->setCoords(centerX - data.radius, centerY + data.radius);
        ellipse->bottomRight->setCoords(centerX + data.radius, centerY - data.radius);
        ellipse->setPen(QPen(color, 1, Qt::DashLine));

        // ����ͼ���ʹ��һ�����ɼ�������������ͼ����
        QCPGraph* legendGraph = m_customPlot->addGraph();
        legendGraph->setName(QString("Field %1").arg(data.field));
        legendGraph->setPen(QPen(color, 2));
        legendGraph->setBrush(QBrush(fillColor));
        legendGraph->setLineStyle(QCPGraph::lsNone);
        legendGraph->setScatterStyle(QCPScatterStyle::ssNone);
    }

    // ���µ��������᷶Χ����Ӧ��������
    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (const FootprintData& data : m_footprintData) {
        minX = qMin(minX, data.left);
        maxX = qMax(maxX, data.right);
        minY = qMin(minY, data.bottom);
        maxY = qMax(maxY, data.top);
    }

    // ���һЩ�߾�
    double marginX = (maxX - minX) * 0.1;
    double marginY = (maxY - minY) * 0.1;
    m_customPlot->xAxis->setRange(minX - marginX, maxX + marginX);
    m_customPlot->yAxis->setRange(minY - marginY, maxY + marginY);

    m_customPlot->replot();
}