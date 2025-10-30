#include "fieldviewwidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FieldViewWidget::FieldViewWidget(QWidget* parent)
    : QWidget(parent), maxMagnitude(0.0), minMagnitude(0.0)
{
    // ��������
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // ����QCustomPlot
    customPlot = new QCustomPlot(this);
    layout->addWidget(customPlot);

    // ����ͼ��
    setupFieldViewPlot();
}

FieldViewWidget::~FieldViewWidget()
{
}

void FieldViewWidget::setupFieldViewPlot()
{
    // ����������
    customPlot->xAxis->setLabel("X");
    customPlot->yAxis->setLabel("Y");

    // ���������᷶Χ���Գ���ʾ��
    double range = 50.0; // Ĭ�Ϸ�Χ
    customPlot->xAxis->setRange(-range, range);
    customPlot->yAxis->setRange(-range, range);

    // ��������
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);

    // ���ñ���
    customPlot->plotLayout()->insertRow(0);
    QCPTextElement* title = new QCPTextElement(customPlot, "FOV", QFont("sans", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    // ���õȱ�����ʾ
    //customPlot->setAspectRatio(1.0);
}

bool FieldViewWidget::loadFieldDataFile(const QString& filename)
{
    // �����������
    clearData();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "����", "�޷����ļ�: " + filename);
        return false;
    }

    QTextStream in(&file);
    int lineNumber = 0;
    bool hasValidData = false;

    // ��ʼ�����ֵ��Сֵ
    maxMagnitude = -1e9;
    minMagnitude = 1e9;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // �������кͱ�����
        if (line.isEmpty() || line.startsWith("index") || line.startsWith("#")) {
            continue;
        }

        // ����������
        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 5) {
            bool ok;
            FieldData data;

            data.index = parts[0].toInt(&ok);
            if (!ok) continue;

            data.xField = parts[1].toDouble(&ok);
            if (!ok) continue;

            data.yField = parts[2].toDouble(&ok);
            if (!ok) continue;

            data.magnitude = parts[3].toDouble(&ok);
            if (!ok) continue;

            data.angle = parts[4].toDouble(&ok);
            if (!ok) continue;

            fieldData.append(data);
            hasValidData = true;

            // �������ֵ��Сֵ
            if (data.magnitude > maxMagnitude) maxMagnitude = data.magnitude;
            if (data.magnitude < minMagnitude) minMagnitude = data.magnitude;
        }
    }

    file.close();

    if (hasValidData) {
        updatePlot();
        //qDebug() << "�ɹ�����" << fieldData.size() << "���ӳ���";
        //qDebug() << "��ֵ��Χ:" << minMagnitude << "��" << maxMagnitude;
        return true;
    }
    else {
        QMessageBox::warning(this, "����", "�ļ���û���ҵ���Ч���ӳ�����");
        return false;
    }
}

void FieldViewWidget::clearData()
{
    fieldData.clear();
    maxMagnitude = 0.0;
    minMagnitude = 0.0;
    customPlot->clearItems();
    customPlot->replot();
}

void FieldViewWidget::updatePlot()
{
    // ���֮ǰ�ļ�ͷ
    customPlot->clearItems();

    // �Զ����������᷶Χ
    if (!fieldData.isEmpty()) {
        double xMin = 1e9, xMax = -1e9, yMin = 1e9, yMax = -1e9;
        for (const FieldData& data : fieldData) {
            if (data.xField < xMin) xMin = data.xField;
            if (data.xField > xMax) xMax = data.xField;
            if (data.yField < yMin) yMin = data.yField;
            if (data.yField > yMax) yMax = data.yField;
        }

        // ��ӱ߾�
        double margin = std::max((xMax - xMin), (yMax - yMin)) * 0.1;
        customPlot->xAxis->setRange(xMin - margin, xMax + margin);
        customPlot->yAxis->setRange(yMin - margin, yMax + margin);
    }

    // ���Ƽ�ͷ
    drawFieldArrows();

    customPlot->replot();
}

void FieldViewWidget::drawFieldArrows()
{
    // ��ͷ���Ȼ����ӳ���Χ�ı���
    double xRange = customPlot->xAxis->range().size();
    double yRange = customPlot->yAxis->range().size();
    double baseArrowLength = std::min(xRange, yRange) * 0.1;

    for (const FieldData& data : fieldData) {
        // ������ͷ
        QCPItemLine* arrow = new QCPItemLine(customPlot);

        // �����ͷ���򣨽Ƕ�ת��Ϊ���ȣ�
        double angleRad = data.angle * M_PI / 180.0;

        // ��ͷ��㣨���ݵ�λ�ã�
        arrow->start->setCoords(data.xField, data.yField);

        // ��ͷ�յ㣨���ڽǶȺͳ��ȼ��㣩
        double endX = data.xField + baseArrowLength * std::cos(angleRad);
        double endY = data.yField + baseArrowLength * std::sin(angleRad);
        arrow->end->setCoords(endX, endY);

        // ���ü�ͷ��ʽ
        QPen arrowPen = getArrowPen(data.magnitude);
        arrow->setPen(arrowPen);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
    }
}

QPen FieldViewWidget::getArrowPen(double magnitude)
{
    // ���ݷ�ֵ���ü�ͷ��ɫ�ʹ�ϸ
    // ʹ����ɫϵ����ֵԽ����ɫԽ��/��Խ��

    // ��һ����ֵ
    double normalized = 0.5;
    if (maxMagnitude > minMagnitude) {
        normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
    }

    // ������ɫ����ǳ����������
    int blueValue = 100 + static_cast<int>(155 * normalized);
    QColor arrowColor(0, 0, blueValue);

    // �����߿�1.0 - 3.0��
    double lineWidth = 1.0 + normalized * 2.0;

    return QPen(arrowColor, lineWidth);
}