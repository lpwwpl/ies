//#include "AberrationWidget.h"
//#include <QFile>
//#include <QTextStream>
//#include <QDebug>
//#include <QApplication>
//
//AberrationWidget::AberrationWidget(QWidget* parent)
//    : QWidget(parent)
//{
//    setupUI();
//}
//
//
//
//void AberrationWidget::setupUI()
//{
//    m_mainLayout = new QGridLayout(this);
//    m_mainLayout->setSpacing(10);
//    m_mainLayout->setContentsMargins(10, 10, 10, 10);
//
//    setLayout(m_mainLayout);
//}
//
//bool AberrationWidget::loadDataFromFile(const QString& filename)
//{
//    QFile file(filename);
//    // 解析数据
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, "错误", "无法打开文件: " + filename);
//        return false;
//    }
//
//
//    dataList.clear();
//
//    QTextStream in(&file);
//    QString line;
//    AberrationData currentData;
//    bool readingData = false;
//    bool hasData = false;
//
//    // 初始化当前数据
//    currentData.lineIndex = -1;
//
//    while (!in.atEnd()) {
//        line = in.readLine().trimmed();
//
//        if (line.startsWith("# CHART NAME:")) {
//            // 如果是新数据块开始，保存前一个数据块
//            if (currentData.lineIndex != -1) {
//                dataList.append(currentData);
//                hasData = true;
//            }
//
//            // 初始化新数据块
//            currentData = AberrationData();
//            currentData.chartName = line.mid(14).trimmed();
//            readingData = false;
//        }
//        else if (line.startsWith("LINE INDEX:")) {
//            currentData.lineIndex = line.mid(12).trimmed().toInt();
//        }
//        else if (line.startsWith("LINE LABEL:")) {
//            currentData.lineLabel = line.mid(12).trimmed();
//        }
//        else if (line.startsWith("LINE COLOR:")) {
//            currentData.lineColor = line.mid(12).trimmed();
//        }
//        else if (line.startsWith("VIEW FIELD INDEX:")) {
//            currentData.viewFieldIndex = line.mid(18).trimmed().toInt();
//        }
//        else if (line.startsWith("WAVE LENGTH INDEX:")) {
//            currentData.waveLengthIndex = line.mid(19).trimmed().toInt();
//        }
//        else if (line.startsWith("WAVE LENGTH:")) {
//            currentData.waveLength = line.mid(13).trimmed().toDouble();
//        }
//        else if (line.startsWith("INDEX")) {
//            // 数据表头，下一行开始是数据
//            readingData = true;
//            // 跳过表头行
//            in.readLine();
//        }
//        else if (readingData && !line.isEmpty()) {
//            // 解析数据行
//            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
//            if (parts.size() >= 3) {
//                currentData.normalizedAperture.append(parts[1].toDouble());
//                currentData.aberration.append(parts[2].toDouble());
//            }
//        }
//    }
//
//    // 添加最后一个数据块
//    if (currentData.lineIndex != -1) {
//        dataList.append(currentData);
//        hasData = true;
//    }
//
//    file.close();
//
//
//
//    QMap<int, QVector<AberrationData>> fieldGroups;
//    for (const auto& data : dataList) {
//        fieldGroups[data.viewFieldIndex].append(data);
//    }
//    int cols = fieldGroups.size();
//    // 为每个视场创建图表
//    
//    int col_idx = 0;
//    for (auto it = fieldGroups.begin(); it != fieldGroups.end(); ++it) {
//        int fieldIndex = it.key();
//        const QVector<AberrationData>& fieldData = it.value();
//
//        QMap<QString, QVector<AberrationData>> chartName_grps = groupByChartName(fieldData);
//
//
//        int row_idx = 0;
// 
//        // 为每个波长数据添加曲线
//        for (auto& key : chartName_grps.keys()) {
//            
//            QCustomPlot* customPlot = new QCustomPlot();
//            customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
//
//
//            QVector<AberrationData> grp = chartName_grps.value(key);
//            for (auto& wave : grp)
//            {
//                QCPGraph* graph = customPlot->addGraph();
//                graph->setData(wave.normalizedAperture, wave.aberration);
//
//                QString legendName = QString("%1nm").arg(wave.waveLength);//λ=
//                graph->setName(legendName);
//                graph->setPen(QPen(getColorFromString(wave.lineColor), 1));
//                graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
//            }
//
//            customPlot->xAxis->setLabel("归一化光阑");
//            customPlot->yAxis->setLabel("光线像差 (mm)");
//            customPlot->xAxis->setRange(-1.1, 1.1);
//
//            // 自动调整Y轴范围
//            customPlot->rescaleAxes();
//
//            // 添加标题
//            QString title = QString("%1").arg(key);
//            customPlot->plotLayout()->insertRow(0);
//            customPlot->plotLayout()->addElement(0, 0,
//                new QCPTextElement(customPlot, title, QFont("sans", 12, QFont::Bold)));
//            customPlot->plotLayout()->insertRow(1);
//            customPlot->plotLayout()->addElement(1, 0, customPlot->legend);
//            //customPlot->plotLayout()->addElement(2, 2, customPlot->axisRect());
//            
//
//            customPlot->legend->setWrap(5); // 每行最多6个图例项
//            customPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
//            //// 添加图例
//            customPlot->legend->setVisible(true);
//            customPlot->legend->setFont(QFont("Helvetica", 8));
//            customPlot->legend->setMinimumSize(0, 15);   // 最小高度30
//            customPlot->legend->setMaximumSize(10000, 15); // 最大高度30
//            //customPlot->legend->setRowStretchFactor(1, 0);
//
//
//            customPlot->replot();
//
//
//            // 添加到标签页
//            QString tabName = QString("视场 %1").arg(fieldIndex);
//            m_mainLayout->addWidget(customPlot, row_idx, col_idx);
//            row_idx++;
//        }
//        col_idx++;
//    }
//
//
//    return true;
//}
//
//
//
//
//QColor AberrationWidget::getColorFromString(const QString& colorStr)
//{
//    if (colorStr == "RGBOrange") return QColor(255, 165, 0);
//    else if (colorStr == "RGBYellow") return QColor(255, 255, 0);
//    else if (colorStr == "RGBGreen") return QColor(0, 255, 0);
//    else if (colorStr == "RGBCyan") return QColor(0, 255, 255);
//    else if (colorStr == "RGBRed") return QColor(255, 0, 0);
//    else if (colorStr == "RGBBlue") return QColor(0, 0, 255);
//    else return QColor(0, 0, 0); // 默认黑色
//}
//
//
//

