#include "DeformationWidget.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <limits>

// ==================== DataLoader 实现 ====================
std::vector<NodeData> DataLoader::load(const std::string& filename) {
    std::vector<NodeData> nodes;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        return nodes;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        NodeData nd;
        if (!(iss >> nd.x >> nd.y >> nd.z >> nd.ux >> nd.uy >> nd.uz)) {
            std::cerr << "Warning: skip malformed line: " << line << std::endl;
            continue;
        }
        nodes.push_back(nd);
    }
    std::cout << "Loaded " << nodes.size() << " raw nodes." << std::endl;
    return nodes;
}

// ==================== Interpolator 实现 ====================
Interpolator::Interpolator(const std::vector<NodeData>& nodes)
    : m_nodes(nodes), m_adaptor(nodes) {
    m_tree = new KDTree(2, m_adaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    m_tree->buildIndex();
}

Interpolator::~Interpolator() {
    delete m_tree;
}

double Interpolator::interpolate(double x, double y, int fieldType, int k, double power) const {
    double query_pt[2] = { x, y };
    std::vector<uint32_t> indices(k);
    std::vector<double> dists_sq(k);
    size_t num_found = m_tree->knnSearch(&query_pt[0], k, &indices[0], &dists_sq[0]);

    double sum_w = 0.0, sum_wv = 0.0;
    for (size_t i = 0; i < num_found; ++i) {
        double dist = std::sqrt(dists_sq[i]);
        if (dist < 1e-12) {
            const auto& nd = m_nodes[indices[i]];
            if (fieldType == 1) return nd.ux;
            if (fieldType == 2) return nd.uy;
            if (fieldType == 3) return nd.uz;
            return std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        }
        double w = 1.0 / std::pow(dist, power);
        sum_w += w;
        const auto& nd = m_nodes[indices[i]];
        double value;
        if (fieldType == 1) value = nd.ux;
        else if (fieldType == 2) value = nd.uy;
        else if (fieldType == 3) value = nd.uz;
        else value = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        sum_wv += w * value;
    }
    return (sum_w > 0) ? (sum_wv / sum_w) : 0.0;
}

// ==================== CustomRasterData 实现 ====================
CustomRasterData::CustomRasterData(const Interpolator& interp,
    double xmin, double xmax,
    double ymin, double ymax,
    int fieldType,
    double zmin, double zmax,
    bool useFixedZRange,
    double centerX, double centerY, double radius)
    : m_interp(interp),
    m_xmin(xmin), m_xmax(xmax), m_ymin(ymin), m_ymax(ymax),
    m_fieldType(fieldType),
    m_useFixedZRange(useFixedZRange),
    m_zmin(zmin), m_zmax(zmax),
    m_zRangeValid(false),
    m_centerX(centerX), m_centerY(centerY), m_radius(radius) {}

// copy() 已删除，可能导致编译错误

QwtInterval CustomRasterData::interval(Qt::Axis axis) const {
    switch (axis) {
    case Qt::XAxis:
        return QwtInterval(m_xmin, m_xmax);
    case Qt::YAxis:
        return QwtInterval(m_ymin, m_ymax);
    case Qt::ZAxis:
        if (m_useFixedZRange) {
            return QwtInterval(m_zmin, m_zmax);
        }
        if (!m_zRangeValid) {
            double minVal = 1e9, maxVal = -1e9;
            const int samplePoints = 100;
            double dx = (m_xmax - m_xmin) / samplePoints;
            double dy = (m_ymax - m_ymin) / samplePoints;
            for (int i = 0; i <= samplePoints; ++i) {
                double x = m_xmin + i * dx;
                for (int j = 0; j <= samplePoints; ++j) {
                    double y = m_ymin + j * dy;
                    double val = value(x, y);
                    if (!std::isnan(val)) {
                        if (val < minVal) minVal = val;
                        if (val > maxVal) maxVal = val;
                    }
                }
            }
            if (minVal == maxVal) {
                minVal -= 1.0;
                maxVal += 1.0;
            }
            m_zmin = minVal;
            m_zmax = maxVal;
            m_zRangeValid = true;
        }
        return QwtInterval(m_zmin, m_zmax);
    default:
        return QwtInterval();
    }
}

double CustomRasterData::value(double x, double y) const {
    if (m_radius > 0) {
        double dx = x - m_centerX;
        double dy = y - m_centerY;
        if (dx * dx + dy * dy > m_radius * m_radius)
            return std::numeric_limits<double>::quiet_NaN();
    }
    x = std::clamp(x, m_xmin, m_xmax);
    y = std::clamp(y, m_ymin, m_ymax);
    return m_interp.interpolate(x, y, m_fieldType);
}

void CustomRasterData::setFieldType(int type) {
    if (m_fieldType != type) {
        m_fieldType = type;
        m_zRangeValid = false;
    }
}

// ==================== MultiDeformationViewer 实现 ====================
MultiDeformationViewer::MultiDeformationViewer(const QString& dataFile, QWidget* parent)
    : QWidget(parent), m_interp(nullptr), m_sharedColorBar(nullptr),
    m_circleCenterX(0.0), m_circleCenterY(0.0), m_circleRadius(10.2)
{
    loadData(dataFile);
    if (m_nodes.empty()) {
        QMessageBox::critical(this, "Error", "Failed to load data file.");
        return;
    }
    computeGlobalZRange();
    setupUI();
}

MultiDeformationViewer::~MultiDeformationViewer() {
    delete m_interp;
}

void MultiDeformationViewer::loadData(const QString& dataFile) {
    std::vector<NodeData> rawNodes = DataLoader::load(dataFile.toStdString());
    if (rawNodes.empty()) return;

    double origXmin = rawNodes[0].x, origXmax = rawNodes[0].x;
    double origYmin = rawNodes[0].y, origYmax = rawNodes[0].y;
    for (const auto& nd : rawNodes) {
        origXmin = std::min(origXmin, nd.x);
        origXmax = std::max(origXmax, nd.x);
        origYmin = std::min(origYmin, nd.y);
        origYmax = std::max(origYmax, nd.y);
    }

    double xRange = origXmax - origXmin;
    double yRange = origYmax - origYmin;
    if (xRange < 1e-12) xRange = 1.0;
    if (yRange < 1e-12) yRange = 1.0;

    const double TARGET_MIN = -10.2;
    const double TARGET_MAX = 10.2;
    const double TARGET_RANGE = TARGET_MAX - TARGET_MIN;

    m_nodes.clear();
    m_nodes.reserve(rawNodes.size());
    for (auto& nd : rawNodes) {
        NodeData mapped = nd;
        mapped.x = TARGET_MIN + (nd.x - origXmin) / xRange * TARGET_RANGE;
        mapped.y = TARGET_MIN + (nd.y - origYmin) / yRange * TARGET_RANGE;
        m_nodes.push_back(mapped);
    }

    std::cout << "Mapped " << m_nodes.size() << " nodes to [-10.2, 10.2]." << std::endl;
    m_interp = new Interpolator(m_nodes);
}

void MultiDeformationViewer::computeGlobalZRange() {
    m_globalZmin = 1e9;
    m_globalZmax = -1e9;
    for (const auto& nd : m_nodes) {
        double dx = nd.x - m_circleCenterX;
        double dy = nd.y - m_circleCenterY;
        if (dx * dx + dy * dy > m_circleRadius * m_circleRadius)
            continue;
        double mag = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        double vals[4] = { mag, nd.ux, nd.uy, nd.uz };
        for (double v : vals) {
            if (v < m_globalZmin) m_globalZmin = v;
            if (v > m_globalZmax) m_globalZmax = v;
        }
    }
    if (m_globalZmin == m_globalZmax) {
        m_globalZmin -= 1.0;
        m_globalZmax += 1.0;
    }
    std::cout << "Global Z range: [" << m_globalZmin << ", " << m_globalZmax << "]" << std::endl;
}

void MultiDeformationViewer::setupUI() {

    m_layout = new QGridLayout(this);
    m_layout->setSpacing(10);
    setWindowTitle("2D Deformation Maps - Normalized to [-12.2, 12.2]");

    const double AXIS_MIN = -12.2;
    const double AXIS_MAX = 12.2;

    m_plotX = createPlot("X Deformation (mm)", 1, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX);
    m_plotY = createPlot("Y Deformation (mm)", 2, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX);
    m_plotZ = createPlot("Z Deformation (mm)", 3, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX);
    m_plotMag = createPlot("Total Deformation (mm)", 0, AXIS_MIN, AXIS_MAX, AXIS_MIN, AXIS_MAX);

    m_layout->addWidget(m_plotX, 0, 0);
    m_layout->addWidget(m_plotY, 0, 1);
    m_layout->addWidget(m_plotZ, 1, 0);
    m_layout->addWidget(m_plotMag, 1, 1);

    // ========== 共享颜色条（分为6段，显示分界点数值） ==========
    m_sharedColorBar = new MyScaleWidget(QwtScaleDraw::LeftScale, this);
    m_sharedColorBar->setColorBarEnabled(true);
    m_sharedColorBar->setColorBarWidth(20);
    m_sharedColorBar->setTitle("Deformation (mm)");


    QwtLinearColorMap* colorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
    colorMap->addColorStop(0.33, QColor(0, 255, 255));
    colorMap->addColorStop(0.67, QColor(255, 255, 0));
    m_sharedColorBar->setColorMap(QwtInterval(m_globalZmin, m_globalZmax), colorMap);

    // 将颜色条刻度分为6段，共7个分界点
    QList<double> majorTicks;
    double step = (m_globalZmax - m_globalZmin) / 5.0;
    for (int i = 0; i <= 5; ++i) {
        majorTicks.append(m_globalZmin + i * step);
    }
    majorTicks.last() = m_globalZmax;

    QwtScaleDiv scaleDiv(m_globalZmin, m_globalZmax,
        QList<double>(),  // minor ticks (none)
        QList<double>(),  // medium ticks (none)
        majorTicks);      // major ticks
    m_sharedColorBar->setScaleDiv(scaleDiv);

    QwtScaleDraw* scaleDraw = m_sharedColorBar->scaleDraw();
    //scaleDraw->setLabelFormat("%.3f");
    scaleDraw->setTickLength(QwtScaleDiv::MajorTick, 6);
    scaleDraw->setTickLength(QwtScaleDiv::MinorTick, 0);
    scaleDraw->setTickLength(QwtScaleDiv::MediumTick, 0);
    scaleDraw->enableComponent(QwtScaleDraw::Backbone, false);

    m_layout->addWidget(m_sharedColorBar, 0, 2, 2, 1);
}

QwtPlot* MultiDeformationViewer::createPlot(const QString& title, int fieldType,
    double xmin, double xmax,
    double ymin, double ymax) {
    QwtPlot* plot = new QwtPlot(this);
    plot->setTitle(title);
    plot->setAxisTitle(QwtPlot::xBottom, "X (mm)");
    plot->setAxisTitle(QwtPlot::yLeft, "Y (mm)");
    plot->setCanvasBackground(Qt::white);

    plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);

    CustomRasterData* rasterData = new CustomRasterData(*m_interp,
        xmin, xmax, ymin, ymax,
        fieldType, m_globalZmin, m_globalZmax, true,
        m_circleCenterX, m_circleCenterY, m_circleRadius);
    QwtPlotSpectrogram* spectrogram = new QwtPlotSpectrogram();
    spectrogram->setData(rasterData);
    QwtLinearColorMap* colorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
    colorMap->addColorStop(0.33, QColor(0, 255, 255));
    colorMap->addColorStop(0.67, QColor(255, 255, 0));
    spectrogram->setColorMap(colorMap);
    spectrogram->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    spectrogram->attach(plot);

    plot->enableAxis(QwtPlot::yRight, false);

    std::vector<double> values;
    std::vector<NodeData> validNodes;
    for (const auto& nd : m_nodes) {
        double dx = nd.x - m_circleCenterX;
        double dy = nd.y - m_circleCenterY;
        if (dx * dx + dy * dy > m_circleRadius * m_circleRadius)
            continue;
        validNodes.push_back(nd);
        double val;
        if (fieldType == 1)      val = nd.ux;
        else if (fieldType == 2) val = nd.uy;
        else if (fieldType == 3) val = nd.uz;
        else                     val = std::sqrt(nd.ux * nd.ux + nd.uy * nd.uy + nd.uz * nd.uz);
        values.push_back(val);
    }
    if (!values.empty()) {
        auto maxIt = std::max_element(values.begin(), values.end());
        auto minIt = std::min_element(values.begin(), values.end());
        size_t maxIdx = std::distance(values.begin(), maxIt);
        size_t minIdx = std::distance(values.begin(), minIt);
        double maxVal = *maxIt;
        double minVal = *minIt;

        double xRange = xmax - xmin;
        double yRange = ymax - ymin;
        addMarkers(plot, values, validNodes, xRange, yRange, maxIdx, minIdx, maxVal, minVal);
        addInfoBox(plot, (int)validNodes.size(), minVal, maxVal,
            xmin + 0.02 * xRange, ymax - 0.05 * yRange);
    }

    plot->replot();
    return plot;
}

