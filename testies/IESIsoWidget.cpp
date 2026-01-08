//#include "IESIsoWidget.h"
//#include "contours.h"
//#include <QDebug>
//
//#include "IESLoader.h"
//
//IESIsoWidget::IESIsoWidget()
//{
//    Init();
//}
//
//IESIsoWidget::~IESIsoWidget()
//{
//
//}
//
//void IESIsoWidget::Init()
//{
//    calculationWidth = 20;
//    fixtureX = 0;
//    fixtureY = 0;
//    fixtureZ = 0;
//    gridSpacing = 0.1;
//    m_levelSize = 5;
//    //setupColorSchemes();
//    m_colorScale= new QCPColorScale(this);
//    
//
//    addLayer("gridLayer", nullptr, QCustomPlot::limAbove); // 先添加到默认图层上方
//    layer("gridLayer")->setMode(QCPLayer::lmBuffered); // 关键：设置为缓冲模式
//
//    m_margin = new QCPMarginGroup(this);
//    axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, m_margin);
//    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
//
//    plotLayout()->addElement(0, 1, m_colorScale);
//}
//
////void IESIsoWidget::setupColorSchemes()
////{
////    // 定义多种颜色方案
////    colorSchemes.clear();
////
////    // 方案1: 彩虹色
////    ColorScheme rainbow;
////    rainbow.name = "彩虹色";
////    rainbow.colors = {
////        QColor(148, 0, 211),   // 紫色
////        QColor(75, 0, 130),    // 靛蓝色
////        QColor(0, 0, 255),     // 蓝色
////        QColor(0, 255, 0),     // 绿色
////        QColor(255, 255, 0),   // 黄色
////        QColor(255, 127, 0),   // 橙色
////        QColor(255, 0, 0)      // 红色
////    };
////    colorSchemes.push_back(rainbow);
////
////    // 方案2: 热力图
////    ColorScheme heatmap;
////    heatmap.name = "热力图";
////    heatmap.colors = {
////        QColor(0, 0, 0),       // 黑色
////        QColor(128, 0, 0),     // 深红色
////        QColor(255, 0, 0),     // 红色
////        QColor(255, 128, 0),   // 橙色
////        QColor(255, 255, 0),   // 黄色
////        QColor(255, 255, 128), // 浅黄色
////        QColor(255, 255, 255)  // 白色
////    };
////    colorSchemes.push_back(heatmap);
////
////    // 方案3: 冷色调
////    ColorScheme cool;
////    cool.name = "冷色调";
////    cool.colors = {
////        QColor(255, 255, 255), // 白色
////        QColor(200, 230, 255), // 浅蓝色
////        QColor(150, 200, 255), // 淡蓝色
////        QColor(100, 150, 255), // 蓝色
////        QColor(50, 100, 200),  // 中蓝色
////        QColor(0, 50, 150),    // 深蓝色
////        QColor(0, 0, 100)      // 海军蓝
////    };
////    colorSchemes.push_back(cool);
////
////    // 方案4: 地形图
////    ColorScheme terrain;
////    terrain.name = "地形图";
////    terrain.colors = {
////        QColor(0, 0, 70),      // 深蓝色
////        QColor(0, 0, 255),     // 蓝色
////        QColor(0, 128, 255),   // 浅蓝色
////        QColor(240, 240, 64),  // 黄色
////        QColor(32, 160, 0),    // 绿色
////        QColor(0, 96, 0),      // 深绿色
////        QColor(128, 128, 128)  // 灰色
////    };
////    colorSchemes.push_back(terrain);
////
////    // 方案5: 自定义鲜艳色
////    ColorScheme vibrant;
////    vibrant.name = "鲜艳色";
////    vibrant.colors = {
////        QColor(255, 0, 0),     // 红色
////        QColor(255, 128, 0),   // 橙色
////        QColor(255, 255, 0),   // 黄色
////        QColor(0, 255, 0),     // 绿色
////        QColor(0, 255, 255),   // 青色
////        QColor(0, 128, 255),   // 蓝色
////        QColor(128, 0, 255)    // 紫色
////    };
////    colorSchemes.push_back(vibrant);
////
////    //// 添加到下拉框
////    //colorSchemeComboBox->clear();
////    //for (const auto& scheme : colorSchemes) {
////    //    colorSchemeComboBox->addItem(scheme.name);
////    //}
////}
//
//void IESIsoWidget::updatePlot()
//{
//    clearPlottables();
//    clearItems();
//    isoluxCurves.clear();
//    illuminanceLevels.clear();
//
//
//    updateColorMapData();
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//
//    // 为每个等值线创建IsoluxCurve
//    std::vector<QColor> colors;
//    double dist = maxIlluminance - minIlluminance;
// 
//    double step = dist/m_levelSize;
//    for (int i = 1; i < m_levelSize; i++)
//    {
//        illuminanceLevels.push_back(minIlluminance + i* step);
//    }
//
//    //if (m_levelSize <= static_cast<int>(src_colors.size())) {
//    //    // 如果需要的颜色数量小于等于方案中的颜色数量，直接取前面的颜色
//    //    for (int i = 0; i < m_levelSize; ++i) {
//    //        colors.push_back(src_colors[i]);
//    //    }
//    //}
//    //else 
//    //{
//    //    int baseColors = static_cast<int>(src_colors.size());
//    //    for (int i = 0; i < m_levelSize; ++i)
//    //    {
//    //        double t = static_cast<double>(i) / (m_levelSize - 1);
//    //        double baseIndex = t * (baseColors - 1);
//    //        int index1 = static_cast<int>(std::floor(baseIndex));
//    //        int index2 = std::min(index1 + 1, baseColors - 1);
//    //        double alpha = baseIndex - index1;
//
//    //        QColor color1 = src_colors[index1];
//    //        QColor color2 = src_colors[index2];
//
//    //        int r = static_cast<int>(color1.red() * (1 - alpha) + color2.red() * alpha);
//    //        int g = static_cast<int>(color1.green() * (1 - alpha) + color2.green() * alpha);
//    //        int b = static_cast<int>(color1.blue() * (1 - alpha) + color2.blue() * alpha);
//
//    //        colors.push_back(QColor(r, g, b));
//    //    }
//    //}
//
//
//    MarchingSquares contourPlot(illuminanceGriddata, -halfRange, halfRange, -halfRange, halfRange);
//
//    vector<MarchingSquares::levelPaths> contours = contourPlot.mkIsos(illuminanceLevels);
//
//    QCPCurve* base;
//    for (int l = 0; l < contours.size(); l++) {
//        for (int j = 0; j < contours[l].size(); j++) {
//            base = new QCPCurve(xAxis, yAxis);
//            QVector<double> x, y;
//            //base->setPen(QPen(colors[l]));
//            base->setPen(QPen(Qt::black));
//            //base->setBrush(QBrush(colors[l]));
//            if (contours[l][j].size() > 4) 
//            {
//                for (int i = 0; i < contours[l][j].size(); i++)
//                {
//                    x.push_back(contours[l][j][i].x());
//                    y.push_back(contours[l][j][i].y());
//                }
//            }
//            base->setData(y, x);
//        }
//    }
//
//    // 设置坐标轴范围
//    xAxis->setRange(-halfRange, halfRange);
//    yAxis->setRange(-halfRange, halfRange);
//
//    //显示网格线
//    if (m_bUseGrid)
//    {
//        xAxis->grid()->setLayer("gridLayer");
//        yAxis->grid()->setLayer("gridLayer");
//
//        xAxis->grid()->setSubGridVisible(true); // 确保子网格线可见
//        yAxis->grid()->setSubGridVisible(true);
//        xAxis->grid()->setLayer("gridLayer");
//        yAxis->grid()->setLayer("gridLayer");
//
//        moveLayer(layer("gridLayer"), layer("main"));
//    }
//    else
//    {
//        moveLayer(layer("main"),layer("gridLayer"));
//        //removeLayer(layer("gridLayer"));
//    }
//    this->replot();
//}
//
//void IESIsoWidget::calculateXZ_PlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double x = -halfRange + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double z = -halfRange + j * gridSpacing;
//            double y = 0;  // XY平面的固定高度
//
//            double illuminance = calculateIlluminanceAtPoint_(x, 0, z);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.z = z;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//
//}
//void IESIsoWidget::calculateYZ_PlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double y = -halfRange + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double z = -halfRange + j * gridSpacing;
//            double x = 0;  // YZ平面的固定X位置
//
//            double illuminance = calculateIlluminanceAtPoint_(0, y, z);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.z = z;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//}
//void IESIsoWidget::calculateXY_PlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfWidth = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double x = -halfWidth + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double y = -halfWidth + j * gridSpacing;
//
//            double illuminance = calculateIlluminanceAtPoint_(x, y, 0);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//}
//void IESIsoWidget::calculateXZPlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double x = -halfRange + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double z = -halfRange + j * gridSpacing;
//            double y = 0;  // XY平面的固定高度
//
//            double illuminance = calculateIlluminanceAtPoint_(x,0,  z);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.z = z;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//}
//void IESIsoWidget::calculateYZPlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double y = -halfRange + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double z = -halfRange + j * gridSpacing;
//            double x = 0;  // YZ平面的固定X位置
//
//            double illuminance = calculateIlluminanceAtPoint_(0,y, z);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.z = z;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//}
//
//double IESIsoWidget::calculateIlluminanceAtPoint(double x, double y, double z)
//{
//    // 计算距离
//    double dx = x - fixtureX;
//    double dy = y - fixtureY;
//    double dz = z - fixtureZ;
//    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);
//
//    if (totalDistance == 0) return 0;
//
//    // 计算垂直角度 (从灯具向下为正)
//    double verticalAngle = acos(dz / totalDistance) * 180.0 / M_PI;  // 注意符号
//
//    // 计算水平角度
//    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;
//    if (horizontalAngle < 0) horizontalAngle += 360;
//
//    // 获取光强值
//    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);
//
//    // 计算照度 (距离平方反比定律 + 余弦定律)
//    double cosIncidence = -dz / totalDistance;  // 入射角余弦
//    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;
//
//    return std::max(0.0, illuminance);
//}
//double IESIsoWidget::calculateIlluminanceAtPoint_(double x, double y, double z)
//{
//    // 计算距离
//    double dx = x - fixtureX;
//    double dy = y - fixtureY;
//    double dz = z - fixtureZ;
//    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);
//
//    if (totalDistance == 0) return 0;
//
//    // 计算垂直角度 (从灯具向下为正)
//    double verticalAngle = acos(-dz / totalDistance) * 180.0 / M_PI;  // 注意符号
// 
//    // 计算水平角度
//    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;
//    if (horizontalAngle < 0) horizontalAngle += 360;
//
//    // 获取光强值
//    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);
//
//    // 计算照度 (距离平方反比定律 + 余弦定律)
//    double cosIncidence = -dz / totalDistance;  // 入射角余弦
//    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;
//
//    return std::max(0.0, illuminance);
//}
//
//void IESIsoWidget::calculateXYPlaneIlluminance()
//{
//    illuminanceGrid.clear();
//    illuminanceGriddata.clear();
//    maxIlluminance = 0;
//    minIlluminance = 1e9;
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfWidth = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//
//    for (int i = 0; i <= gridSize; ++i) {
//        std::vector<double> oneLine;
//        double x = -halfWidth + i * gridSpacing;
//        for (int j = 0; j <= gridSize; ++j) {
//            double y = -halfWidth + j * gridSpacing;
//
//            double illuminance = calculateIlluminanceAtPoint(x, y,0);
//
//            IlluminancePoint point;
//            point.x = x;
//            point.y = y;
//            point.illuminance = illuminance;
//
//            illuminanceGrid.push_back(point);
//            oneLine.push_back(illuminance);
//            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
//            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
//        }
//        illuminanceGriddata.push_back(oneLine);
//    }
//}
//
//void IESIsoWidget::updateColorMapData()
//{
//    if (illuminanceGrid.empty()) return;
//
//
//    m_colorMap = new QCPColorMap(xAxis, yAxis);
//    m_colorMap->setColorScale(m_colorScale);
//    //m_colorMap->setGradient(QCPColorGradient::gpJet);
//    QCPColorGradient gradient;
//    gradient.setColorStopAt(0, QColor(0, 0, 255));     // 蓝色
//    gradient.setColorStopAt(0.25, QColor(0, 255, 255)); // 青色
//    gradient.setColorStopAt(0.5, QColor(0, 255, 0));   // 绿色
//    gradient.setColorStopAt(0.75, QColor(255, 255, 0)); // 黄色
//    gradient.setColorStopAt(1, QColor(255, 0, 0));     // 红色
//
//    m_colorMap->setGradient(gradient);
//    m_colorMap->setInterpolate(true);
//
//    gridSpacing = calculationWidth / m_numOfPoints;
//
//    double halfRange = calculationWidth / 2.0;
//    int gridSize = calculationWidth / gridSpacing;
//
//    //QCPColorMap* colorMap = new QCPColorMap(this->xAxis, this->yAxis);
//    //QCPColorMapData* data = m_colorMap->data();
//    QCPColorMapData* m_colorMapData = new QCPColorMapData(gridSize, gridSize,QCPRange(-halfRange, halfRange),
//        QCPRange(-halfRange, halfRange));
//
//
//    double minIllu=INFINITY;
//    double maxIllu=-INFINITY;
//    // 填充数据
//    for (int i = 0; i < gridSize; ++i) {
//        for (int j = 0; j < gridSize; ++j) {
//            int idx = i * (gridSize + 1) + j;
//            if (idx < illuminanceGrid.size()) {
//                m_colorMapData->setCell(i, j, illuminanceGrid[idx].illuminance);
//                if (minIllu > illuminanceGrid[idx].illuminance)minIllu = illuminanceGrid[idx].illuminance;
//                if (maxIllu < illuminanceGrid[idx].illuminance)maxIllu = illuminanceGrid[idx].illuminance;
//            }
//        }
//    }
//
//    // 设置数据范围
//    m_colorMap->setData(m_colorMapData);
//    m_colorMap->setDataRange(QCPRange(minIllu, maxIllu));
//    m_colorScale->setDataRange(QCPRange(minIllu, maxIllu));
//}
//void IESIsoWidget::updateIESXY(double distance, double halfmap)
//{
//    fixtureX = 0;
//    fixtureY = 0;
//    fixtureZ = distance;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateXYPlaneIlluminance();
//
//    updatePlot();
//}
//
//void IESIsoWidget::updateIESYZ(double distance, double halfmap)
//{
//    fixtureX = distance;
//    fixtureY = 0;
//    fixtureZ = 0;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateYZPlaneIlluminance();
//   
//    updatePlot();
//}
//void IESIsoWidget::updateIESXZ(double distance, double halfmap)
//{
//    fixtureX = 0;
//    fixtureY = distance;
//    fixtureZ = 0;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateXZPlaneIlluminance();
//
//    updatePlot();
//}
//
//void IESIsoWidget::updateIESXY_(double distance, double halfmap)
//{
//    fixtureX = 0;
//    fixtureY = 0;
//    fixtureZ = distance;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateXY_PlaneIlluminance();
//
//    updatePlot();
//}
//void IESIsoWidget::updateIESYZ_(double distance, double halfmap)
//{
//    fixtureX = distance;
//    fixtureY = 0;
//    fixtureZ = 0;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateYZ_PlaneIlluminance();
//
//    updatePlot();
//}
//void IESIsoWidget::updateIESXZ_(double distance, double halfmap)
//{
//    fixtureX = 0;
//    fixtureY = distance;
//    fixtureZ = 0;
//    calculationWidth = halfmap * 2;
//    if (IESLoader::instance().light.candela.size() < 1)
//        return;
//
//    calculateXZ_PlaneIlluminance();
//
//    updatePlot();
//}
#include "IESIsoWidget.h"
#include "IESLoader.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_painter.h>
#include <QDebug>
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <limits>

