//#include "DistortionWidget.h"
//#include <QVBoxLayout>
//#include <QFile>
//#include <QTextStream>
//#include <QDebug>
//#include <QRegularExpression>
//#include <cmath>
//
//DistortionGridWidget::DistortionGridWidget(QWidget* parent)
//    : QWidget(parent)
//{
//    // 创建布局和绘图部件
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//
//    m_customPlot = new QCustomPlot(this);
//    layout->addWidget(m_customPlot);
//
//    setupPlot();
//}
//
//void DistortionGridWidget::setupPlot()
//{
//    // 设置图表标题
//    m_customPlot->plotLayout()->insertRow(0);
//    QCPTextElement* title = new QCPTextElement(m_customPlot, "Distortion Grid Visualization", QFont("Arial", 14, QFont::Bold));
//    m_customPlot->plotLayout()->addElement(0, 0, title);
//
//    // 设置坐标轴
//    m_customPlot->xAxis->setLabel("Horizontal FOV (mm)");
//    m_customPlot->yAxis->setLabel("");
//
//
//    // 启用网格
//    //m_customPlot->xAxis->grid()->setVisible(true);
//    //m_customPlot->yAxis->grid()->setVisible(true);
//    //m_customPlot->xAxis->grid()->setSubGridVisible(true);
//    //m_customPlot->yAxis->grid()->setSubGridVisible(true);
//
//    // 设置网格线样式
//    //m_customPlot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::SolidLine));
//    //m_customPlot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::SolidLine));
//    //m_customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
//    //m_customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
//
//    // 设置坐标轴线
//    m_customPlot->xAxis->setBasePen(QPen(Qt::black, 2));
//    m_customPlot->yAxis->setBasePen(QPen(Qt::black, 2));
//
//    // 设置刻度
//    m_customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
//    m_customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
//    m_customPlot->xAxis->setSubTickPen(QPen(Qt::black, 1));
//    m_customPlot->yAxis->setSubTickPen(QPen(Qt::black, 1));
//    // 设置交互功能
//    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//    resize(QSize(640, 640));
//    // 设置等比例缩放
//    //m_customPlot->setAspectRatio(1.0);
//
//    // 设置图例
//    //m_customPlot->legend->setVisible(true);
//    //m_customPlot->legend->setFont(QFont("Helvetica", 10));
//    //m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);
//}
//
//bool DistortionGridWidget::loadDistortionData(const QString& filename)
//{
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qDebug() << "无法打开文件:" << filename;
//        return false;
//    }
//
//    // 清空旧数据
//    m_distortionData.clear();
//    m_gridData.clear();
//
//    QTextStream in(&file);
//    bool dataSection = false;
//    m_maxRadialDistortion = 0.0;
//    m_maxTangentialDistortion = 0.0;
//
//    while (!in.atEnd()) {
//        QString line = in.readLine().trimmed();
//
//        if (line.isEmpty() || line.startsWith("#")) {
//            continue;
//        }
//
//        if (line.contains("Rel X Fld") && line.contains("Real X")) {
//            dataSection = true;
//            continue;
//        }
//
//        if (dataSection) {
//            QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
//
//            if (fields.size() >= 8) {
//                DistortionPoint point;
//                point.relXFld = fields[0].toDouble();
//                point.relYFld = fields[1].toDouble();
//                point.paraxX = fields[2].toDouble();
//                point.paraxY = fields[3].toDouble();
//                point.realX = fields[4].toDouble();
//                point.realY = fields[5].toDouble();
//                point.radDist = fields[6].toDouble();
//                point.tanDist = fields[7].toDouble();
//
//                m_distortionData.append(point);
//
//                // 更新最大畸变值
//                if (std::abs(point.radDist) > std::abs(m_maxRadialDistortion)) {
//                    m_maxRadialDistortion = point.radDist;
//                }
//                if (std::abs(point.tanDist) > std::abs(m_maxTangentialDistortion)) {
//                    m_maxTangentialDistortion = point.tanDist;
//                }
//            }
//        }
//    }
//
//    file.close();
//
//    if (m_distortionData.isEmpty()) {
//        qDebug() << "未找到有效数据";
//        return false;
//    }
//
//    organizeGridData();
//    plotGrids();
//
//    qDebug() << "成功加载" << m_distortionData.size() << "个数据点";
//    qDebug() << "最大径向畸变:" << m_maxRadialDistortion << "%";
//    qDebug() << "最大切向畸变:" << m_maxTangentialDistortion << "%";
//
//    return true;
//}
//
//void DistortionGridWidget::organizeGridData()
//{
//    // 创建11x11网格
//    m_gridData.resize(11);
//    for (int i = 0; i < 11; ++i) {
//        m_gridData[i].resize(11);
//    }
//
//    for (int row = 0; row < 11; ++row) {
//        for (int col = 0; col < 11; ++col) {
//            m_gridData[row][col] = m_distortionData[row * 11 + col];
//        }
//    }
//}
//
//void DistortionGridWidget::plotGrids()
//{
//    m_customPlot->clearGraphs();
//    m_customPlot->clearItems();
//
//    // 绘制理想网格
//    plotIdealGrid();
//
//    // 绘制畸变网格
//    plotDistortedGrid();
//
//    // 设置坐标范围 - 根据数据范围调整
//    m_customPlot->xAxis->setRange(xMin - marginX, xMax + marginX);
//    m_customPlot->yAxis->setRange(yMin - marginY, yMax + marginY);
//    m_customPlot->rescaleAxes();
//    //updateInfoText();
//    m_customPlot->replot();
//}
//
//void DistortionGridWidget::plotIdealGrid()
//{
//     xMin = INFINITY;
//     yMin = INFINITY;
//     xMax = -INFINITY;
//     yMax = -INFINITY;
//    // 绘制理想网格的水平线
//    for (int x = 0; x < 11; ++x) {
//        QVector<double> xCoords, yCoords;
//
//        for (int y = 0; y < 11; ++y) {
//            if (m_gridData[x][y].paraxX > xMax)xMax = m_gridData[x][y].paraxX;
//            if (m_gridData[x][y].paraxY > yMax) yMax = m_gridData[x][y].paraxY;
//            if (m_gridData[x][y].paraxX < xMin)xMin = m_gridData[x][y].paraxX;
//            if (m_gridData[x][y].paraxY < yMin)yMin = m_gridData[x][y].paraxY;
//            xCoords.append(m_gridData[x][y].paraxX);
//            yCoords.append(m_gridData[x][y].paraxY);
//        }
//
//        QCPGraph* graph = m_customPlot->addGraph();
//        graph->setData(xCoords, yCoords);
//        graph->setLineStyle(QCPGraph::lsLine);
//        graph->setPen(QPen(QColor(0, 0, 255, 180), 1.5, Qt::SolidLine));
//    }
//
//    // 绘制理想网格的垂直线
//    for (int y = 0; y < 11; ++y) {
//        QVector<double> xCoords, yCoords;
//
//        for (int x = 0; x < 11; ++x) {
//            xCoords.append(m_gridData[x][y].paraxX);
//            yCoords.append(m_gridData[x][y].paraxY);
//        }
//
//        QCPGraph* graph = m_customPlot->addGraph();
//        graph->setData(xCoords, yCoords);
//        graph->setLineStyle(QCPGraph::lsLine);
//        graph->setPen(QPen(QColor(0, 0, 255, 180), 1.5, Qt::SolidLine));
//    }
//    xRange = (xMax - xMin) / 2;
//    yRange = (yMax - yMin) / 2;
//    marginX = xRange * 0.1;
//    marginY = yRange * 0.1;
//
//
//}
//
//void DistortionGridWidget::plotDistortedGrid()
//{
//    // 绘制畸变网格的水平线
//    for (int x = 0; x < 11; ++x) {
//        QVector<double> xCoords, yCoords;
//
//        for (int y = 0; y < 11; ++y) {
//            xCoords.append(m_gridData[x][y].realX);
//            yCoords.append(m_gridData[x][y].realY);
//        }
//
//        QCPCurve* graph = new QCPCurve(m_customPlot->xAxis, m_customPlot->yAxis);
//        graph->setData(xCoords, yCoords);
//        graph->setPen(QPen(QColor(255, 0, 0, 200), 2, Qt::SolidLine));
//    }
//
//    // 绘制畸变网格的垂直线
//    for (int y = 0; y < 11; ++y) {
//        QVector<double> xCoords, yCoords;
//
//        for (int x = 0; x < 11; ++x) {
//            xCoords.append(m_gridData[x][y].realX);
//            yCoords.append(m_gridData[x][y].realY);
//        }
//
//        QCPCurve* graph = new QCPCurve(m_customPlot->xAxis, m_customPlot->yAxis);
//        //m_customPlot->addGraph(graph);
//        graph->setData(xCoords, yCoords);
//        graph->setPen(QPen(QColor(255, 0, 0, 200), 2, Qt::SolidLine));
//    }
//}
//
//void DistortionGridWidget::updateInfoText()
//{
//    QCPItemText* infoText = new QCPItemText(m_customPlot);
//    infoText->position->setType(QCPItemPosition::ptAxisRectRatio);
//    infoText->position->setCoords(0.02, 0.98);
//
//    QString info = QString("Distortion Analysis:\n"
//        "Max Radial Dist: %1%\n"
//        "Max Tangential Dist: %2%\n"
//        "Data Points: %3")
//        .arg(m_maxRadialDistortion, 0, 'f', 2)
//        .arg(m_maxTangentialDistortion, 0, 'f', 2)
//        .arg(m_distortionData.size());
//
//    infoText->setText(info);
//    infoText->setFont(QFont("Arial", 9));
//    infoText->setPadding(QMargins(8, 8, 8, 8));
//    infoText->setBrush(QBrush(QColor(255, 255, 255, 230)));
//    infoText->setPen(QPen(Qt::black));
//    infoText->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
//}
#include "DistortionWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QPainter>
#include <cmath>
#include <limits>
#include <algorithm>
#include <qwt_text.h>
DistortionGridWidget::DistortionGridWidget(QWidget* parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
{
    // 创建布局和绘图部件
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_plot = new QwtPlot(this);
    layout->addWidget(m_plot);

    setupPlot();
    setupInteractions();

    setMinimumSize(640, 640);
}

DistortionGridWidget::~DistortionGridWidget()
{
    clearAllCurves();
}

void DistortionGridWidget::setupPlot()
{
    // 设置画布背景
    m_plot->setCanvasBackground(Qt::white);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);

    // 设置图表标题
    QwtText title("Distortion Grid Visualization");
    title.setFont(QFont("Arial", 14, QFont::Bold));
    m_plot->setTitle(title);

    // 设置坐标轴标签
    m_plot->setAxisTitle(QwtPlot::xBottom, "Horizontal FOV (mm)");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Vertical FOV (mm)");

    // 设置坐标轴线
    m_plot->setAxisMaxMajor(QwtPlot::xBottom, 10);
    m_plot->setAxisMaxMinor(QwtPlot::xBottom, 5);
    m_plot->setAxisMaxMajor(QwtPlot::yLeft, 10);
    m_plot->setAxisMaxMinor(QwtPlot::yLeft, 5);

    // 添加网格
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableY(true);
    grid->enableXMin(true);
    grid->enableYMin(true);

    // 设置主网格线样式
    QPen majorGridPen(QColor(200, 200, 200), 1, Qt::SolidLine);
    grid->setMajorPen(majorGridPen);

    // 设置次网格线样式
    QPen minorGridPen(QColor(220, 220, 220), 1, Qt::DotLine);
    grid->setMinorPen(minorGridPen);

    grid->attach(m_plot);

    // 设置坐标轴样式
    QPen axisPen(Qt::black, 2);
    //m_plot->setAxisPen(QwtPlot::xBottom, axisPen);
    //m_plot->setAxisPen(QwtPlot::yLeft, axisPen);

    // 设置刻度样式
    QPen tickPen(Qt::black, 1);
    //m_plot->setAxisPen(QwtPlot::xBottom, tickPen, true);
    //m_plot->setAxisPen(QwtPlot::yLeft, tickPen, true);
}

