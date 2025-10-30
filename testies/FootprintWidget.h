#ifndef FOOTPRINTWIDGET_H
#define FOOTPRINTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include "qcustomplot.h"

struct FootprintData {
    QString field;
    double left;
    double right;
    double top;
    double bottom;
    double radius;
};

class FootprintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FootprintWidget(QWidget* parent = nullptr);
    bool loadDataFromFile(const QString& filename);
    void plotFootprints();

private:
    QCustomPlot* m_customPlot;
    QVector<FootprintData> m_footprintData;

    void setupPlot();
    QColor getFieldColor(const QString& field);
};

#endif // FOOTPRINTWIDGET_H