IESIsoWidget::IESIsoWidget(QWidget* parent)
    : QwtPlot(parent)
    , m_spectrogram(nullptr)
    , m_grid(nullptr)
    , m_colorScale(nullptr)
    , m_rasterData(nullptr)
    , calculationWidth(20)
    , gridSpacing(0.1)
    , maxIlluminance(0)
    , minIlluminance(1e9)
    , fixtureX(0)
    , fixtureY(0)
    , fixtureZ(0)
    , m_levelSize(5)
    , m_bUseGrid(false)
    , m_numOfPoints(200)
    , m_colorMap(nullptr)
{
    Init();
}

IESIsoWidget::~IESIsoWidget()
{
    clearPlot();

    if (m_rasterData) {
        delete m_rasterData;
    }
    if (m_colorMap) {
        delete m_colorMap;
    }
}

void IESIsoWidget::Init()
{
    // 设置画布
    QwtPlotCanvas* canvas = new QwtPlotCanvas();
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);
    canvas->setLineWidth(1);
    canvas->setPalette(Qt::white);
    setCanvas(canvas);

    // 设置坐标轴范围
    setAxisScale(QwtPlot::xBottom, -10, 10);
    setAxisScale(QwtPlot::yLeft, -10, 10);

    // 启用右侧坐标轴用于颜色标尺
    enableAxis(QwtPlot::yRight);
    axisWidget(QwtPlot::yRight)->setColorBarEnabled(true);

    // 创建颜色映射
    m_colorMap = new IESColorMap();

    // 创建网格
    m_grid = new QwtPlotGrid();
    m_grid->enableX(true);
    m_grid->enableY(true);

    m_grid->setMajorPen(QPen(Qt::black, 1, Qt::DotLine));
    //m_grid->setMinorPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    m_grid->attach(this);
    m_grid->setVisible(false);

    // 设置布局
    plotLayout()->setAlignCanvasToScales(true);

    // 设置画布大小策略
    canvas->setMinimumSize(400, 300);

    // 启用交互
    canvas->setCursor(Qt::CrossCursor);
}