void DistortionGridWidget::setupInteractions()
{
    // 创建拖拽功能 (对应 QCP::iRangeDrag)
    m_panner = new QwtPlotPanner(m_plot->canvas());
    m_panner->setMouseButton(Qt::LeftButton);

    // 创建缩放功能 (对应 QCP::iRangeZoom)
    m_magnifier = new QwtPlotMagnifier(m_plot->canvas());
    m_magnifier->setMouseButton(Qt::RightButton, Qt::ControlModifier);
    m_magnifier->setWheelFactor(1.1);

    // 启用抗锯齿
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);
    //m_plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
}

void DistortionGridWidget::clearAllCurves()
{
    // 清理理想网格曲线
    for (auto curve : m_idealCurves) {
        curve->detach();
        delete curve;
    }
    m_idealCurves.clear();

    // 清理畸变网格曲线
    for (auto curve : m_distortedCurves) {
        curve->detach();
        delete curve;
    }
    m_distortedCurves.clear();
}

bool DistortionGridWidget::loadDistortionData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filename;
        return false;
    }

    // 清空旧数据
    m_distortionData.clear();
    m_gridData.clear();
    clearAllCurves();

    QTextStream in(&file);
    bool dataSection = false;
    m_maxRadialDistortion = 0.0;
    m_maxTangentialDistortion = 0.0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        if (line.contains("Rel X Fld") && line.contains("Real X")) {
            dataSection = true;
            continue;
        }

        if (dataSection) {
            QStringList fields = line.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);

            if (fields.size() >= 8) {
                DistortionPoint point;
                point.relXFld = fields[0].toDouble();
                point.relYFld = fields[1].toDouble();
                point.paraxX = fields[2].toDouble();
                point.paraxY = fields[3].toDouble();
                point.realX = fields[4].toDouble();
                point.realY = fields[5].toDouble();
                point.radDist = fields[6].toDouble();
                point.tanDist = fields[7].toDouble();

                m_distortionData.append(point);

                // 更新最大畸变值
                if (std::abs(point.radDist) > std::abs(m_maxRadialDistortion)) {
                    m_maxRadialDistortion = point.radDist;
                }
                if (std::abs(point.tanDist) > std::abs(m_maxTangentialDistortion)) {
                    m_maxTangentialDistortion = point.tanDist;
                }
            }
        }
    }

    file.close();

    if (m_distortionData.isEmpty()) {
        qDebug() << "未找到有效数据";
        return false;
    }

    organizeGridData();
    plotGrids();

    qDebug() << "成功加载" << m_distortionData.size() << "个数据点";
    qDebug() << "最大径向畸变:" << m_maxRadialDistortion << "%";
    qDebug() << "最大切向畸变:" << m_maxTangentialDistortion << "%";

    return true;
}