#include "AberrationWidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QFrame>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>

AberrationWidget::AberrationWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
{
    setupUI();
}

AberrationWidget::~AberrationWidget()
{
    clearPlots();
}

void AberrationWidget::setupUI()
{
    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    setLayout(m_mainLayout);
}

void AberrationWidget::clearPlots()
{
    // 清除所有曲线
    qDeleteAll(m_curves);
    m_curves.clear();

    // 清除所有图表
    qDeleteAll(m_plots);
    m_plots.clear();

    // 清除布局中的部件
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
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
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
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

    if (!hasData) {
        QMessageBox::warning(this, "错误", "文件中没有有效数据");
        return false;
    }

    // 清除之前的图表
    clearPlots();

    // 按视场分组
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

        // 按chartName分组
        QMap<QString, QVector<AberrationData>> chartName_grps = groupByChartName(fieldData);

        int row_idx = 0;

        // 为每个chartName创建图表
        for (auto& key : chartName_grps.keys()) {
            // 创建QwtPlot
            QwtPlot* plot = new QwtPlot();
            plot->setCanvasBackground(Qt::white);
            plot->setMinimumSize(400, 300);

            // 启用抗锯齿和缓冲
            //plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
            //plot->canvas()->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);

            // 设置标题
            plot->setTitle(QString("%1 - 视场 %2").arg(key).arg(fieldIndex));

            // 设置坐标轴标签
            plot->setAxisTitle(QwtPlot::xBottom, "归一化光阑");
            plot->setAxisTitle(QwtPlot::yLeft, "光线像差 (mm)");

            // 设置X轴范围
            plot->setAxisScale(QwtPlot::xBottom, -1.1, 1.1);

            // 添加网格
            QwtPlotGrid* grid = new QwtPlotGrid();
            grid->enableX(true);
            grid->enableY(true);
            grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
            grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
            grid->attach(plot);

            QVector<AberrationData> grp = chartName_grps.value(key);
            double minY = std::numeric_limits<double>::max();
            double maxY = std::numeric_limits<double>::lowest();

            // 为每个波长数据添加曲线
            for (auto& wave : grp) {
                QwtPlotCurve* curve = new QwtPlotCurve();

                // 设置曲线名称（图例中显示）
                QString legendName = QString("%1nm").arg(wave.waveLength);
                curve->setTitle(legendName);

                // 设置曲线数据
                curve->setSamples(wave.normalizedAperture, wave.aberration);

                // 设置曲线颜色
                QColor curveColor = getColorFromString(wave.lineColor);
                curve->setPen(QPen(curveColor, 2));
                curve->setRenderHint(QwtPlotItem::RenderAntialiased);

                // 设置符号（圆圈）
                QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse);
                symbol->setSize(5);
                symbol->setColor(curveColor);
                symbol->setPen(QPen(curveColor, 1));
                curve->setSymbol(symbol);

                // 设置曲线属性
                curve->setItemAttribute(QwtPlotItem::Legend, true);
                curve->setItemAttribute(QwtPlotItem::AutoScale, true);

                // 附加到图表
                curve->attach(plot);

                // 记录曲线对象
                m_curves.append(curve);

                // 更新Y轴范围
                for (double val : wave.aberration) {
                    if (val < minY) minY = val;
                    if (val > maxY) maxY = val;
                }
            }

            // 设置Y轴范围（留一些边距）
            double margin = (maxY - minY) * 0.1;
            if (margin == 0) margin = 0.1; // 防止所有值相同的情况
            plot->setAxisScale(QwtPlot::yLeft, minY - margin, maxY + margin);

            // 添加图例（可点击）
            QwtLegend* legend = new QwtLegend();
            legend->setDefaultItemMode(QwtLegendData::Checkable); // 使图例项可点击
            plot->insertLegend(legend, QwtPlot::BottomLegend);

            // 设置图例位置和最大高度
            plot->plotLayout()->setLegendPosition(QwtPlot::BottomLegend);

            // 连接图例点击信号
            connect(legend, SIGNAL(clicked(const QVariant&, int)),
                this, SLOT(onLegendClicked(const QVariant&)));

            // 设置交互功能（拖拽和缩放）
            setupPlotInteractions(plot);

            // 自动调整布局
            plot->updateLayout();

            // 重绘图表
            plot->replot();

            // 添加到网格布局
            m_mainLayout->addWidget(plot, row_idx, col_idx);
            m_plots.append(plot);

            row_idx++;
        }
        col_idx++;
    }

    return true;
}

