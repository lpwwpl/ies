//#include "IESCartesianWidget.h"
//#include "IESLoader.h"
//#include <QDebug>
//
//IESCartesianWidget::IESCartesianWidget(QWidget* parent):QCustomPlot(parent)
//{
//    Init();
//
//}
//
//IESCartesianWidget::~IESCartesianWidget()
//{
//
//}
//
//
//void IESCartesianWidget::Init()
//{    
//    QFont font;
//    font.setStyleStrategy(QFont::NoAntialias);
//    font.setFamily("Microsoft YaHei");
//    font.setPixelSize(10);
//
//    
//    //this->plotLayout()->clear(); // 清空默认布局
//        // 设置图例
//    legend->setVisible(true);
//    //legend->setPositionStyle(QCPLegend::psTopRight);
//
//    // 图例样式
//    legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
//    legend->setBorderPen(QPen(Qt::darkGray));
//    legend->setFont(QFont("Arial", 9));
//
//   
//    addLayer("background1", nullptr, QCustomPlot::limBelow);
//    addLayer("foreground", nullptr, QCustomPlot::limAbove);
//
//
//
//    graph0_180 = addGraph(xAxis, yAxis);
//    graph0_180->setLayer("foreground");
//
//    // 8. 设置图形样式 (可选)
//    graph0_180->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
//    graph0_180->setPen(QPen(Qt::blue, 2)); // 线宽和颜色
//    //graph0_180->setBrush(QBrush(QColor(0, 0, 250, 150)));
//
//    graph90_270 = addGraph(xAxis, yAxis);
//    graph90_270->setLayer("background1");
//    // 8. 设置图形样式 (可选)
//    graph90_270->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
//    graph90_270->setPen(QPen(Qt::red, 2)); // 线宽和颜色
//    //graph90_270->setBrush(QBrush(QColor(255, 0, 0, 150)));
//
//
//    moveLayer(layer("foreground"), layer("background1"));
//}
//
//
//void IESCartesianWidget::updateIES(double angle)
//{
//    //clearPlottables();
//    //clearGraphs();
//
//    graph0_180->data().clear();
//    graph90_270->data().clear();
//
//    /// ///////////////////////////////////////////////////////////////
//    
//
//    if (IESLoader::instance().light.m_IESType > 4)
//    {
//        auto profile0_180 = generateC0C180Profile(angle);
//        // 创建0-180°曲线
//        QVector<double> angles0_180, values0_180;
//        for (const auto& point : profile0_180) {
//            angles0_180.append(point.x());
//            values0_180.append(point.y());
//        }
//        graph0_180->setData(angles0_180, values0_180);
//        QString cname = QString("C%1").arg(angle);
//        graph0_180->setName(cname);
//
//
//        double graph90_270_angle = 90 + angle;
//        if (graph90_270_angle > 360)graph90_270_angle -= 360;
//        cname = QString("C%1").arg(graph90_270_angle);
//        QVector<double> angles90_270, values90_270;
//        graph90_270->setData(angles90_270, values90_270);
//        graph90_270->setName(cname);
//    }
//    else
//    {
//        auto profile0_180 = generateC0C180Profile(angle);
//        // 创建0-180°曲线
//        QVector<double> angles0_180, values0_180;
//        for (const auto& point : profile0_180) {
//            angles0_180.append(point.x());
//            values0_180.append(point.y());
//        }
//   
//        graph0_180->setData(angles0_180, values0_180);
//        QString cname = QString("C%1").arg(angle);
//        graph0_180->setName(cname);
//
//        // 生成90-270°剖面数据 (C90° - C270°)
//        auto profile90_270 = generateC90C270Profile(angle);
//        //// 创建90-270°曲线
//        QVector<double> angles90_270, values90_270;
//        for (const auto& point : profile90_270) {
//            angles90_270.append(point.x());
//            values90_270.append(point.y());
//        }
//        //graph90_270->setBrush(Qt::NoBrush);
//        graph90_270->setData(angles90_270, values90_270);
//        double graph90_270_angle = 90 + angle;
//        if (graph90_270_angle > 360)graph90_270_angle -= 360;
//        cname = QString("C%1").arg(graph90_270_angle);
//        graph90_270->setName(cname);
//    }
//    rescaleAxes();
//    replot();
//}
//
//
//// 生成0-180°剖面数据
//std::vector<QPointF> IESCartesianWidget::generateC0C180Profile(double angle) {
//    std::vector<QPointF> profile;
//
//    std::vector<double> r1;
//    std::vector<double> r2;
//
//    int oneEightyPhiIndex = IESLoader::instance().findPhiIndex(angle);
//    if (oneEightyPhiIndex >= 0) {
//        r1 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
//    }
//    std::vector<QPointF> profile1;
//    profile1.reserve(IESLoader::instance().newThetas_r1.size());
//
//    for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
//        profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r1[i]));
//    }
//    profile.insert(profile.end(), profile1.begin(), profile1.end());
//    if (IESLoader::instance().light.m_IESType <= 4)
//    {
//        int twoEightyPhiIndex = angle + 180;
//        if (twoEightyPhiIndex >= 360)twoEightyPhiIndex = twoEightyPhiIndex - 360;
//        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);
//        
//        if (twoEightyPhiIndex >= 0) {
//            r2 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
//            std::reverse(r2.begin(), r2.end());
//            // 去掉第一个点
//            if (r2.size() > 1) {
//                r2.erase(r2.begin());
//            }
//        }
//        
//        std::vector<QPointF> profile2;
//        profile2.reserve(IESLoader::instance().newThetas_r2.size());
//        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
//            profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i]-360, r2[i]));
//        }
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//
//    return profile;
//}
//// 生成90-270°剖面数据
//std::vector<QPointF> IESCartesianWidget::generateC90C270Profile(double angle) {
//    std::vector<QPointF> profile;
//
//    std::vector<double> r3;
//    std::vector<double> r4;
//
//    if (IESLoader::instance().light.m_IESType <= 4)
//    {
//        int oneEightyPhiIndex = 90 + angle;
//        if (oneEightyPhiIndex >= 360)oneEightyPhiIndex = oneEightyPhiIndex - 360;
//        oneEightyPhiIndex = IESLoader::instance().findPhiIndex(oneEightyPhiIndex);
//        if (oneEightyPhiIndex >= 0) {
//            r3 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
//        }
//        std::vector<QPointF> profile1;
//        profile1.reserve(IESLoader::instance().newThetas_r1.size());
//        for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
//            if (std::isnan(r3[i]))
//                ;
//            else
//                profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r3[i]));
//        }
//
//        profile.insert(profile.end(), profile1.begin(), profile1.end());
//        // /////////////////////////////////////
//        int twoEightyPhiIndex = angle + 270;
//        if (twoEightyPhiIndex >= 360)twoEightyPhiIndex = twoEightyPhiIndex - 360;
//        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);
//     
//        if (twoEightyPhiIndex >= 0) {
//            r4 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
//            std::reverse(r4.begin(), r4.end());
//            // 去掉第一个点
//            if (r4.size() > 1) {
//                r4.erase(r4.begin());
//            }
//        }
//        std::vector<QPointF> profile2;
//        profile2.reserve(IESLoader::instance().newThetas_r2.size());
//        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
//            if (std::isnan(r4[i]))
//                ;
//            else
//                profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i] - 360, r4[i]));
//        }
//
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//    return profile;
//}
//
//
//void IESCartesianWidget::on_chkFillBlue_stateChanged(int value)
//{
//   
//    if (value)
//    {
//        graph0_180->setBrush(QBrush(QColor(0, 0, 255, 80)));
//    }
//    else
//    {
//        graph0_180->setBrush(Qt::NoBrush);
//    }   
//    graph0_180->setPen(QPen(Qt::blue, 2)); // 线宽和颜色
//    replot();
//}
//void IESCartesianWidget::on_chkFillRed_stateChanged(int value)
//{
//    
//    if (value)
//    {
//        graph90_270->setBrush(QBrush(QColor(255, 0, 0, 80)));
//    }
//    else
//    {
//        graph90_270->setBrush(Qt::NoBrush);
//    }
//    graph90_270->setPen(QPen(Qt::red, 2)); // 线宽和颜色
//    replot();
//}
//void IESCartesianWidget::on_chkViewRed_stateChanged(int value)
//{
//    
//    if (value)
//    {
//        graph90_270->setVisible(true);
//    }
//    else
//    {
//        graph90_270->setVisible(false);
//    }
//
//    replot();
//}
//
//void IESCartesianWidget::on_chkFillGreen_stateChanged(int value)
//{
//   
//    if (value)
//    {
//        graph0_180->setBrush(QBrush(QColor(0, 255, 0, 80)));
//    }
//    else
//    {
//        graph0_180->setBrush(Qt::NoBrush);
//    }
//    graph0_180->setPen(QPen(Qt::green, 2)); // 线宽和颜色
//    replot();
//}
//
//void IESCartesianWidget::on_chkFillYellow_stateChanged(int value)
//{
//   
//    if (value)
//    {
//        graph90_270->setBrush(QBrush(QColor(190, 190, 71,80)));
//    }
//    else
//    {
//        graph90_270->setBrush(Qt::NoBrush);
//    }
//    graph90_270->setPen(QPen(QColor(190, 190, 71), 2)); // 线宽和颜色
//    replot();
//}
//
//void IESCartesianWidget::on_chkViewYellow_stateChanged(int value)
//{
//    if (value)
//    {
//        graph90_270->setVisible(true);
//    }
//    else
//    {
//        graph90_270->setVisible(false);
//    }
//
//
//    replot();
//}
//
//void IESCartesianWidget::on_horizontalSlider_valueChanged(int value)
//{
//    //if (IESLoader::instance().light.m_IESType <= 4)
//    //{
//    //    value 
//    //}
//    graph0_180->data().clear();
//
//    auto profile0_180 = generateC0C180Profile(value);
//    // 创建0-180°曲线
//    QVector<double> angles0_180, values0_180;
//    for (const auto& point : profile0_180) {
//        angles0_180.append(point.x());
//        values0_180.append(point.y());
//    }
//    //graph0_180->setBrush(Qt::NoBrush);
//    graph0_180->setData(angles0_180, values0_180);
//    QString cname = QString("C%1").arg(value);
//    graph0_180->setName(cname);
//
//    graph90_270->data()->clear();
//
//    auto profile90_270 = generateC90C270Profile(value);
//
//    //// 创建90-270°曲线
//    QVector<double> angles90_270, values90_270;
//    for (const auto& point : profile90_270) {
//        angles90_270.append(point.x());
//        values90_270.append(point.y());
//    }
//    //graph90_270->setBrush(Qt::NoBrush);
//    graph90_270->setData(angles90_270, values90_270);
//
//    double graph90_270_angle = 90 + value;
//    if (graph90_270_angle > 360)graph90_270_angle -= 360;
//    cname = QString("C%1").arg(graph90_270_angle);
//    graph90_270->setName(cname);
//
//    rescaleAxes();
//    replot();
//}
//
//void IESCartesianWidget::on_horizontalSlider_2_valueChanged(int value)
//{
//    //graph90_270->data()->clear();
//
//    //auto profile90_270 = generateC90C270Profile(value);
//
//    ////// 创建90-270°曲线
//    //QVector<double> angles90_270, values90_270;
//    //for (const auto& point : profile90_270) {
//    //    angles90_270.append(point.x());
//    //    values90_270.append(point.y());
//    //}
//    ////graph90_270->setBrush(Qt::NoBrush);
//    //graph90_270->setData(angles90_270, values90_270);
//    ////graph90_270->setPen(QPen(Qt::blue, 2));
//    //graph90_270->setName("C90° - C270°");
//
//    //rescaleAxes();
//    //replot();
//}

