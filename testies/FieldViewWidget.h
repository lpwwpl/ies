#ifndef FIELDVIEWWIDGET_H
#define FIELDVIEWWIDGET_H

#include <QWidget>
#include <QVector>
#include "qcustomplot.h"

struct FieldData {
    int index;
    double xField;
    double yField;
    double magnitude;
    double angle;
};

class FieldViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FieldViewWidget(QWidget* parent = nullptr);
    ~FieldViewWidget();

    // �����ӳ������ļ�
    bool loadFieldDataFile(const QString& filename);

    // �����������
    void clearData();

    // ������ʾ
    void updatePlot();

private:
    void setupFieldViewPlot();
    void drawFieldArrows();
    QPen getArrowPen(double magnitude);

    QCustomPlot* customPlot;
    QVector<FieldData> fieldData;
    double maxMagnitude;
    double minMagnitude;
};

#endif // FIELDVIEWWIDGET_H