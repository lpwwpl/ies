#ifndef MULTIDEFORMATIONVIEWER_H
#define MULTIDEFORMATIONVIEWER_H

#include <QWidget>
#include <QGridLayout>
#include <vector>
#include <string>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_interval.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include "QwtPropertyBrowser.h"
#include <nanoflann.hpp>
#include <QPainter>
#include "PlotBase.h"
#include <qwt_text.h>
class QSplitter;
class QComboBox;

// 数据结构
struct NodeData {
    double x, y, z;
    double ux, uy, uz;
};

// 数据加载
class DataLoader {
public:
    static std::vector<NodeData> load(const std::string& filename);
};

// KD-Tree 适配器
struct PointCloudAdaptor {
    const std::vector<NodeData>& cloud;
    PointCloudAdaptor(const std::vector<NodeData>& c) : cloud(c) {}
    inline size_t kdtree_get_point_count() const { return cloud.size(); }
    inline double kdtree_get_pt(const size_t idx, int dim) const {
        if (dim == 0) return cloud[idx].x;
        if (dim == 1) return cloud[idx].y;
        return 0.0;
    }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, PointCloudAdaptor>,
    PointCloudAdaptor, 2>;

// 插值器
class Interpolator {
public:
    Interpolator(const std::vector<NodeData>& nodes);
    ~Interpolator();
    double interpolate(double x, double y, int fieldType = 0,
        int k = 8, double power = 2.0) const;
private:
    const std::vector<NodeData>& m_nodes;
    PointCloudAdaptor m_adaptor;
    KDTree* m_tree;
};

// 栅格数据（支持圆形裁剪和固定 Z 范围）
// 注意：已删除 copy() 方法，可能导致编译错误
class CustomRasterData : public QwtRasterData {
public:
    CustomRasterData(const Interpolator& interp,
        double xmin, double xmax,
        double ymin, double ymax,
        int fieldType = 0,
        double zmin = 0, double zmax = 1,
        bool useFixedZRange = false,
        double centerX = 0.0, double centerY = 0.0, double radius = -1.0);

    virtual QwtInterval interval(Qt::Axis axis) const override;
    virtual double value(double x, double y) const override;
    void setFieldType(int type);
    int fieldType() const { return m_fieldType; }
private:
    const Interpolator& m_interp;
    double m_xmin, m_xmax, m_ymin, m_ymax;
    int m_fieldType;
    bool m_useFixedZRange;
    mutable double m_zmin, m_zmax;
    mutable bool m_zRangeValid;
    double m_centerX, m_centerY, m_radius;   // 圆形裁剪参数
};
class MyScaleWidget : public QwtScaleWidget {
public:
    // 构造函数，接收对齐方式等参数，直接传递给基类
    explicit MyScaleWidget(QwtScaleDraw::Alignment alignment = QwtScaleDraw::LeftScale,
        QWidget* parent = nullptr)
        : QwtScaleWidget(alignment, parent) {
    }

protected:
    // 重写 paintEvent，这是所有绘制工作发生的地方
    void paintEvent(QPaintEvent* event) override 
    {
        // 准备绘制
        QPainter painter(this);
        scaleDraw()->draw(&painter, palette());
        if (colorBarInterval().isValid() && isColorBarEnabled() && colorBarWidth() > 0)
        {
            drawColorBar(&painter, colorBarRect(contentsRect()));
        }
        QRect r = contentsRect();
        if (scaleDraw()->orientation() == Qt::Horizontal)
        {
            r.setLeft(r.left() + startBorderDist());
            r.setWidth(r.width() - endBorderDist());
        }
        else
        {
            r.setTop(r.top() + startBorderDist());
            r.setHeight(r.height() - endBorderDist());
        }

        QwtText title = this->title();

        if (!title.isEmpty())
        {
            // 使用 drawTitle 绘制，对齐方式为左对齐（标题文字靠左）
            drawTitle(&painter, QwtScaleDraw::LeftScale, r);
        }
    }
};
// 主窗口：四个子图 + 共享颜色条
class MultiDeformationViewer : public QWidget {
    Q_OBJECT
public:
    explicit MultiDeformationViewer(const QString& dataFile, QWidget* parent = nullptr);
    ~MultiDeformationViewer();
    void zoomIn_info(PlotInfo* info);
    void zoomOut_info(PlotInfo* info);
    void fitview_info(PlotInfo* info);
    PlotInfo* GetPlotInfo(QwtPlot* plot);
public Q_SLOTS:
    void onPlotSelected(int index);
    void onApplyToAllToggled(bool checked);
    void applyAllSettingsToAllPlots();
    void syncSettingsToPlot(PlotInfo* curInfo, PlotInfo* info);

    void updateXScaleAxes(QwtPlot* plot, PlotInfo& info);
    void updateYScaleAxes(QwtPlot* plot, PlotInfo& info);
    void onPropertyChanged();
    void updateX();
    void updateY();
    void updateXY() { updateX(); updateY(); }
    void updateLegend();
    void updateAxesSettings_noparam();
    void updateAutoScaleY();
    void updateAutoScaleX();
    void onLegendClicked(const QVariant& itemInfo);

    void saveAsSVG(const QString& fileName);

    void fitView();
    void zoomIn();
    void zoomOut();

    void onColorBarToggled(bool);

public:
    void saveInitialView(PlotInfo& info);
    void setupPlotInteractions(QwtPlot* plot);
    void updateAxesSettings(QwtPlot* plot, PlotInfo& info);

private:
    void loadData(const QString& dataFile);
    void computeGlobalZRange();
    void setupUI();
    QwtPlot* createPlot(const QString& title, int fieldType,
        double xmin, double xmax, double ymin, double ymax,bool isYTitle);
    void addMarkers(QwtPlot* plot, const std::vector<double>& values,
        const std::vector<NodeData>& nodes,
        double xRange, double yRange,
        size_t maxIdx, size_t minIdx,
        double maxVal, double minVal);
    void addInfoBox(QwtPlot* plot, int nodeCount, double minVal, double maxVal,
        double xPos, double yPos);


    QToolBar* m_toolBar;
    QwtPlot* m_plotX;
    QwtPlot* m_plotY;
    QwtPlot* m_plotZ;
    QwtPlot* m_plotMag;

    std::vector<NodeData> m_nodes;
    Interpolator* m_interp;
    double m_globalZmin, m_globalZmax;
    double m_circleCenterX, m_circleCenterY, m_circleRadius;
    //QGridLayout* m_layout;
    MyScaleWidget* m_sharedColorBar;


    QList<PlotInfo*> m_plotInfos;          // 所有图表的详细信息
    int m_currentPlotIndex;               // 当前选中的图表索引
    QVector<QwtPlot*> m_plots;
    QSplitter* m_splitter;
    // UI 控件
    QGridLayout* m_mainLayout;           // 主布局（包含图表网格）
    QVBoxLayout* m_controlLayout;        // 右侧控制面板布局
    QComboBox* m_plotCombo;              // 图表选择下拉框
    QCheckBox* m_applyToAllCheckBox;     // 应用到所有图表复选框
    QCheckBox* m_colorbarCheckBox;     // 应用到所有图表复选框
    QwtPropertyBrowser* m_propertyBrowser; // 属性编辑器
};


#endif // MULTIDEFORMATIONVIEWER_H