void MultiDeformationViewer::addMarkers(QwtPlot* plot, const std::vector<double>& values,
    const std::vector<NodeData>& nodes,
    double xRange, double yRange,
    size_t maxIdx, size_t minIdx,
    double maxVal, double minVal) {
    if (values.empty()) return;

    QwtPlotMarker* maxMarker = new QwtPlotMarker();
    maxMarker->setValue(nodes[maxIdx].x, nodes[maxIdx].y);
    maxMarker->setSymbol(new QwtSymbol(QwtSymbol::Triangle, QBrush(Qt::red), QPen(Qt::white, 2), QSize(12, 12)));
    maxMarker->attach(plot);

    QwtPlotMarker* minMarker = new QwtPlotMarker();
    minMarker->setValue(nodes[minIdx].x, nodes[minIdx].y);
    minMarker->setSymbol(new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::blue), QPen(Qt::white, 2), QSize(12, 12)));
    minMarker->attach(plot);

    double offsetX = 0.02 * xRange;
    double offsetY = 0.02 * yRange;
    QwtPlotMarker* maxLabel = new QwtPlotMarker();
    maxLabel->setValue(nodes[maxIdx].x + offsetX, nodes[maxIdx].y + offsetY);
    maxLabel->setLabel(QwtText(QString(" Max: %1").arg(maxVal, 0, 'f', 3)));
    maxLabel->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    maxLabel->attach(plot);

    QwtPlotMarker* minLabel = new QwtPlotMarker();
    minLabel->setValue(nodes[minIdx].x + offsetX, nodes[minIdx].y - offsetY);
    minLabel->setLabel(QwtText(QString(" Min: %1").arg(minVal, 0, 'f', 3)));
    minLabel->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    minLabel->attach(plot);
}

void MultiDeformationViewer::addInfoBox(QwtPlot* plot, int nodeCount,
    double minVal, double maxVal,
    double xPos, double yPos) {
    QString infoText = QString("Nodes: %1\nRange: [%2, %3]")
        .arg(nodeCount)
        .arg(minVal, 0, 'f', 3)
        .arg(maxVal, 0, 'f', 3);
    QwtPlotMarker* infoBox = new QwtPlotMarker();
    infoBox->setValue(xPos, yPos);
    QwtText text(infoText);
    text.setColor(Qt::white);
    text.setBackgroundBrush(QColor(0, 0, 0, 180));
    text.setBorderPen(QPen(Qt::gray, 1));
    text.setFont(QFont("Arial", 9));
    infoBox->setLabel(text);
    infoBox->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoBox->attach(plot);
}