#include "IESCartesianWidget.h"
#include "IESLoader.h"
#include <qwt_symbol.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_renderer.h>
#include <QDebug>
#include <cmath>
#include <algorithm>

IESCartesianWidget::IESCartesianWidget(QWidget* parent)
    : QwtPlot(parent)
    , m_curve0_180(nullptr)
    , m_curve90_270(nullptr)
    , m_grid(nullptr)
    , m_zoomer(nullptr)
    , m_panner(nullptr)
    , m_magnifier(nullptr)
{
    initPlot();
}

IESCartesianWidget::~IESCartesianWidget()
{
    // Qwt会自动管理附加对象的生命周期
}

void IESCartesianWidget::initPlot()
{
    // 设置画布背景
    setCanvasBackground(Qt::white);

    // 设置坐标轴标题
    setAxisTitle(QwtPlot::xBottom, "角度 (度)");
    setAxisTitle(QwtPlot::yLeft, "光强 (坎德拉)");

    // 设置坐标轴范围
    setAxisScale(QwtPlot::xBottom, -180, 180);
    setAxisScale(QwtPlot::yLeft, 0, 100);

    // 启用自动缩放
    setAxisAutoScale(QwtPlot::xBottom, true);
    setAxisAutoScale(QwtPlot::yLeft, true);

    // 设置网格
    m_grid = new QwtPlotGrid();
    m_grid->setPen(QPen(Qt::gray, 0, Qt::DotLine));
    m_grid->attach(this);

    // 创建第一条曲线 (C0° - C180°)
    m_curve0_180 = new FilledCurve();
    m_pen0_180 = QPen(Qt::blue, 2);
    m_curve0_180->setPen(m_pen0_180);
    m_curve0_180->setStyle(QwtPlotCurve::Lines);
    m_curve0_180->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
        QBrush(Qt::blue),
        QPen(Qt::blue, 1),
        QSize(2, 2)));
    m_brush0_180 = QBrush(Qt::NoBrush);
    m_curve0_180->setFillBrush(m_brush0_180);
    m_curve0_180->attach(this);

    // 创建第二条曲线 (C90° - C270°)
    m_curve90_270 = new FilledCurve();
    m_pen90_270 = QPen(Qt::red, 2);
    m_curve90_270->setPen(m_pen90_270);
    m_curve90_270->setStyle(QwtPlotCurve::Lines);
    m_curve90_270->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
        QBrush(Qt::red),
        QPen(Qt::red, 1),
        QSize(2, 2)));
    m_brush90_270 = QBrush(Qt::NoBrush);
    m_curve90_270->setFillBrush(m_brush90_270);
    m_curve90_270->attach(this);

    // 显示图例
    QwtLegend* legend = new QwtLegend();
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    insertLegend(legend, QwtPlot::RightLegend);

    // 设置绘图区域布局
    plotLayout()->setAlignCanvasToScales(true);

    // 添加缩放功能
    m_zoomer = new QwtPlotZoomer(canvas());
    m_zoomer->setRubberBandPen(QPen(Qt::darkBlue, 2, Qt::DotLine));
    m_zoomer->setTrackerPen(QPen(Qt::darkBlue));
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);

    // 添加平移功能
    m_panner = new QwtPlotPanner(canvas());

    // 添加鼠标滚轮缩放功能
    m_magnifier = new QwtPlotMagnifier(canvas());

    // 设置自动重绘
    setAutoReplot(true);
}

