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
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_text.h>
#include <qwt_symbol.h>

#include "common.h"

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

// 自定义刻度绘制类，用于显示多行文本
class MultiLineScaleDraw : public QwtScaleDraw
{
public:
    MultiLineScaleDraw(const QMap<double, QString>& tickLabels)
        : m_tickLabels(tickLabels)
    {
    }

    virtual QwtText label(double value) const override
    {
        if (m_tickLabels.contains(value)) {
            return QwtText(m_tickLabels[value]);
        }
        return QwtText(); // 返回空文本，不显示其他位置的标签
    }

    // 更新刻度标签
    void updateTickLabels(const QMap<double, QString>& tickLabels) {
        m_tickLabels = tickLabels;
    }

private:
    QMap<double, QString> m_tickLabels;
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
    // 右键菜单动作
    void resetView();
    // 缩放完成后的处理
    void handleZoomed(const QRectF&);
    // 平移完成后的处理
    void handlePanned(int dx, int dy);

protected:
    // 重写上下文菜单事件
    void contextMenuEvent(QContextMenuEvent* event) override;
    // 重写事件过滤器
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // 设置图表
    void setupPlot();

    // 数据解析方法
    void parseDataLine(const QString& line);
    void parseRMSData(QTextStream& in);
    void parse100PercentData(QTextStream& in);
    void parseAiryData(QTextStream& in);

    // 添加点数据到图表
    void addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset,
        const QString& fieldName, int fieldIndex, double centerX, double centerY);

    // 计算点列中心
    void calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY);

    // 添加信息到刻度
    void addInfoToTicks();

    // 更新点列位置
    void updateSpotPositions();

    // 保存和恢复初始视图
    void saveInitialView();
    void restoreInitialView();

    // 清除所有曲线
    void clearAllCurves();

    // 计算点列的实际范围
    void calculateSpotDataRange();

    // 同步左右Y轴刻度
    void synchronizeAxisTicks();

    // 获取视场名称
    QString getFieldName(int fieldIndex) const;

    // 更新刻度线显示
    void updateAxisTicks();

    // 重新应用自定义刻度绘制
    void reapplyScaleDraws();

    // 更新刻度标签位置
    void updateTickLabelPositions();

    // 根据当前Y轴位置更新标签
    void updateLabelsForCurrentView();

private:
    QwtPlot* m_plot;                            // 图表控件
    QVector<SpotData> m_spotData;               // 点列图数据
    QMap<QString, QColor> m_colorMap;           // 颜色映射
    QVector<double> m_fieldRMS;                 // RMS直径值
    QVector<double> m_field100;                 // 100%直径值
    QVector<double> m_airyX;                    // Airy X直径值
    QVector<double> m_airyY;                    // Airy Y直径值
    QMap<int, QPair<double, double>> m_spotCenters; // 点列中心坐标

    // 存储每个视场的曲线
    QMap<int, QVector<QwtPlotCurve*>> m_fieldCurves;
    QMap<int, QVector<QwtPlotMarker*>> m_fieldMarkers;

    QwtPlotZoomer* m_zoomer;                    // 缩放工具
    QwtPlotPanner* m_panner;                    // 平移工具
    QwtPlotGrid* m_grid;                        // 网格

    // 自定义刻度绘制器
    MultiLineScaleDraw* m_leftScaleDraw;
    MultiLineScaleDraw* m_rightScaleDraw;

    double m_dataScale;                         // 数据缩放比例
    bool m_isInitialPlot;                       // 是否是初始绘制
    QMap<int, QVector<SpotData>> m_fieldData;   // 按视场分组的数据

    double m_initialXMin, m_initialXMax;        // 初始X轴范围
    double m_initialYMin, m_initialYMax;        // 初始Y轴范围

    // 点列的实际范围（相对于中心的最大偏移）
    double m_maxSpotXRange;                     // 最大X方向范围
    double m_maxSpotYRange;                     // 最大Y方向范围

    // 用于跟踪当前的Y轴刻度位置和标签
    QMap<double, QString> m_currentLeftTickLabels;
    QMap<double, QString> m_currentRightTickLabels;

    // 原始的点列中心位置（数据坐标系）
    QMap<int, double> m_originalFieldCenters;

    // 固定参数
    static constexpr double FIELD_SPACING = 3.4;  // 点列间距

    // 左侧Y轴偏移量
    static constexpr double LEFT_AXIS_OFFSET = 0.5; // 左侧Y坐标偏移量
};


#endif