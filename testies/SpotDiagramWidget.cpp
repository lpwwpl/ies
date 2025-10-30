#include "SpotDiagramWidget.h"
#include <QDebug>
#include <cmath>

SpotDiagramPlotter::SpotDiagramPlotter(QWidget* parent)
    : QWidget(parent)
{
    // ��ʼ����ɫӳ��
    m_colorMap["RGBOrange"] = QColor(255, 165, 0);
    m_colorMap["RGBYellow"] = QColor(255, 255, 0);
    m_colorMap["RGBGreen"] = QColor(0, 255, 0);
    m_colorMap["RGBCyan"] = QColor(0, 255, 255);

    // ��������
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_plot = new QCustomPlot(this);
    mainLayout->addWidget(m_plot);

    setupPlot();
}

bool SpotDiagramPlotter::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "�޷����ļ�:" << filename;
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
    //qDebug() << "�ɹ�����" << m_spotData.size() << "�����ݵ�";
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
    // �����ϲ����Ҳ�������
    m_plot->xAxis2->setVisible(true);
    m_plot->yAxis2->setVisible(true);

    // ����ͼ���������
    m_plot->xAxis->setLabel("X(mm)");
    m_plot->yAxis->setLabel("postion");

    // �����ϲ����Ҳ�������ı�ǩ
    m_plot->xAxis2->setLabel("X(mm)");
    m_plot->yAxis2->setLabel("RMS");

    // ���������᷶Χ��ʹ�ϲ����²��������Ҳ�ͬ��
    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis2, SLOT(setRange(QCPRange)));

    // ����ѡ�����ź��϶�
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // ���ñ���
    m_plot->plotLayout()->insertRow(0);
    m_plot->plotLayout()->addElement(0, 0,
        new QCPTextElement(m_plot, QStringLiteral("��ѧϵͳ����ͼ����"), QFont("sans", 14, QFont::Bold)));
}

void SpotDiagramPlotter::plotSpotDiagrams()
{
    // �������ͼ��
    m_plot->clearPlottables();
    m_plot->clearItems();

    // ���ӳ���������
    QMap<int, QVector<SpotData>> fieldData;
    for (const SpotData& data : m_spotData) {
        fieldData[data.fieldIndex].append(data);
    }

    // Ϊÿ���ӳ�����ͼ�Σ���ֱ�ֲ�
    QList<int> fieldIndices = fieldData.keys();
    std::sort(fieldIndices.begin(), fieldIndices.end());

    // ֻȡǰ3���ӳ�
    int maxFields = qMin(3, fieldIndices.size());

    // ����X�᷶Χ
    double xRange = 0.025; // X�᷶Χ

    // ����Y�᷶Χ - ���ӷ�Χ���ṩ����ռ�
    double yRange = xRange * 6.0; // ��һ������Y�᷶Χ

    // ���㴹ֱƫ�� - ʹ����ͼ��Y���Ͼ��ȷֲ��Ҿ���
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

    // �����Զ����Y��̶ȱ�ǩ
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    // �洢ÿ���ӳ���RMSֵ
    QMap<int, double> fieldRMS;

    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = verticalOffsets[i];
        QString fieldName = QString("%1").arg(fieldIndex);

        // ���Y��̶ȱ�ǩ
        textTicker->addTick(verticalOffset, fieldName);

        // ����RMS���洢
        double rmsX, rmsY, rmsRadius;
        calculateRMS(fieldData[fieldIndex], rmsX, rmsY, rmsRadius);
        fieldRMS[fieldIndex] = rmsRadius;

        addSpotDataToPlot(fieldData[fieldIndex], verticalOffset, fieldName);
    }

    // ����Y��ʹ���ı��̶�
    m_plot->yAxis->setTicker(textTicker);

    // Ϊ�Ҳ�Y�ᴴ��RMS�̶ȱ�ǩ
    QSharedPointer<QCPAxisTickerText> rmsTicker(new QCPAxisTickerText);
    for (int i = 0; i < maxFields; i++) {
        int fieldIndex = fieldIndices[i];
        double verticalOffset = verticalOffsets[i];
        QString rmsLabel = QString("%1").arg(fieldRMS[fieldIndex], 0, 'f', 6);
        rmsTicker->addTick(verticalOffset, rmsLabel);
    }
    m_plot->yAxis2->setTicker(rmsTicker);

    // ���������᷶Χ - ʹ����ͼ��Y���Ͼ�����ʾ
    m_plot->xAxis->setRange(-xRange, xRange);
    m_plot->yAxis->setRange(-yRange / 2, yRange / 2);

    // ���ͼ��
    //m_plot->legend->setVisible(true);
    //m_plot->legend->setFont(QFont("Arial", 9));
    //m_plot->legend->setRowSpacing(-3);

    // ���»���
    m_plot->replot();
}

void SpotDiagramPlotter::addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset, const QString& fieldName)
{
    if (data.isEmpty()) return;

    // ����ɫ��������
    QMap<QString, QVector<QCPGraphData>> colorData;

    for (const SpotData& spot : data) {
        QCPGraphData point;
        point.key = spot.x;
        point.value = spot.y + verticalOffset; // ��Ӵ�ֱƫ��
        colorData[spot.colorName].append(point);
    }

    // Ϊÿ����ɫ����ͼ��
    for (auto it = colorData.begin(); it != colorData.end(); ++it) {
        const QString& colorName = it.key();
        const QVector<QCPGraphData>& points = it.value();

        QCPGraph* graph = m_plot->addGraph();
        graph->setName(colorName);

        // ������ɫ����ʽ
        QColor color = m_colorMap.value(colorName, Qt::black);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, color, 4));
        graph->setLineStyle(QCPGraph::lsNone);
        //graph->setSelectable(false);

        // �������
        graph->data()->set(points);
    }

    // ���㲢����RMSԲ
    double rmsX, rmsY, rmsRadius;
    calculateRMS(data, rmsX, rmsY, rmsRadius);
    m_rmsValues[data.first().fieldIndex] = rmsRadius;

    // ����RMSԲ
    QCPItemEllipse* rmsCircle = new QCPItemEllipse(m_plot);
    rmsCircle->topLeft->setCoords(-rmsRadius, verticalOffset + rmsRadius);
    rmsCircle->bottomRight->setCoords(rmsRadius, verticalOffset - rmsRadius);
    rmsCircle->setPen(QPen(Qt::blue, 1, Qt::DashLine));
    rmsCircle->setSelectable(false);

    // ����ӳ���ǩ
    QCPItemText* fieldLabel = new QCPItemText(m_plot);
    fieldLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    fieldLabel->position->setType(QCPItemPosition::ptAxisRectRatio);

    // �����ǩ��Y���ϵ����λ��
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

    // ����ƽ��ֵ
    double sumX = 0, sumY = 0;
    for (const SpotData& spot : data) {
        sumX += spot.x;
        sumY += spot.y;
    }
    double meanX = sumX / data.size();
    double meanY = sumY / data.size();

    // ����RMS
    double sumVarX = 0, sumVarY = 0;
    for (const SpotData& spot : data) {
        sumVarX += (spot.x - meanX) * (spot.x - meanX);
        sumVarY += (spot.y - meanY) * (spot.y - meanY);
    }

    rmsX = std::sqrt(sumVarX / data.size());
    rmsY = std::sqrt(sumVarY / data.size());
    rmsRadius = std::sqrt(rmsX * rmsX + rmsY * rmsY);
}