//#ifndef ABERRATIONWIDGET_H
//#define ABERRATIONWIDGET_H
//
//#include <QWidget>
//#include <QVector>
//#include <QString>
//#include <QColor>
//#include <QGridLayout>
//#include <QLabel>
//#include "qcustomplot.h"
//
//// 数据结构
//struct AberrationData {
//    QString chartName;
//    int lineIndex;
//    QString lineLabel;
//    QString lineColor;
//    int viewFieldIndex;
//    int waveLengthIndex;
//    double waveLength;
//    QVector<double> normalizedAperture;
//    QVector<double> aberration;
//};
//
//class AberrationWidget : public QWidget
//{
//    Q_OBJECT
//
//public:
//    explicit AberrationWidget(QWidget* parent = nullptr);
//
//    // 加载数据文件
//    bool loadDataFromFile(const QString& filename);
//
//    QColor getColorFromString(const QString& colorStr);
//public slots:
//  
//
//private:
//    void setupUI();
//
//
//    QGridLayout* m_mainLayout;
//    QVector<QCustomPlot*> m_plots;
//    QVector<AberrationData> dataList;
//
//
//    // 按chartName分组数据
//    static QMap<QString, QVector<AberrationData>> groupByChartName(const QVector<AberrationData>& data) {
//        QMap<QString, QVector<AberrationData>> groupedData;
//
//        for (const AberrationData& item : data) {
//            groupedData[item.chartName].append(item);
//        }
//
//        return groupedData;
//    }
//
//    // 按chartName分组并排序（按lineIndex）
//    static QMap<QString, QVector<AberrationData>> groupAndSortByChartName(const QVector<AberrationData>& data) {
//        QMap<QString, QVector<AberrationData>> groupedData = groupByChartName(data);
//
//        // 对每个组内的数据按lineIndex排序
//        for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
//            std::sort(it->begin(), it->end(),
//                [](const AberrationData& a, const AberrationData& b) {
//                    return a.lineIndex < b.lineIndex;
//                });
//        }
//
//        return groupedData;
//    }
//
//    // 获取所有不同的chartName
//    static QVector<QString> getUniqueChartNames(const QVector<AberrationData>& data) {
//        QSet<QString> uniqueNames;
//        for (const AberrationData& item : data) {
//            uniqueNames.insert(item.chartName);
//        }
//        return uniqueNames.values().toVector();
//    }
//
//    // 按chartName和viewFieldIndex双重分组
//    static QMap<QString, QMap<int, QVector<AberrationData>>> groupByChartAndField(const QVector<AberrationData>& data) {
//        QMap<QString, QMap<int, QVector<AberrationData>>> groupedData;
//
//        for (const AberrationData& item : data) {
//            groupedData[item.chartName][item.viewFieldIndex].append(item);
//        }
//
//        return groupedData;
//    }
//
//    // 打印分组信息（用于调试）
//    static void printGroupingInfo(const QMap<QString, QVector<AberrationData>>& groupedData) {
//        qDebug() << "分组结果:";
//        for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
//            qDebug() << "Chart:" << it.key() << "包含" << it->size() << "条数据";
//            for (const AberrationData& item : it.value()) {
//                qDebug() << "  - LineIndex:" << item.lineIndex
//                    << "WaveLength:" << item.waveLength
//                    << "FieldIndex:" << item.viewFieldIndex;
//            }
//        }
//    }
//};
//
//#endif // ABERRATIONWIDGET_H
#ifndef ABERRATIONWIDGET_H
#define ABERRATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QSet>
#include <QDebug>
#include <algorithm>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include "PlotSetting.h"
#include "QwtPropertyBrowser.h"
#include "PlotBase.h"
class QSplitter;
class QComboBox;
// 数据结构
struct AberrationData {
    QString chartName;
    int lineIndex;
    QString lineLabel;
    QString lineColor;
    int viewFieldIndex;
    int waveLengthIndex;
    double waveLength;
    QVector<double> normalizedAperture;
    QVector<double> aberration;
};
// 存储每个图表的所有信息
struct PlotInfo 
{
    //用来记录每个图标的初始XY范围
    double m_current_factor = 1;
    double m_initialXMin, m_initialXMax;        // 初始X轴范围
    double m_initialYMin, m_initialYMax;        // 初始Y轴范围
    double m_initialXMin_orig, m_initialXMax_orig;        // 初始X轴范围
    double m_initialYMin_orig, m_initialYMax_orig;        // 初始Y轴范围
    QwtPlot* plot;
    QwtPlotGrid* grid;
    QwtLegend* legend;
    PlotSettings* settings;
    QList<QwtPlotCurve*> curves;      // 该图表中的所有曲线