void IESIsoWidget::clearPlot()
{
    // 清除等值线曲线
    for (QwtPlotCurve* curve : m_contourCurves) {
        curve->detach();
        delete curve;
    }
    m_contourCurves.clear();

    // 清除光谱图
    //if (m_spectrogram) {
    //    m_spectrogram->detach();
    //    delete m_spectrogram;
    //    m_spectrogram = nullptr;
    //}
}

void IESIsoWidget::updatePlot()
{
    // 清除旧图形
    clearPlot();

    if (illuminanceGriddata.empty()) {
        qDebug() << "Illuminance data is empty!";
        return;
    }

    double halfRange = calculationWidth / 2.0;

    // 创建新的光谱图
    m_spectrogram = new QwtPlotSpectrogram("照度分布");
    m_spectrogram->setRenderHint(QwtPlotItem::RenderAntialiased, false);
    m_spectrogram->setColorMap(m_colorMap);

    // 创建并设置光栅数据
    m_rasterData = new IlluminanceRasterData();
    m_rasterData->setData(illuminanceGriddata, halfRange);
    m_spectrogram->setData(m_rasterData);
    m_spectrogram->attach(this);

    // 设置光谱图的Z值，使其在网格下方
    m_spectrogram->setZ(0);

    double step = halfRange / 10;
    // 设置坐标轴范围
    setAxisScale(QwtPlot::xBottom, -halfRange, halfRange, step);
    setAxisScale(QwtPlot::yLeft, -halfRange, halfRange, step);

    // 更新右侧颜色标尺
    if (maxIlluminance > minIlluminance) {
        axisWidget(QwtPlot::yRight)->setColorMap(
            QwtInterval(minIlluminance, maxIlluminance),
            m_colorMap
        );
        setAxisScale(QwtPlot::yRight, minIlluminance, maxIlluminance);
    }

    // 计算等值线
    if (m_levelSize > 0 && maxIlluminance > minIlluminance) {
        double dist = maxIlluminance - minIlluminance;
        double step = dist / m_levelSize;

        std::vector<double> levels;
        for (int i = 1; i < m_levelSize; i++) {
            levels.push_back(minIlluminance + i * step);
        }

        // 使用MarchingSquares生成等值线
        if (!illuminanceGriddata.empty()) {
            // 注意：MarchingSquares生成的数据需要修正90°翻转
            MarchingSquares contourPlot(illuminanceGriddata,
                -halfRange, halfRange,
                -halfRange, halfRange);

            std::vector<MarchingSquares::levelPaths> contours = contourPlot.mkIsos(levels);

            // 创建等值线曲线
            for (size_t l = 0; l < contours.size(); l++) {
                for (size_t j = 0; j < contours[l].size(); j++) {
                    if (contours[l][j].size() > 4) {
                        QwtPlotCurve* curve = new QwtPlotCurve();
                        curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
                        curve->setPen(QPen(Qt::black, 1));
                        curve->setBrush(Qt::NoBrush);

                        QPolygonF points;
                        for (size_t i = 0; i < contours[l][j].size(); i++) {
                            // 修正90°翻转：交换x和y坐标
                            double x = contours[l][j][i].x();  // 原y坐标作为x坐标
                            double y = contours[l][j][i].y();  // 原x坐标作为y坐标
                            points << QPointF(x, y);
                        }

                        // 闭合曲线
                        if (points.size() > 2) {
                            points << points.first();
                        }

                        curve->setSamples(points);
                        curve->attach(this);
                        curve->setZ(10);  // 等值线在颜色映射之上

                        m_contourCurves.push_back(curve);
                    }
                }
            }
        }
    }
    //    //显示网格线
    if (m_bUseGrid)
    {
        m_grid->setVisible(true);
    }
    else
    {
        m_grid->setVisible(false);
    }
    // 重新绘制
    replot();

    qDebug() << "Plot updated. Illuminance range:" << minIlluminance << "-" << maxIlluminance;
}