void AberrationWidget::setupPlotInteractions(QwtPlot* plot)
{
    if (!plot) return;

    // 1. 拖拽功能 - 对应 QCP::iRangeDrag
    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MiddleButton);  // 中键拖拽（更符合习惯）
    panner->setEnabled(true);

    // 2. 缩放功能 - 对应 QCP::iRangeZoom
    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setMouseButton(Qt::NoButton);  // 不使用鼠标按钮，仅用滚轮
    magnifier->setWheelFactor(1.1);  // 滚轮缩放因子
    magnifier->setZoomInKey(Qt::Key_Plus, Qt::ControlModifier);
    magnifier->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);
    magnifier->setEnabled(true);

    // 3. 右键菜单用于重置视图（可选功能）
    plot->canvas()->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AberrationWidget::onLegendClicked(const QVariant& itemInfo)
{
    // 获取发送信号的图例
    QwtLegend* legend = qobject_cast<QwtLegend*>(sender());
    if (!legend) return;

    // 获取对应的plot
    QwtPlot* plot = nullptr;
    for (auto* p : m_plots) {
        if (p->legend() == legend) {
            plot = p;
            break;
        }
    }

    if (!plot) return;

    // 找到对应的曲线
    QwtPlotItem* item = plot->infoToItem(itemInfo);
    if (item && item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
        QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);

        // 切换曲线的可见性
        bool visible = !curve->isVisible();
        curve->setVisible(visible);

        // 更新图例显示
        QList<QWidget*> legendWidgets = legend->legendWidgets(itemInfo);
        for (QWidget* w : legendWidgets) {
            if (QwtLegendLabel* label = qobject_cast<QwtLegendLabel*>(w)) {
                label->setChecked(visible);
            }
        }

        // 重新调整坐标轴范围
        if (visible) {
            // 如果显示曲线，重新计算范围
            plot->setAutoReplot(false);
            plot->updateAxes();
            plot->setAutoReplot(true);
        }

        plot->replot();
    }
}

QColor AberrationWidget::getColorFromString(const QString& colorStr)
{
    if (colorStr == "RGBOrange") return QColor(255, 165, 0);
    else if (colorStr == "RGBYellow") return QColor(255, 255, 0);
    else if (colorStr == "RGBGreen") return QColor(0, 255, 0);
    else if (colorStr == "RGBCyan") return QColor(0, 255, 255);
    else if (colorStr == "RGBRed") return QColor(255, 0, 0);
    else if (colorStr == "RGBBlue") return QColor(0, 0, 255);
    else if (colorStr.contains("nm")) {
        // 处理波长值，根据波长返回不同颜色
        QString numStr = colorStr;
        numStr.replace("nm", "").trimmed();
        bool ok;
        double wavelength = numStr.toDouble(&ok);
        if (ok) {
            // 根据波长返回颜色（简化版）
            if (wavelength < 480) return QColor(0, 0, 255);    // 蓝色
            else if (wavelength < 510) return QColor(0, 255, 255); // 青色
            else if (wavelength < 550) return QColor(0, 255, 0);   // 绿色
            else if (wavelength < 590) return QColor(255, 255, 0); // 黄色
            else if (wavelength < 630) return QColor(255, 165, 0); // 橙色
            else return QColor(255, 0, 0); // 红色
        }
    }
    return QColor(0, 0, 0); // 默认黑色
}