void DistortionGridWidget::organizeGridData()
{
    // 创建11x11网格
    m_gridData.resize(11);
    for (int i = 0; i < 11; ++i) {
        m_gridData[i].resize(11);
    }

    for (int row = 0; row < 11; ++row) {
        for (int col = 0; col < 11; ++col) {
            m_gridData[row][col] = m_distortionData[row * 11 + col];
        }
    }
}

void DistortionGridWidget::plotGrids()
{
    clearAllCurves();

    // 绘制理想网格
    plotIdealGrid();

    // 绘制畸变网格
    plotDistortedGrid();

    // 设置坐标范围
    m_plot->setAxisScale(QwtPlot::xBottom, xMin - marginX, xMax + marginX);
    m_plot->setAxisScale(QwtPlot::yLeft, yMin - marginY, yMax + marginY);

    updateInfoText();

    // 重新绘制
    m_plot->replot();
}

void DistortionGridWidget::plotIdealGrid()
{
    // 重置范围变量
    xMin = std::numeric_limits<double>::max();
    yMin = std::numeric_limits<double>::max();
    xMax = std::numeric_limits<double>::lowest();
    yMax = std::numeric_limits<double>::lowest();

    // 绘制理想网格的水平线
    for (int x = 0; x < 11; ++x) {
        QVector<QPointF> points;
        points.reserve(11);

        for (int y = 0; y < 11; ++y) {
            double px = m_gridData[x][y].paraxX;
            double py = m_gridData[x][y].paraxY;

            points.append(QPointF(px, py));

            // 更新范围
            xMin = std::min(xMin, px);
            yMin = std::min(yMin, py);
            xMax = std::max(xMax, px);
            yMax = std::max(yMax, py);
        }

        QwtPlotCurve* curve = new QwtPlotCurve();
        curve->setSamples(points);
        curve->setPen(QPen(QColor(0, 0, 255, 180), 1.5, Qt::SolidLine));
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        curve->setItemAttribute(QwtPlotItem::Legend, false);
        curve->attach(m_plot);

        m_idealCurves.append(curve);
    }

    // 绘制理想网格的垂直线
    for (int y = 0; y < 11; ++y) {
        QVector<QPointF> points;
        points.reserve(11);

        for (int x = 0; x < 11; ++x) {
            double px = m_gridData[x][y].paraxX;
            double py = m_gridData[x][y].paraxY;

            points.append(QPointF(px, py));

            // 更新范围（虽然已经更新过，但为了安全再次更新）
            xMin = std::min(xMin, px);
            yMin = std::min(yMin, py);
            xMax = std::max(xMax, px);
            yMax = std::max(yMax, py);
        }

        QwtPlotCurve* curve = new QwtPlotCurve();
        curve->setSamples(points);
        curve->setPen(QPen(QColor(0, 0, 255, 180), 1.5, Qt::SolidLine));
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        curve->setItemAttribute(QwtPlotItem::Legend, false);
        curve->attach(m_plot);

        m_idealCurves.append(curve);
    }

    // 计算边距
    xRange = (xMax - xMin) / 2;
    yRange = (yMax - yMin) / 2;
    marginX = xRange * 0.1;
    marginY = yRange * 0.1;
}

