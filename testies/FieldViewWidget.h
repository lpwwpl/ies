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

    bool loadFieldDataFile(const QString& filename);
    void clearData();
    void resetView();

private:
    void setupFieldViewPlot();
    void updatePlot();
    void drawFieldArrows();
    QPen getArrowPen(double magnitude);
    double getArrowLength(double magnitude);

    QCustomPlot* customPlot;
    QVector<FieldData> fieldData;
    double maxMagnitude;
    double minMagnitude;

    // 默认视图范围
    double m_defaultRange = 50.0;
};

#endif // FIELDVIEWWIDGET_H