// 修正数据存储顺序，以匹配Qwt的坐标系
void IESIsoWidget::calculateXYPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfWidth = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将y作为第一维，x作为第二维
    // 这样在显示时就不会有90°翻转了
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // y方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double y = -halfWidth + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // x方向
            double x = -halfWidth + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint(x, y, 0);

            IlluminancePoint point;
            point.x = x;
            point.y = y;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // y作为第一维
    }

    // 确保最小值合理
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESIsoWidget::calculateYZPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfRange = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将z作为第一维，y作为第二维
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // z方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double z = -halfRange + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // y方向
            double y = -halfRange + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint(0, y, z);

            IlluminancePoint point;
            point.x = 0;
            point.y = y;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // z作为第一维
    }

    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESIsoWidget::calculateXZPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfRange = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将z作为第一维，x作为第二维
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // z方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double z = -halfRange + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // x方向
            double x = -halfRange + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint(x, 0, z);

            IlluminancePoint point;
            point.x = x;
            point.y = 0;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // z作为第一维
    }

    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESIsoWidget::calculateXY_PlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfWidth = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将y作为第一维，x作为第二维
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // y方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double y = -halfWidth + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // x方向
            double x = -halfWidth + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint_(x, y, 0);

            IlluminancePoint point;
            point.x = x;
            point.y = y;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // y作为第一维
    }

    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESIsoWidget::calculateYZ_PlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfRange = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将z作为第一维，y作为第二维
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // z方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double z = -halfRange + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // y方向
            double y = -halfRange + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint_(0, y, z);

            IlluminancePoint point;
            point.x = 0;
            point.y = y;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // z作为第一维
    }

    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESIsoWidget::calculateXZ_PlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;
    double halfRange = calculationWidth / 2.0;
    int gridSize = m_numOfPoints;

    // 注意：为了匹配Qwt的坐标系，我们需要将z作为第一维，x作为第二维
    illuminanceGriddata.resize(gridSize + 1);

    for (int j = 0; j <= gridSize; ++j) {  // z方向
        std::vector<double> oneLine(gridSize + 1, 0.0);
        double z = -halfRange + j * gridSpacing;
        for (int i = 0; i <= gridSize; ++i) {  // x方向
            double x = -halfRange + i * gridSpacing;
            double illuminance = calculateIlluminanceAtPoint_(x, 0, z);

            IlluminancePoint point;
            point.x = x;
            point.y = 0;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine[i] = illuminance;

            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata[j] = oneLine;  // z作为第一维
    }

    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

// 修正数据索引访问，确保正确的坐标系
// 同时需要修正 IlluminanceRasterData 的 value 函数
// 在 IESIsoWidget.h 中修改 IlluminanceRasterData 的 value 函数：

double IESIsoWidget::calculateIlluminanceAtPoint(double x, double y, double z)
{

    double dx = x - fixtureX;
    double dy = y - fixtureY;
    double dz = z - fixtureZ;
    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);

    if (totalDistance == 0) return 0;

    double verticalAngle = acos(dz / totalDistance) * 180.0 / M_PI;
    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;

    if (horizontalAngle < 0) horizontalAngle += 360;

    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);
    double cosIncidence = 0;
    if (z == 0)
        cosIncidence = -dz / totalDistance;
    else if (x == 0)
        cosIncidence = -dx / totalDistance;
    else if (y == 0)
        cosIncidence = -dy / totalDistance;

    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;

    return std::max(0.0, illuminance);
}