void DistortionGridWidget::plotDistortedGrid()
{
    // 绘制畸变网格的水平线
    for (int x = 0; x < 11; ++x) {
        QVector<QPointF> points;
        points.reserve(11);

        for (int y = 0; y < 11; ++y) {
            double rx = m_gridData[x][y].realX;
            double ry = m_gridData[x][y].realY;

            points.append(QPointF(rx, ry));

            // 更新范围以包含畸变数据
            xMin = std::min(xMin, rx);
            yMin = std::min(yMin, ry);
            xMax = std::max(xMax, rx);
            yMax = std::max(yMax, ry);
        }

        QwtPlotCurve* curve = new QwtPlotCurve();
        curve->setSamples(points);
        curve->setPen(QPen(QColor(255, 0, 0, 200), 2.0, Qt::SolidLine));
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        curve->setItemAttribute(QwtPlotItem::Legend, false);
        curve->attach(m_plot);

        m_distortedCurves.append(curve);
    }

    // 绘制畸变网格的垂直线
    for (int y = 0; y < 11; ++y) {
        QVector<QPointF> points;
        points.reserve(11);

        for (int x = 0; x < 11; ++x) {
            double rx = m_gridData[x][y].realX;
            double ry = m_gridData[x][y].realY;

            points.append(QPointF(rx, ry));

            // 更新范围
            xMin = std::min(xMin, rx);
            yMin = std::min(yMin, ry);
            xMax = std::max(xMax, rx);
            yMax = std::max(yMax, ry);
        }

        QwtPlotCurve* curve = new QwtPlotCurve();
        curve->setSamples(points);
        curve->setPen(QPen(QColor(255, 0, 0, 200), 2.0, Qt::SolidLine));
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);
        curve->setItemAttribute(QwtPlotItem::Legend, false);
        curve->attach(m_plot);

        m_distortedCurves.append(curve);
    }

    // 重新计算边距，考虑畸变数据
    xRange = (xMax - xMin) / 2;
    yRange = (yMax - yMin) / 2;
    marginX = xRange * 0.1;
    marginY = yRange * 0.1;
}

void DistortionGridWidget::updateInfoText()
{
    // 创建信息文本标签
    QwtText infoText;
    QString info = QString("Distortion Analysis:\n"
        "Max Radial Dist: %1%\n"
        "Max Tangential Dist: %2%\n"
        "Data Points: %3")
        .arg(m_maxRadialDistortion, 0, 'f', 2)
        .arg(m_maxTangentialDistortion, 0, 'f', 2)
        .arg(m_distortionData.size());

    infoText.setText(info);
    infoText.setFont(QFont("Arial", 9));
    infoText.setColor(Qt::black);
    infoText.setBackgroundBrush(QBrush(QColor(255, 255, 255, 230)));
    infoText.setBorderRadius(4);
    infoText.setBorderPen(QPen(Qt::black));

    // 使用QwtPlotMarker来显示文本
    //QwtPlotMarker* marker = new QwtPlotMarker();
    //marker->setValue(xMin + marginX, yMax - marginY);  // 左上角位置
    //marker->setLabel(infoText);
    //marker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    //marker->setSpacing(8);
    //marker->attach(m_plot);
}