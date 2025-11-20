#ifndef DISTORTIONGRIDWIDGET_H
#define DISTORTIONGRIDWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "qcustomplot.h"

struct DistortionPoint {
    double relXFld;    // 相对X视场
    double relYFld;    // 相对Y视场
    double paraxX;     // 理想X坐标
    double paraxY;     // 理想Y坐标
    double realX;      // 实际X坐标
    double realY;      // 实际Y坐标
    double radDist;    // 径向畸变
    double tanDist;    // 切向畸变
};

class DistortionGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DistortionGridWidget(QWidget* parent = nullptr);

    // 公共接口
    bool loadDistortionData(const QString& filename);
    void plotGrids();

private:
    void setupPlot();
    void organizeGridData();
    void plotIdealGrid();
    void plotDistortedGrid();
    void updateInfoText();

    QCustomPlot* m_customPlot;

    QVector<DistortionPoint> m_distortionData;
    QVector<QVector<DistortionPoint>> m_gridData; // 11x11网格数据

    double m_maxRadialDistortion = 0.0;
    double m_maxTangentialDistortion = 0.0;
};

#endif // DISTORTIONGRIDWIDGET_H