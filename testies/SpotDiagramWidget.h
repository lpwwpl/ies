//#ifndef SPOTDIAGRAMWIDGET_H
//#define SPOTDIAGRAMWIDGET_H
//
//#include <QWidget>
//#include <QVBoxLayout>
//#include <QFile>
//#include <QTextStream>
//#include <QMap>
//#include <QVector>
//#include <QPair>
//#include <QDebug>
//#include <QMenu>
//#include <QAction>
//#include <QContextMenuEvent>
//#include "qcustomplot.h"
//
//// 点列图数据结构
//struct SpotData {
//    int index;          // 索引
//    double x;           // X坐标
//    double y;           // Y坐标
//    int fieldIndex;     // 视场索引
//    int waveIndex;      // 波长索引
//    double wavelength;  // 波长
//    QString colorName;  // 颜色名称
//};
//
//class SpotDiagramPlotter : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit SpotDiagramPlotter(QWidget* parent = nullptr);
//
//    // 从文件加载数据
//    bool loadDataFromFile(const QString& filename);
//
//    // 绘制点列图
//    void plotSpotDiagrams();
//
//
//private slots:
//    // 处理缩放事件
//    void onRangeChanged();
//
//
//    // 右键菜单动作
//    void resetView();
//
//
//private:
//    // 设置图表
//    void setupPlot();
//
//    // 数据解析方法
//    void parseDataLine(const QString& line);
//    void parseRMSData(QTextStream& in);
//    void parse100PercentData(QTextStream& in);
//    void parseAiryData(QTextStream& in);
//
//    // 添加点数据到图表
//    void addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
//        const QString& fieldName, int fieldIndex);
//
//    // 计算点列中心
//    void calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY);
//
//    // 添加信息到刻度
//    void addInfoToTicks();
//
//    // 更新点列位置
//    void updateSpotPositions();
//
//    // 重新计算垂直偏移
//    QMap<int, double> calculateVerticalOffsets();
//
//
//    // 保存和恢复初始视图
//    void saveInitialView();
//    void restoreInitialView();
//
//private:
//    QCustomPlot* m_plot;                            // 图表控件
//    QVector<SpotData> m_spotData;                   // 点列图数据
//    QMap<QString, QColor> m_colorMap;               // 颜色映射
//    QVector<double> m_fieldRMS;                     // RMS直径值
//    QVector<double> m_field100;                     // 100%直径值
//    QVector<double> m_airyX;                        // Airy X直径值
//    QVector<double> m_airyY;                        // Airy Y直径值
//    QMap<int, QPair<double, double>> m_spotCenters; // 点列中心坐标
//    QMap<int, QVector<QCPGraph*>> m_fieldGraphs;    // 每个视场的图形对象
//    QMap<int, QCPItemText*> m_fieldLabels;          // 视场标签
//
//    double m_dataScale;                             // 数据缩放比例
//    bool m_isInitialPlot;                           // 是否是初始绘制
//    QMap<int, QVector<SpotData>> m_fieldData;       // 按视场分组的数据
//    QCPRange m_initialXRange;                       // 初始X轴范围
//    QCPRange m_initialYRange;                       // 初始Y轴范围
//
//
//    double xRange;
//    double yMin;
//    double yMax;
//};
//
//#endif // SPOTDIAGRAMWIDGET_H

#ifndef SPOTDIAGRAMWIDGET_H
#define SPOTDIAGRAMWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_widget.h>
#include <qwt_text.h>
#include <qwt_legend.h>

// 点列图数据结构
struct SpotData {
    int index;          // 索引
    double x;           // X坐标
    double y;           // Y坐标
    int fieldIndex;     // 视场索引
    int waveIndex;      // 波长索引
    double wavelength;  // 波长
    QString colorName;  // 颜色名称
};

class SpotDiagramPlotter : public QWidget
{
    Q_OBJECT

public:
    explicit SpotDiagramPlotter(QWidget* parent = nullptr);
    ~SpotDiagramPlotter();

    // 从文件加载数据
    bool loadDataFromFile(const QString& filename);

    // 绘制点列图
    void plotSpotDiagrams();

private slots:
    void resetView();
    void showContextMenu(const QPoint& pos);

private:
    // 设置图表
    void setupPlot();
    void setupLegend();

    // 数据解析方法
    void parseDataLine(const QString& line);
    void parseRMSData(QTextStream& in);
    void parse100PercentData(QTextStream& in);
    void parseAiryData(QTextStream& in);

    // 添加点数据到图表
    void addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
        const QString& fieldName, int fieldIndex);

    // 计算点列中心
    void calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY);

    // 添加信息到刻度
    void addInfoToTicks();

    // 更新点列位置
    void updateSpotPositions();

    // 重新计算垂直偏移
    QMap<int, double> calculateVerticalOffsets();

    // 保存和恢复初始视图
    void saveInitialView();
    void restoreInitialView();

    // 清除图表
    void clearPlot();

private:
    QwtPlot* m_plot;                            // Qwt图表控件
    QwtPlotGrid* m_grid;                         // 网格
    QVector<SpotData> m_spotData;               // 点列图数据
    QMap<QString, QColor> m_colorMap;           // 颜色映射
    QVector<double> m_fieldRMS;                 // RMS直径值
    QVector<double> m_field100;                 // 100%直径值
    QVector<double> m_airyX;                    // Airy X直径值
    QVector<double> m_airyY;                    // Airy Y直径值
    QMap<int, QPair<double, double>> m_spotCenters; // 点列中心坐标
    QMap<int, QVector<QwtPlotMarker*>> m_fieldMarkers; // 每个视场的标记对象
    QMap<int, QwtPlotMarker*> m_fieldLabels;    // 视场标签

    QwtPlotZoomer* m_zoomer;                    // 缩放器
    QwtPlotPanner* m_panner;                    // 平移器

    double m_dataScale;                         // 数据缩放比例
    QMap<int, QVector<SpotData>> m_fieldData;   // 按视场分组的数据
    QRectF m_initialView;                       // 初始视图范围
    QVector<QwtPlotCurve*> m_curves;            // 曲线对象集合
    QVector<QwtPlotMarker*> m_markers;          // 标记对象集合

    double xRange;                              // X轴范围
    double yMin;                                // Y轴最小值
    double yMax;                                // Y轴最大值

    // 存储每个视场每个颜色的数据（用于动态更新）
    QMap<int, QMap<QString, QVector<QPointF>>> m_originalData;
};

#endif // SPOTDIAGRAMWIDGET_H