void IESCartesianWidget::updateIES(double angle)
{
    // 更新曲线数据
    if (IESLoader::instance().light.m_IESType > 4)
    {
        // 类型大于4：只显示C0-C180剖面
        auto profile0_180 = generateC0C180Profile(angle);
        auto profile90_270 = generateC90C270Profile(angle);
        // 准备数据
        QVector<QPointF> points0_180;
        QVector<QPointF> points90_270;

        points0_180.reserve(profile0_180.size());
        for (const auto& point : profile0_180) {
            points0_180.append(point);
        }
        for (const auto& point : profile90_270) {
            points90_270.append(point);
        }
        m_curve0_180->setSamples(points0_180);
        m_curve90_270->setSamples(points90_270);

        int oneEightyPhiIndex = angle;
        if (oneEightyPhiIndex >= 180) {
            oneEightyPhiIndex -= 360;
        }
        m_curve0_180->setTitle(QString("B %1 deg").arg(oneEightyPhiIndex));
        m_curve0_180->legendChanged();


        int twoEightyPhiIndex = angle + 90;
        if (twoEightyPhiIndex >= 180) {
            twoEightyPhiIndex -= 360;
        }
        m_curve90_270->setTitle(QString("B %1 deg").arg(twoEightyPhiIndex));
        m_curve90_270->legendChanged();

    }
    else
    {
        // 类型1-4：显示两个剖面
        auto profile0_180 = generateC0C180Profile(angle);
        auto profile90_270 = generateC90C270Profile(angle);

        // 准备数据
        QVector<QPointF> points0_180;
        QVector<QPointF> points90_270;

        points0_180.reserve(profile0_180.size());
        points90_270.reserve(profile90_270.size());

        for (const auto& point : profile0_180) {
            points0_180.append(point);
        }
        for (const auto& point : profile90_270) {
            points90_270.append(point);
        }
        
        m_curve0_180->setSamples(points0_180);
        m_curve90_270->setSamples(points90_270);

        m_curve0_180->setTitle(QString("C %1 deg").arg(angle));
        m_curve0_180->legendChanged();


        int twoEightyPhiIndex = angle + 90;
        if (twoEightyPhiIndex >= 360) {
            twoEightyPhiIndex -= 360;
        }
        m_curve90_270->setTitle(QString("C %1 deg").arg(twoEightyPhiIndex));
        m_curve90_270->legendChanged();

        //m_curve0_180->setSamples(points0_180);
        //QString cname = QString("C%1").arg(angle);
        //m_curve0_180->setTitle(cname);

        //m_curve90_270->setSamples(points90_270);
        //double graph90_270_angle = 90 + angle;
        //if (graph90_270_angle > 360) graph90_270_angle -= 360;
        //cname = QString("C%1").arg(graph90_270_angle);
        //m_curve90_270->setTitle(cname);
        //m_curve90_270->setVisible(true);
    }

    // 自动调整坐标轴范围
    replot();
}

