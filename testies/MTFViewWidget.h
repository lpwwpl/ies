#ifndef MTFVIEWER_H
#define MTFVIEWER_H

#include <QWidget>
#include <QVector>
#include <QPointF>

class QCustomPlot;
class QCPGraph;
class QCPLegend;
class QCPAbstractLegendItem;

class MTFViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MTFViewer(QWidget* parent = nullptr);

    // 从文件加载数据
    bool loadFromFile(const QString& filename);
    // 从字符串加载数据
    bool loadFromString(const QString& data);
    // 清除所有数据
    void clearData();
    // 设置坐标轴范围
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    // 设置图表标题
    void setTitle(const QString& title);

signals:
    void dataLoaded(bool success);
    void errorOccurred(const QString& error);

private slots:
    void onLegendClick(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event);
    void onSelectionChanged();

private:
    void setupUI();
    void plotData();
    QColor getColorFromName(const QString& colorName);

    struct MTFLine {
        int index;
        QString label;
        QString colorName;
        QVector<QPointF> data;
        bool visible;
    };

    QCustomPlot* m_plot;
    QVector<MTFLine> m_lines;
    QString m_currentFile;
};

#endif // MTFVIEWER_H