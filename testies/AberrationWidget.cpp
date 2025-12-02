#include "AberrationWidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QApplication>

AberrationWidget::AberrationWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}



void AberrationWidget::setupUI()
{
    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(m_mainLayout);
}

bool AberrationWidget::loadDataFromFile(const QString& filename)
{
    QFile file(filename);
    // 解析数据
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
        return false;
    }


    dataList.clear();

    QTextStream in(&file);
    QString line;
    AberrationData currentData;
    bool readingData = false;
    bool hasData = false;

    // 初始化当前数据
    currentData.lineIndex = -1;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("# CHART NAME:")) {
            // 如果是新数据块开始，保存前一个数据块
            if (currentData.lineIndex != -1) {
                dataList.append(currentData);
                hasData = true;
            }

            // 初始化新数据块
            currentData = AberrationData();
            currentData.chartName = line.mid(14).trimmed();
            readingData = false;
        }
        else if (line.startsWith("LINE INDEX:")) {
            currentData.lineIndex = line.mid(12).trimmed().toInt();
        }
        else if (line.startsWith("LINE LABEL:")) {
            currentData.lineLabel = line.mid(12).trimmed();
        }
        else if (line.startsWith("LINE COLOR:")) {
            currentData.lineColor = line.mid(12).trimmed();
        }
        else if (line.startsWith("VIEW FIELD INDEX:")) {
            currentData.viewFieldIndex = line.mid(18).trimmed().toInt();
        }
        else if (line.startsWith("WAVE LENGTH INDEX:")) {
            currentData.waveLengthIndex = line.mid(19).trimmed().toInt();
        }
        else if (line.startsWith("WAVE LENGTH:")) {
            currentData.waveLength = line.mid(13).trimmed().toDouble();
        }
        else if (line.startsWith("INDEX")) {
            // 数据表头，下一行开始是数据
            readingData = true;
            // 跳过表头行
            in.readLine();
        }
        else if (readingData && !line.isEmpty()) {
            // 解析数据行
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                currentData.normalizedAperture.append(parts[1].toDouble());
                currentData.aberration.append(parts[2].toDouble());
            }
        }
    }

    // 添加最后一个数据块
    if (currentData.lineIndex != -1) {
        dataList.append(currentData);
        hasData = true;
    }

    file.close();



    QMap<int, QVector<AberrationData>> fieldGroups;
    for (const auto& data : dataList) {
        fieldGroups[data.viewFieldIndex].append(data);
    }
    int cols = fieldGroups.size();
    // 为每个视场创建图表
    
    int col_idx = 0;
    for (auto it = fieldGroups.begin(); it != fieldGroups.end(); ++it) {
        int fieldIndex = it.key();
        const QVector<AberrationData>& fieldData = it.value();

        QMap<QString, QVector<AberrationData>> chartName_grps = groupByChartName(fieldData);


        int row_idx = 0;
 
        // 为每个波长数据添加曲线
        for (auto& key : chartName_grps.keys()) {
            
            QCustomPlot* customPlot = new QCustomPlot();
            customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);


            QVector<AberrationData> grp = chartName_grps.value(key);
            for (auto& wave : grp)
            {
                QCPGraph* graph = customPlot->addGraph();
                graph->setData(wave.normalizedAperture, wave.aberration);

                QString legendName = QString("%1nm").arg(wave.waveLength);//λ=
                graph->setName(legendName);
                graph->setPen(QPen(getColorFromString(wave.lineColor), 1));
                graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
            }

            customPlot->xAxis->setLabel("归一化光阑");
            customPlot->yAxis->setLabel("光线像差 (mm)");
            customPlot->xAxis->setRange(-1.1, 1.1);

            // 自动调整Y轴范围
            customPlot->rescaleAxes();

            // 添加标题
            QString title = QString("%1").arg(key);
            customPlot->plotLayout()->insertRow(0);
            customPlot->plotLayout()->addElement(0, 0,
                new QCPTextElement(customPlot, title, QFont("sans", 12, QFont::Bold)));
            customPlot->plotLayout()->insertRow(1);
            customPlot->plotLayout()->addElement(1, 0, customPlot->legend);
            //customPlot->plotLayout()->addElement(2, 2, customPlot->axisRect());
            

            customPlot->legend->setWrap(5); // 每行最多6个图例项
            customPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
            //// 添加图例
            customPlot->legend->setVisible(true);
            customPlot->legend->setFont(QFont("Helvetica", 8));
            customPlot->legend->setMinimumSize(0, 15);   // 最小高度30
            customPlot->legend->setMaximumSize(10000, 15); // 最大高度30
            //customPlot->legend->setRowStretchFactor(1, 0);


            customPlot->replot();


            // 添加到标签页
            QString tabName = QString("视场 %1").arg(fieldIndex);
            m_mainLayout->addWidget(customPlot, row_idx, col_idx);
            row_idx++;
        }
        col_idx++;
    }


    return true;
}




QColor AberrationWidget::getColorFromString(const QString& colorStr)
{
    if (colorStr == "RGBOrange") return QColor(255, 165, 0);
    else if (colorStr == "RGBYellow") return QColor(255, 255, 0);
    else if (colorStr == "RGBGreen") return QColor(0, 255, 0);
    else if (colorStr == "RGBCyan") return QColor(0, 255, 255);
    else if (colorStr == "RGBRed") return QColor(255, 0, 0);
    else if (colorStr == "RGBBlue") return QColor(0, 0, 255);
    else return QColor(0, 0, 0); // 默认黑色
}