    PlotInfo operator=(const PlotInfo& other)
    {
        m_current_factor = other.m_current_factor;
        m_initialXMin = other.m_initialXMin;
        m_initialXMax = other.m_initialXMax;
        m_initialYMin = other.m_initialYMin;
        m_initialYMax = other.m_initialYMax;
        m_initialXMin_orig = other.m_initialXMin_orig;
        m_initialXMax_orig = other.m_initialXMax_orig;
        m_initialYMin_orig = other.m_initialYMin_orig;
        m_initialYMin_orig = other.m_initialYMin_orig;
        *settings = *other.settings;
        return *this;
    }
};
class AberrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AberrationWidget(QWidget* parent = nullptr);
    ~AberrationWidget();
    void saveInitialView(PlotInfo&);
    // 加载数据文件
    bool loadDataFromFile(const QString& filename);

    QColor getColorFromString(const QString& colorStr);
    void applyAllSettingsToAllPlots();
    void syncSettingsToPlot(PlotInfo* settings, PlotInfo* info);
    void updateCurveStyle(QwtPlot* plot,MTFLine&);
public slots:
    void onLegendClicked(const QVariant& itemInfo);
    void onPropertyChanged();                     // 属性浏览器修改信号
    void onPlotSelected(int index);               // 切换当前编辑的图表
    void onApplyToAllToggled(bool checked);       // “应用到所有”勾选状态变化

    void updateYScaleAxes(QwtPlot* plot, PlotInfo&);
    void updateXScaleAxes(QwtPlot* plot, PlotInfo&);
    void updateAxesSettings(QwtPlot* plot, PlotInfo&);
    void updateAxesSettings_noparam();


    void updateAutoScaleX();
    void updateAutoScaleY();
    void updateX();
    void updateY();
    void updateXY() { updateX(); updateY(); }
private:
    void setupUI();
    void clearPlots();
    void setupPlotInteractions(QwtPlot* plot);

    QVector<QwtPlot*> m_plots;
    // 交互相关的对象
    QList<QwtPlotCurve*> m_curves;


    QSplitter* m_splitter;
    // UI 控件
    QGridLayout* m_mainLayout;           // 主布局（包含图表网格）
    QVBoxLayout* m_controlLayout;        // 右侧控制面板布局
    QComboBox* m_plotCombo;              // 图表选择下拉框
    QCheckBox* m_applyToAllCheckBox;     // 应用到所有图表复选框
    QwtPropertyBrowser* m_propertyBrowser; // 属性编辑器

    // 数据
    QVector<AberrationData> dataList;
    QList<PlotInfo*> m_plotInfos;          // 所有图表的详细信息
    int m_currentPlotIndex;               // 当前选中的图表索引


    // 按chartName分组数据
    static QMap<QString, QVector<AberrationData>> groupByChartName(const QVector<AberrationData>& data) 
    {
        QMap<QString, QVector<AberrationData>> groupedData;

        for (const AberrationData& item : data) {
            groupedData[item.chartName].append(item);
        }

        return groupedData;
    }

    // 按chartName分组并排序（按lineIndex）
    static QMap<QString, QVector<AberrationData>> groupAndSortByChartName(const QVector<AberrationData>& data) 
    {
        QMap<QString, QVector<AberrationData>> groupedData = groupByChartName(data);

        // 对每个组内的数据按lineIndex排序
        for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
            std::sort(it->begin(), it->end(),
                [](const AberrationData& a, const AberrationData& b) {
                    return a.lineIndex < b.lineIndex;
                });
        }

        return groupedData;
    }

    // 获取所有不同的chartName
    static QVector<QString> getUniqueChartNames(const QVector<AberrationData>& data) 
    {
        QSet<QString> uniqueNames;
        for (const AberrationData& item : data) {
            uniqueNames.insert(item.chartName);
        }
        return uniqueNames.values().toVector();
    }

    // 按chartName和viewFieldIndex双重分组
    static QMap<QString, QMap<int, QVector<AberrationData>>> groupByChartAndField(const QVector<AberrationData>& data) 
    {
        QMap<QString, QMap<int, QVector<AberrationData>>> groupedData;

        for (const AberrationData& item : data) {
            groupedData[item.chartName][item.viewFieldIndex].append(item);
        }

        return groupedData;
    }

    // 打印分组信息（用于调试）
    static void printGroupingInfo(const QMap<QString, QVector<AberrationData>>& groupedData) 
    {
        qDebug() << "分组结果:";
        for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
            qDebug() << "Chart:" << it.key() << "包含" << it->size() << "条数据";
            for (const AberrationData& item : it.value()) {
                qDebug() << "  - LineIndex:" << item.lineIndex
                    << "WaveLength:" << item.waveLength
                    << "FieldIndex:" << item.viewFieldIndex;
            }
        }
    }
};

#endif // ABERRATIONWIDGET_H