std::vector<QPointF> IESCartesianWidget::generateC0C180Profile(double angle)
{
    std::vector<QPointF> profile;

    std::vector<double> r1;
    std::vector<double> r2;

    int oneEightyPhiIndex = IESLoader::instance().findPhiIndex(angle);
    if (oneEightyPhiIndex >= 0) {
        r1 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
    }

    std::vector<QPointF> profile1;
    profile1.reserve(IESLoader::instance().newThetas_r1.size());

    for (size_t i = 0; i < IESLoader::instance().newThetas_r1.size() && i < r1.size(); ++i) {
        profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r1[i]));
    }
    profile.insert(profile.end(), profile1.begin(), profile1.end());

    //if (IESLoader::instance().light.m_IESType <= 4)
    {
        int twoEightyPhiIndex = static_cast<int>(angle + 180);
        if (twoEightyPhiIndex >= 360) twoEightyPhiIndex -= 360;
        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);

        if (twoEightyPhiIndex >= 0) {
            r2 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
            std::reverse(r2.begin(), r2.end());
            // 去掉第一个点
            if (r2.size() > 1) {
                r2.erase(r2.begin());
            }
        }

        std::vector<QPointF> profile2;
        profile2.reserve(IESLoader::instance().newThetas_r2.size());
        for (size_t i = 0; i < IESLoader::instance().newThetas_r2.size() && i < r2.size(); ++i) {
            profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i] - 360, r2[i]));
        }
        profile.insert(profile.end(), profile2.begin(), profile2.end());
    }

    return profile;
}

