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

    // 加载视场数据文件
    bool loadFieldDataFile(const QString& filename);

    // 清除所有数据
    void clearData();

    // 更新显示
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