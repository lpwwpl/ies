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
#include "qcustomplot.h"

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

    // 从文件加载数据
    bool loadDataFromFile(const QString& filename);

    // 绘制点列图
    void plotSpotDiagrams();

protected:
    // 右键菜单事件
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    // 处理缩放事件
    void onRangeChanged();

    // 处理鼠标选择事件
    void onMousePress(QMouseEvent* event);
    void onMouseMove(QMouseEvent* event);
    void onMouseRelease(QMouseEvent* event);

    // 右键菜单动作
    void resetView();
    void zoomToSelection();

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
        const QString& fieldName, int fieldIndex);

    // 计算点列中心
    void calculateSpotCenter(const QVector<SpotData>& data, double& centerX, double& centerY);

    // 添加RMS信息到图表
    void addRMSInfoToPlot();

    // 添加中心坐标信息到图表左侧
    void addCenterInfoToPlot(const QMap<int, QPair<double, double>>& spotCenters);

    // 更新点列位置
    void updateSpotPositions();

    // 重新计算垂直偏移
    QMap<int, double> calculateVerticalOffsets();

    // 绘制选择矩形
    void drawSelectionRect(const QRect& rect);

    // 保存和恢复初始视图
    void saveInitialView();
    void restoreInitialView();

private:
    QCustomPlot* m_plot;                            // 图表控件
    QVector<SpotData> m_spotData;                   // 点列图数据
    QMap<QString, QColor> m_colorMap;               // 颜色映射
    QVector<double> m_fieldRMS;                     // RMS直径值
    QVector<double> m_field100;                     // 100%直径值
    QVector<double> m_airyX;                        // Airy X直径值
    QVector<double> m_airyY;                        // Airy Y直径值
    QMap<int, QPair<double, double>> m_spotCenters; // 点列中心坐标
    QMap<int, QVector<QCPGraph*>> m_fieldGraphs;    // 每个视场的图形对象
    QMap<int, QCPItemText*> m_fieldLabels;          // 视场标签
    QCPItemText* m_rmsInfoText;                     // RMS信息文本
    QCPItemText* m_centerInfoText;                  // 中心坐标信息文本
    QCPItemRect* m_selectionRect;                   // 选择矩形
    QMenu* m_contextMenu;                           // 右键菜单
    QAction* m_resetViewAction;                     // 重置视图动作
    QAction* m_zoomToSelectionAction;               // 放大到选择区域动作
    double m_dataScale;                             // 数据缩放比例
    bool m_isInitialPlot;                           // 是否是初始绘制
    QMap<int, QVector<SpotData>> m_fieldData;       // 按视场分组的数据
    QPoint m_selectionStart;                        // 选择开始位置
    QPoint m_selectionEnd;                          // 选择结束位置
    bool m_isSelecting;                             // 是否正在选择
    QCPRange m_initialXRange;                       // 初始X轴范围
    QCPRange m_initialYRange;                       // 初始Y轴范围
};

#endif // SPOTDIAGRAMWIDGET_H