std::vector<QPointF> IESCartesianWidget::generateC90C270Profile(double angle)
{
    std::vector<QPointF> profile;

    std::vector<double> r3;
    std::vector<double> r4;

    //if (IESLoader::instance().light.m_IESType <= 4)
    {
        int oneEightyPhiIndex = static_cast<int>(90 + angle);
        if (oneEightyPhiIndex >= 360) oneEightyPhiIndex -= 360;
        oneEightyPhiIndex = IESLoader::instance().findPhiIndex(oneEightyPhiIndex);
        if (oneEightyPhiIndex >= 0) {
            r3 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
        }

        std::vector<QPointF> profile1;
        profile1.reserve(IESLoader::instance().newThetas_r1.size());
        for (size_t i = 0; i < IESLoader::instance().newThetas_r1.size() && i < r3.size(); ++i) {
            if (!std::isnan(r3[i])) {
                profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r3[i]));
            }
        }

        profile.insert(profile.end(), profile1.begin(), profile1.end());

        int twoEightyPhiIndex = static_cast<int>(angle + 270);
        if (twoEightyPhiIndex >= 360) twoEightyPhiIndex -= 360;
        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);

        if (twoEightyPhiIndex >= 0) {
            r4 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
            std::reverse(r4.begin(), r4.end());
            // 去掉第一个点
            if (r4.size() > 1) {
                r4.erase(r4.begin());
            }
        }

        std::vector<QPointF> profile2;
        profile2.reserve(IESLoader::instance().newThetas_r2.size());
        for (size_t i = 0; i < IESLoader::instance().newThetas_r2.size() && i < r4.size(); ++i) {
            if (!std::isnan(r4[i])) {
                profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i] - 360, r4[i]));
            }
        }

        profile.insert(profile.end(), profile2.begin(), profile2.end());
    }
    return profile;
}

