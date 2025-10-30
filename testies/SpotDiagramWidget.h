#ifndef SPOTDIAGRAMPLOTTER_H
#define SPOTDIAGRAMPLOTTER_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QMap>

// QCustomPlot 头文件
#include "qcustomplot.h"

struct SpotData {
    int index;
    double x;
    double y;
    int fieldIndex;
    int waveIndex;
    double wavelength;
    QString colorName;
};

class SpotDiagramPlotter : public QWidget
{
    Q_OBJECT

public:
    explicit SpotDiagramPlotter(QWidget* parent = nullptr);
    bool loadDataFromFile(const QString& filename);
    void plotSpotDiagrams();

private:
    void parseDataLine(const QString& line);
    void setupPlot();
    void addSpotDataToPlot(const QVector<SpotData>& data, double verticalOffset, const QString& fieldName);
    void calculateRMS(const QVector<SpotData>& data, double& rmsX, double& rmsY, double& rmsRadius);

    QVector<SpotData> m_spotData;
    QCustomPlot* m_plot;
    QMap<QString, QColor> m_colorMap;

    // RMS 值存储
    QMap<int, double> m_rmsValues;
};

#endif // SPOTDIAGRAMPLOTTER_H