double IESIsoWidget::calculateIlluminanceAtPoint_(double x, double y, double z)
{
    double dx = x - fixtureX;
    double dy = y - fixtureY;
    double dz = z - fixtureZ;
    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);

    if (totalDistance == 0) return 0;

    double verticalAngle = acos(-dz / totalDistance) * 180.0 / M_PI;
    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;

    if (horizontalAngle < 0) horizontalAngle += 360;

    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);
    double cosIncidence = 0;
    if (z == 0)
        cosIncidence = -dz / totalDistance;
    else if (x == 0)
        cosIncidence = -dx / totalDistance;
    else if (y == 0)
        cosIncidence = -dy / totalDistance;
    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;

    return std::max(0.0, illuminance);
}

// 同时需要更新 IESIsoWidget.h 中的 IlluminanceRasterData::value 函数：
// 由于我们修改了数据存储顺序（y作为第一维，x作为第二维），
// 我们需要相应地修改 value 函数中的索引计算

// 在 IESIsoWidget.h 中修改 IlluminanceRasterData 类的 value 函数：
/*
virtual double value(double x, double y) const override {
    if (m_data.empty() || m_gridSize == 0)
        return 0.0;

    // 检查是否在数据范围内
    if (x < m_xInterval.minValue() || x > m_xInterval.maxValue() ||
        y < m_yInterval.minValue() || y > m_yInterval.maxValue()) {
        return 0.0;
    }

    // 将坐标转换为网格索引
    // 注意：由于我们修改了数据存储顺序（y作为第一维，x作为第二维）
    // 所以这里需要：
    // - 用y得到第一维索引（j）
    // - 用x得到第二维索引（i）
    double normalizedX = (x - m_xInterval.minValue()) / m_xInterval.width();
    double normalizedY = (y - m_yInterval.minValue()) / m_yInterval.width();

    int i = static_cast<int>(normalizedX * (m_gridSize - 1));  // x方向，第二维
    int j = static_cast<int>(normalizedY * (m_gridSize - 1));  // y方向，第一维

    i = qBound(0, i, static_cast<int>(m_gridSize) - 1);
    j = qBound(0, j, static_cast<int>(m_gridSize) - 1);

    return m_data[j][i];  // 第一维是y，第二维是x
}
*/

void IESIsoWidget::updateIESXY(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = 0;
    fixtureZ = distance;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXYPlaneIlluminance();
    updatePlot();
}

void IESIsoWidget::updateIESYZ(double distance, double halfmap)
{
    fixtureX = distance;
    fixtureY = 0;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateYZPlaneIlluminance();
    updatePlot();
}

void IESIsoWidget::updateIESXZ(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = distance;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXZPlaneIlluminance();
    updatePlot();
}

void IESIsoWidget::updateIESXY_(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = 0;
    fixtureZ = distance;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXY_PlaneIlluminance();
    updatePlot();
}

void IESIsoWidget::updateIESYZ_(double distance, double halfmap)
{
    fixtureX = distance;
    fixtureY = 0;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateYZ_PlaneIlluminance();
    updatePlot();
}

void IESIsoWidget::updateIESXZ_(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = distance;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;

    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXZ_PlaneIlluminance();
    updatePlot();
}