void IESCartesianWidget::on_chkFillBlue_stateChanged(int value)
{
    if (value) {
        m_brush0_180 = QBrush(QColor(0, 0, 255, 80));
    }
    else {
        m_brush0_180 = QBrush(Qt::NoBrush);
    }

    if (m_curve0_180) {
        m_curve0_180->setFillBrush(m_brush0_180);
        m_curve0_180->setPen(m_pen0_180);
        replot();
    }
}

void IESCartesianWidget::on_chkFillRed_stateChanged(int value)
{
    if (value) {
        m_brush90_270 = QBrush(QColor(255, 0, 0, 80));
    }
    else {
        m_brush90_270 = QBrush(Qt::NoBrush);
    }

    if (m_curve90_270) {
        m_curve90_270->setFillBrush(m_brush90_270);
        m_curve90_270->setPen(m_pen90_270);
        replot();
    }
}

void IESCartesianWidget::on_chkViewRed_stateChanged(int value)
{
    if (m_curve90_270) {
        m_curve90_270->setVisible(value);
        replot();
    }
}

void IESCartesianWidget::on_chkFillGreen_stateChanged(int value)
{
    if (value) {
        m_brush0_180 = QBrush(QColor(0, 255, 0, 80));
        m_pen0_180 = QPen(Qt::green, 2);
    }
    else {
        m_brush0_180 = QBrush(Qt::NoBrush);
        m_pen0_180 = QPen(Qt::blue, 2);
    }

    if (m_curve0_180) {
        m_curve0_180->setFillBrush(m_brush0_180);
        m_curve0_180->setPen(m_pen0_180);
        replot();
    }
}

void IESCartesianWidget::on_chkFillYellow_stateChanged(int value)
{
    if (value) {
        m_brush90_270 = QBrush(QColor(190, 190, 71, 80));
        m_pen90_270 = QPen(QColor(190, 190, 71), 2);
    }
    else {
        m_brush90_270 = QBrush(Qt::NoBrush);
        m_pen90_270 = QPen(Qt::red, 2);
    }

    if (m_curve90_270) {
        m_curve90_270->setFillBrush(m_brush90_270);
        m_curve90_270->setPen(m_pen90_270);
        replot();
    }
}

void IESCartesianWidget::on_chkViewYellow_stateChanged(int value)
{
    if (m_curve90_270) {
        m_curve90_270->setVisible(value);
        replot();
    }
}

void IESCartesianWidget::on_horizontalSlider_valueChanged(int value)
{
    updateIES(value);
}

void IESCartesianWidget::on_horizontalSlider_2_valueChanged(int value)
{
    // 如果需要单独控制第二个剖面，可以在这里实现
    Q_UNUSED(value);
}