//// IESSpotLightWidget.h
//#pragma once
//#include <QWidget>
//#include "QCustomPlot.h"
//#include "tiny_ies.hpp"
//#include "contours.h"
//
//// 照度计算点
//struct IlluminancePoint {
//	double x, y,z;
//	double illuminance; // 照度(lux)
//};
//// 等照度线
//struct IsoluxCurve {
//	double illuminanceLevel;
//	std::vector<QPointF> points;
//	QColor color;
//};
//// 颜色方案
//struct ColorScheme {
//	QString name;
//	std::vector<QColor> colors;
//};
//
//class IESIsoWidget:public QCustomPlot
//{
//	using ColorMapDataPtr = std::unique_ptr<QCPColorMapData>;
//public:
//	IESIsoWidget();
//	~IESIsoWidget();
//
//	void Init();
//
//	void updateIESXY(double distance, double halfmap);
//	void updateIESYZ(double distance,double halfmap);
//	void updateIESXZ(double distance, double halfmap);
//	// ///-x,-y,-z
//	void updateIESXY_(double distance, double halfmap);
//	void updateIESYZ_(double distance, double halfmap);
//	void updateIESXZ_(double distance, double halfmap);
//	void updateColorMapData();
//	void calculateXZPlaneIlluminance();
//	void calculateYZPlaneIlluminance();
//	void calculateXYPlaneIlluminance();
//
//	void calculateXZ_PlaneIlluminance();
//	void calculateYZ_PlaneIlluminance();
//	void calculateXY_PlaneIlluminance();
//
//	//xyz,-x,-y
//	double calculateIlluminanceAtPoint(double x, double y, double z);
//	// ////////////////////////-z
//	double calculateIlluminanceAtPoint_(double x, double y, double z);
//
//	void updatePlot();
//
//	//void setupColorSchemes();
//public:
//	QCPPolarAxisRadial* radialAxis = nullptr;
//	QCPPolarAxisAngular* angularAxis = nullptr;
//	QCPColorScale* m_colorScale;
//	QVector<double> m_contourLevels;
//	//ColorMapDataPtr m_colorMapData;
//	QCPMarginGroup* m_margin;
//	QCPColorMap* m_colorMap;
//	double calculationWidth;
//	double workplaneHeight;
//	double gridSpacing;
//	double maxIlluminance;
//	double minIlluminance;
//
//	std::vector<double> illuminanceLevels;
//	std::vector<IsoluxCurve> isoluxCurves;
//	std::vector<IlluminancePoint> illuminanceGrid;
//	std::vector<vector<double>> illuminanceGriddata;
//	double fixtureX;
//	double fixtureY;
//	double fixtureZ;
//	int m_levelSize;
//	bool m_bUseGrid;
//	int m_numOfPoints;
//	// 颜色方案
//	std::vector<ColorScheme> colorSchemes;
//};
//
#pragma once
#include <QWidget>
#include <QVector>
#include <QPointF>
#include <memory>
#include "tiny_ies.hpp"
#include "contours.h"
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_legenditem.h>

// 照度计算点
struct IlluminancePoint {
    double x, y, z;
    double illuminance; // 照度(lux)
};

// 等照度线
struct IsoluxCurve {
    double illuminanceLevel;
    std::vector<QPointF> points;
    QColor color;
};

// 自定义颜色映射
class IESColorMap : public QwtLinearColorMap
{
public:
    IESColorMap()
        : QwtLinearColorMap(Qt::blue, Qt::red)
    {
        // 设置颜色渐变
        addColorStop(0.0, Qt::blue);
        addColorStop(0.25, QColor(0, 255, 255));   // 青色
        addColorStop(0.5, Qt::green);
        addColorStop(0.75, Qt::yellow);
        addColorStop(1.0, Qt::red);
    }
};

// 用于QwtPlotSpectrogram的数据类
class IlluminanceRasterData : public QwtRasterData
{
public:
    IlluminanceRasterData()
        : m_gridSize(0)
        , m_halfRange(0)
    {
        // 初始化默认区间
        m_xInterval = QwtInterval(-10, 10);
        m_yInterval = QwtInterval(-10, 10);
        m_zInterval = QwtInterval(0, 1000);
    }

    virtual ~IlluminanceRasterData() {}

