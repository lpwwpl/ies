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

class AberrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AberrationWidget(QWidget* parent = nullptr);
    ~AberrationWidget();

    // 加载数据文件
    bool loadDataFromFile(const QString& filename);

    QColor getColorFromString(const QString& colorStr);

public slots:
    void onLegendClicked(const QVariant& itemInfo);

private:
    void setupUI();
    void clearPlots();
    void setupPlotInteractions(QwtPlot* plot);

    QGridLayout* m_mainLayout;
    QVector<QwtPlot*> m_plots;
    QVector<AberrationData> dataList;

    // 交互相关的对象
    QList<QwtPlotCurve*> m_curves;

    // 按chartName分组数据
    static QMap<QString, QVector<AberrationData>> groupByChartName(const QVector<AberrationData>& data) {
        QMap<QString, QVector<AberrationData>> groupedData;

        for (const AberrationData& item : data) {
            groupedData[item.chartName].append(item);
        }

        return groupedData;
    }

    // 按chartName分组并排序（按lineIndex）
    static QMap<QString, QVector<AberrationData>> groupAndSortByChartName(const QVector<AberrationData>& data) {
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
    static QVector<QString> getUniqueChartNames(const QVector<AberrationData>& data) {
        QSet<QString> uniqueNames;
        for (const AberrationData& item : data) {
            uniqueNames.insert(item.chartName);
        }
        return uniqueNames.values().toVector();
    }

    // 按chartName和viewFieldIndex双重分组
    static QMap<QString, QMap<int, QVector<AberrationData>>> groupByChartAndField(const QVector<AberrationData>& data) {
        QMap<QString, QMap<int, QVector<AberrationData>>> groupedData;

        for (const AberrationData& item : data) {
            groupedData[item.chartName][item.viewFieldIndex].append(item);
        }

        return groupedData;
    }

    // 打印分组信息（用于调试）
    static void printGroupingInfo(const QMap<QString, QVector<AberrationData>>& groupedData) {
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