#include "MTFViewWidget.h"
#include "qcustomplot.h"

#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMouseEvent>

MTFViewer::MTFViewer(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
{
    setupUI();
}

void MTFViewer::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // ������ͼ����
    m_plot = new QCustomPlot(this);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
    m_plot->setMinimumSize(400, 300);

    // ����������
    m_plot->xAxis->setLabel("�ռ�Ƶ�� (cycles/mm)");
    m_plot->yAxis->setLabel("MTFֵ");
    m_plot->xAxis->setRange(0, 200);
    m_plot->yAxis->setRange(0, 1.1);

    // ��ʾͼ��
    m_plot->legend->setVisible(true);
    m_plot->legend->setFillOrder(QCPLegend::foColumnsFirst);

    layout->addWidget(m_plot);

    // �����ź�
    connect(m_plot->legend, &QCPLegend::selectionChanged, this, &MTFViewer::onSelectionChanged);
}

bool MTFViewer::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString("�޷����ļ�: %1").arg(filename));
        return false;
    }

    QTextStream in(&file);
    QString data = in.readAll();
    file.close();

    m_currentFile = filename;
    return loadFromString(data);
}

bool MTFViewer::loadFromString(const QString& data)
{
    m_lines.clear();

    QStringList lines = data.split('\n', Qt::SkipEmptyParts);

    MTFLine currentLine;
    bool readingData = false;
    bool hasValidData = false;

    for (const QString& line : lines) {
        if (line.startsWith("# CHART NAME:")) {
            // ��ͼ��ʼ�������ǰ�����ݣ�����ǰһ����
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                currentLine = MTFLine();
                hasValidData = true;
            }
            readingData = false;
        }
        else if (line.startsWith("  LINE INDEX:")) {
            // ��������ʼ
            if (readingData && !currentLine.data.isEmpty()) {
                m_lines.append(currentLine);
                hasValidData = true;
            }
            currentLine = MTFLine();
            currentLine.index = line.split(":").last().trimmed().toInt();
            currentLine.visible = true;
            readingData = false;
        }
        else if (line.startsWith("  LINE LABEL:")) {
            currentLine.label = line.split(":").last().trimmed();
        }
        else if (line.startsWith("  LINE COLOR:")) {
            currentLine.colorName = line.split(":").last().trimmed();
        }
        else if (line.startsWith("     INDEX")) {
            // ���ݱ�ͷ����һ�п�ʼ������
            readingData = true;
        }
        else if (readingData && !line.trimmed().isEmpty() && !line.startsWith("#")) {
            // ��ȡ������
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                bool ok1, ok2;
                double x = parts[1].toDouble(&ok1);
                double y = parts[2].toDouble(&ok2);
                if (ok1 && ok2) {
                    currentLine.data.append(QPointF(x, y));
                    hasValidData = true;
                }
            }
        }
    }

    // ������һ����
    if (!currentLine.data.isEmpty()) {
        m_lines.append(currentLine);
        hasValidData = true;
    }

    if (hasValidData) {
        plotData();
        emit dataLoaded(true);
        return true;
    }
    else {
        emit errorOccurred("δ�ҵ���Ч��MTF����");
        return false;
    }
}

void MTFViewer::clearData()
{
    m_lines.clear();
    m_plot->clearGraphs();
    m_plot->replot();
    m_currentFile.clear();
}

void MTFViewer::setXRange(double min, double max)
{
    m_plot->xAxis->setRange(min, max);
    m_plot->replot();
}

void MTFViewer::setYRange(double min, double max)
{
    m_plot->yAxis->setRange(min, max);
    m_plot->replot();
}

void MTFViewer::setTitle(const QString& title)
{
    m_plot->plotLayout()->insertRow(0);
    m_plot->plotLayout()->addElement(0, 0, new QCPTextElement(m_plot, title, QFont("sans", 12, QFont::Bold)));
    m_plot->replot();
}

void MTFViewer::plotData()
{
    m_plot->clearGraphs();

    // �����������ǩ
    m_plot->xAxis->setLabel("�ռ�Ƶ�� (cycles/mm)");
    m_plot->yAxis->setLabel("MTFֵ");

    // Ĭ����ɫ�б�
    QVector<QColor> defaultColors = {
        QColor(255, 0, 0),     // ��ɫ
        QColor(0, 0, 255),     // ��ɫ
        QColor(0, 255, 0),     // ��ɫ
        QColor(255, 0, 255),   // ���
        QColor(0, 255, 255),   // ��ɫ
        QColor(255, 165, 0)    // ��ɫ
    };

    // Ϊÿ���ߴ���ͼ��
    for (int i = 0; i < m_lines.size(); ++i) {
        const MTFLine& line = m_lines[i];

        QCPGraph* graph = m_plot->addGraph();

        // ׼������
        QVector<double> x, y;
        for (const QPointF& point : line.data) {
            x.append(point.x());
            y.append(point.y());
        }

        graph->setData(x, y);

        // ������������
        QColor color = getColorFromName(line.colorName);
        if (!color.isValid()) {
            color = defaultColors[i % defaultColors.size()];
        }

        // ����������ʽ
        Qt::PenStyle style = Qt::SolidLine;
        if (i >= 3) style = Qt::DashLine; // ��4���߿�ʼ������

        QPen pen(color, 2);
        pen.setStyle(style);
        graph->setPen(pen);

        // ����ͼ������
        QString legendName = QString("���� %1").arg(line.index);
        if (!line.label.isEmpty()) {
            legendName += ": " + line.label;
        }
        graph->setName(legendName);

        // ����ɢ����ʽ
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

        // ���ÿɼ���
        graph->setVisible(line.visible);
    }

    m_plot->rescaleAxes();
    m_plot->replot();
}

QColor MTFViewer::getColorFromName(const QString& colorName)
{
    if (colorName == "RGBRed") return QColor(255, 0, 0);
    if (colorName == "RGBGreen") return QColor(0, 255, 0);
    if (colorName == "RGBBlue") return QColor(0, 0, 255);
    return QColor(); // ��Ч��ɫ
}

void MTFViewer::onLegendClick(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event)
{
    Q_UNUSED(legend)
        Q_UNUSED(event)

        if (item) {
            QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
            if (plItem) {
                bool visible = !plItem->plottable()->visible();
                plItem->plottable()->setVisible(visible);

                // �����ڲ�����״̬
                for (int i = 0; i < m_lines.size(); ++i) {
                    if (m_plot->graph(i) == plItem->plottable()) {
                        m_lines[i].visible = visible;
                        break;
                    }
                }

                m_plot->replot();
            }
        }
}

void MTFViewer::onSelectionChanged()
{
    // ����ͼ��ѡ��仯
    m_plot->replot();
}