    void setData(const std::vector<std::vector<double>>& data, double halfRange) {
        m_data = data;
        m_gridSize = data.size();
        m_halfRange = halfRange;

        if (m_gridSize > 0) {
            // 设置X轴和Y轴的范围
            m_xInterval = QwtInterval(-m_halfRange, m_halfRange);
            m_yInterval = QwtInterval(-m_halfRange, m_halfRange);

            // 计算Z轴范围（照度值范围）
            double minZ = std::numeric_limits<double>::max();
            double maxZ = std::numeric_limits<double>::lowest();

            for (const auto& row : data) {
                for (double value : row) {
                    if (value < minZ) minZ = value;
                    if (value > maxZ) maxZ = value;
                }
            }

            // 确保范围有效
            if (minZ > maxZ) {
                minZ = 0;
                maxZ = 1;
            }
            else if (minZ == maxZ) {
                maxZ = minZ + 1;
            }

            m_zInterval = QwtInterval(minZ, maxZ);
        }
    }

    // 实现纯虚函数 interval
    virtual QwtInterval interval(Qt::Axis axis) const override {
        switch (axis) {
        case Qt::XAxis:
            return m_xInterval;
        case Qt::YAxis:
            return m_yInterval;
        case Qt::ZAxis:
            return m_zInterval;
        default:
            return QwtInterval();
        }
    }

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

    // 可选：实现更高效的边界框检查
    virtual QRectF pixelHint(const QRectF& area) const override {
        Q_UNUSED(area);
        if (m_gridSize == 0)
            return QRectF();

        double cellWidth = m_xInterval.width() / m_gridSize;
        double cellHeight = m_yInterval.width() / m_gridSize;
        return QRectF(m_xInterval.minValue(), m_yInterval.minValue(),
            cellWidth, cellHeight);
    }

private:
    std::vector<std::vector<double>> m_data;
    int m_gridSize;
    double m_halfRange;
    QwtInterval m_xInterval;
    QwtInterval m_yInterval;
    QwtInterval m_zInterval;
};

class IESIsoWidget : public QwtPlot
{
    Q_OBJECT

public:
    IESIsoWidget(QWidget* parent = nullptr);
    ~IESIsoWidget();

    void Init();

    void updateIESXY(double distance, double halfmap);
    void updateIESYZ(double distance, double halfmap);
    void updateIESXZ(double distance, double halfmap);

    void updateIESXY_(double distance, double halfmap);
    void updateIESYZ_(double distance, double halfmap);
    void updateIESXZ_(double distance, double halfmap);

    void updatePlot();

    // 计算函数
    void calculateXZPlaneIlluminance();
    void calculateYZPlaneIlluminance();
    void calculateXYPlaneIlluminance();
    void calculateXZ_PlaneIlluminance();
    void calculateYZ_PlaneIlluminance();
    void calculateXY_PlaneIlluminance();

    // 照度计算
    double calculateIlluminanceAtPoint(double x, double y, double z);
    double calculateIlluminanceAtPoint_(double x, double y, double z);

    // 设置网格可见性
    void setGridVisible(bool visible) {
        m_grid->setVisible(visible);
        replot();
    }

    // 设置等值线级别数量
    void setLevelSize(int levelSize) {
        m_levelSize = levelSize;
    }

    // 设置点数
    void setNumPoints(int numPoints) {
        m_numOfPoints = numPoints;
    }

private:
    //void updateColorMapData();
    void clearPlot();

public:
    // Qwt组件
    QwtPlotSpectrogram* m_spectrogram;
    QwtPlotGrid* m_grid;
    QwtScaleWidget* m_colorScale;
    IlluminanceRasterData* m_rasterData;

    // 数据
    double calculationWidth;
    double gridSpacing;
    double maxIlluminance;
    double minIlluminance;

    std::vector<double> illuminanceLevels;
    std::vector<IsoluxCurve> isoluxCurves;
    std::vector<IlluminancePoint> illuminanceGrid;
    std::vector<std::vector<double>> illuminanceGriddata;

    double fixtureX;
    double fixtureY;
    double fixtureZ;
    int m_levelSize;
    bool m_bUseGrid;
    int m_numOfPoints;

    std::vector<QwtPlotCurve*> m_contourCurves;  // 存储等值线曲线
    IESColorMap* m_colorMap;                     // 颜色映射
};