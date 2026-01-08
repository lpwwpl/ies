//#include "IESPolarWidget.h"
//#include "IESLoader.h"
//#include <QDebug>
//
//IESPolarWidget::IESPolarWidget()
//{
//    Init();
//
//}
//
//IESPolarWidget::~IESPolarWidget()
//{
//
//}
//
//
//void IESPolarWidget::Init()
//{
//
//    legend->setVisible(true);
//    //legend->setPositionStyle(QCPLegend::psTopRight);
//
//    // 图例样式
//    legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
//    legend->setBorderPen(QPen(Qt::darkGray));
//    legend->setFont(QFont("Arial", 9));
//
//    QFont font;
//    font.setStyleStrategy(QFont::NoAntialias);
//    font.setFamily("Microsoft YaHei");
//    font.setPixelSize(10);
//
//    //极坐标
//    this->plotLayout()->clear(); // 清空默认布局
//    angularAxis = new QCPPolarAxisAngular(this);
//    this->plotLayout()->addElement(0, 0, angularAxis); // 将角度轴添加到布局中心
//
//    // 3. 创建径向轴 (从中心向外)
//    radialAxis = angularAxis->radialAxis();
//    angularAxis->setTickLabelFont(font);
//    angularAxis->setLabel(tr("角度轴 (θ)"));
//    angularAxis->setRange(0, 360); // 角度范围 (0-360度)
//    angularAxis->grid()->setVisible(true);
//    angularAxis->setTicks(true);
//   
//    addLayer("background1", nullptr, QCustomPlot::limBelow);
//    addLayer("foreground", nullptr, QCustomPlot::limAbove);
//
//
//    radialAxis->setLabel(tr("径向轴 (r)"));
//    radialAxis->setAngle(0);
//
//    radialAxis->setSubTicks(false);
//    radialAxis->setTickLabelFont(font);
//    graph0_180 = new QCPPolarGraph(angularAxis, radialAxis);
//    graph0_180->setLayer("foreground");
//
//    // 8. 设置图形样式 (可选)
//    graph0_180->setLineStyle(QCPPolarGraph::lsLine); // 线型
//    graph0_180->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
//    graph0_180->setPen(QPen(Qt::blue, 2)); // 线宽和颜色
//    //graph0_180->setBrush(QBrush(QColor(0, 0, 250, 150)));
//
//    graph90_270 = new QCPPolarGraph(angularAxis, radialAxis);
//    graph90_270->setLayer("background1");
//    // 8. 设置图形样式 (可选)
//    graph90_270->setLineStyle(QCPPolarGraph::lsLine); // 线型
//    graph90_270->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
//    graph90_270->setPen(QPen(Qt::red, 2)); // 线宽和颜色
//    //graph90_270->setBrush(QBrush(QColor(255, 0, 0, 150)));
//
//
//
//    moveLayer(layer("foreground"), layer("background1"));
//}
//
//
//void IESPolarWidget::updateIES(double angle)
//{
//    radialAxis->setRange(0, IESLoader::instance().light.max_candela * IESLoader::instance().light.multiplier);
//    clearPlottables();
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
//
//        graph0_180->setData(angles0_180, values0_180);
//        graph0_180->setName("C0° - C180°");
//   
//
//
//        QVector<double> angles90_270, values90_270;
//        graph90_270->setData(angles90_270, values90_270);
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
//        //graph0_180->setBrush(Qt::NoBrush);
//        graph0_180->setData(angles0_180, values0_180);
//        graph0_180->setName("C0° - C180°");
//
//        // 生成90-270°剖面数据 (C90° - C270°)
//
//        auto profile90_270 = generateC90C270Profile(angle);
//
//        //// 创建90-270°曲线
//        QVector<double> angles90_270, values90_270;
//        for (const auto& point : profile90_270) {
//            angles90_270.append(point.x());
//            values90_270.append(point.y());
//        }
//        graph90_270->setData(angles90_270, values90_270);
//        graph90_270->setName("C90° - C270°");
//    }
//    rescaleAxes();
//    replot();
//}
//
//
//// 生成0-180°剖面数据
//std::vector<QPointF> IESPolarWidget::generateC0C180Profile(double angle) {
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
//            profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], r2[i]));
//        }
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//
//    return profile;
//}
//// 生成90-270°剖面数据
//std::vector<QPointF> IESPolarWidget::generateC90C270Profile(double angle) {
//    std::vector<QPointF> profile;
//
//
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
//                profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], r4[i]));
//        }
//
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//    return profile;
//}
//
//
//void IESPolarWidget::on_chkFillBlue_stateChanged(int value)
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
//void IESPolarWidget::on_chkFillRed_stateChanged(int value)
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
//void IESPolarWidget::on_chkViewRed_stateChanged(int value)
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
//void IESPolarWidget::on_chkFillGreen_stateChanged(int value)
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
//void IESPolarWidget::on_chkFillYellow_stateChanged(int value)
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
//void IESPolarWidget::on_chkViewYellow_stateChanged(int value)
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
//void IESPolarWidget::on_horizontalSlider_valueChanged(int value)
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
//
//
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
//    //graph90_270->setPen(QPen(Qt::blue, 2));
//    graph90_270->setName("C90° - C270°");
//
//    rescaleAxes();
//    replot();
//}
//
//void IESPolarWidget::on_horizontalSlider_2_valueChanged(int value)
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

//#include "IESPolarWidget.h"
//#include <cmath>
//#include <algorithm>
//#include "IESLoader.h"
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif
//
//IESPolarWidget::IESPolarWidget(QWidget* parent)
//    : QwtPlot(parent)
//    , m_curve0_180(nullptr)
//    , m_curve90_270(nullptr)
//    , m_showCurve90_270(true)
//    , m_fill0_180(false)
//    , m_fill90_270(false)
//    , m_color0_180(Qt::blue)
//    , m_color90_270(Qt::red)
//    , m_maxRadius(100.0)
//    , m_currentAngle(0.0)
//{
//    Init();
//}
//
//IESPolarWidget::~IESPolarWidget()
//{
//    clearAll();
//}
//
//void IESPolarWidget::Init()
//{
//    // 设置画布背景
//    setCanvasBackground(Qt::white);
//    //canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
//    canvas()->setMinimumSize(400, 400);
//
//    // 设置图例
//    QwtLegend* legend = new QwtLegend();
//    legend->setDefaultItemMode(QwtLegendData::Checkable);
//    insertLegend(legend, QwtPlot::RightLegend);
//
//    // 设置图例样式
//    //legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
//    //legend->setBorderPen(QPen(Qt::darkGray));
//    legend->setFont(QFont("Arial", 9));
//
//    // 设置坐标轴
//    setAxisTitle(QwtPlot::xBottom, "");
//    setAxisTitle(QwtPlot::yLeft, "");
//
//    // 设置坐标轴范围（极坐标图通常显示在单位圆内）
//    setAxisScale(QwtPlot::xBottom, -1.1, 1.1);
//    setAxisScale(QwtPlot::yLeft, -1.1, 1.1);
//
//    // 隐藏坐标轴
//    enableAxis(QwtPlot::xBottom, false);
//    enableAxis(QwtPlot::yLeft, false);
//    enableAxis(QwtPlot::xTop, false);
//    enableAxis(QwtPlot::yRight, false);
//
//    // 初始化网格标记
//    updateRadialMarkers();
//    updateAngularMarkers();
//}
//
//void IESPolarWidget::clearAll()
//{
//
//    // 清理曲线
//    if (m_curve0_180) {
//        m_curve0_180->detach();
//        delete m_curve0_180;
//        m_curve0_180 = nullptr;
//    }
//    if (m_curve90_270) {
//        m_curve90_270->detach();
//        delete m_curve90_270;
//        m_curve90_270 = nullptr;
//    }
//
//    // 清理网格曲线
//    for (auto curve : m_gridCurves) {
//        curve->detach();
//        delete curve;
//    }
//    m_gridCurves.clear();
//
//    // 清理标记
//    for (auto marker : m_radialMarkers) {
//        marker->detach();
//        delete marker;
//    }
//    m_radialMarkers.clear();
//
//    for (auto marker : m_angularMarkers) {
//        marker->detach();
//        delete marker;
//    }
//    m_angularMarkers.clear();
//}
//
//QPointF IESPolarWidget::polarToCartesian(double angle, double radius) const
//{
//    // 将角度从度转换为弧度
//    double radians = angle * M_PI / 180.0;
//
//    // 转换为直角坐标
//    double x = radius * cos(radians);
//    double y = radius * sin(radians);
//
//    return QPointF(x, y);
//}
//
//void IESPolarWidget::updateRadialMarkers()
//{
//    // 清理旧的径向标记
//    for (auto marker : m_radialMarkers) {
//        marker->detach();
//        delete marker;
//    }
//    m_radialMarkers.clear();
//
//    // 清理旧的网格曲线
//    for (auto curve : m_gridCurves) {
//        curve->detach();
//        delete curve;
//    }
//    m_gridCurves.clear();
//
//    // 创建径向圆环（网格）
//    int numCircles = 5;  // 显示5个同心圆
//    for (int i = 1; i <= numCircles; i++) {
//        QVector<QPointF> circlePoints;
//        double radius = i * 0.2;  // 归一化半径
//
//        // 生成圆上的点
//        for (int angle = 0; angle <= 360; angle += 5) {
//            circlePoints.append(polarToCartesian(angle, radius));
//        }
//
//        // 创建圆形曲线（网格线）
//        QwtPlotCurve* circle = new QwtPlotCurve();
//        circle->setSamples(circlePoints);
//        circle->setPen(QPen(Qt::lightGray, 0.5, Qt::DotLine));
//        circle->setItemAttribute(QwtPlotItem::Legend, false);
//        circle->attach(this);
//        m_gridCurves.append(circle);
//
//        // 添加半径标签
//        QwtPlotMarker* labelMarker = new QwtPlotMarker();
//        QPointF labelPos = polarToCartesian(0, radius + 0.05);  // 稍微超出圆
//        labelMarker->setValue(labelPos);
//
//        QwtText label(QString::number(i * 0.2 * m_maxRadius, 'f', 1));
//        label.setColor(Qt::darkGray);
//        label.setFont(QFont("Arial", 8));
//        labelMarker->setLabel(label);
//        labelMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//        labelMarker->attach(this);
//
//        m_radialMarkers.append(labelMarker);
//    }
//}
//
//void IESPolarWidget::updateAngularMarkers()
//{
//    // 清理旧的角度标记
//    for (auto marker : m_angularMarkers) {
//        marker->detach();
//        delete marker;
//    }
//    m_angularMarkers.clear();
//
//    // 创建角度标记（每30度一个）
//    for (int angle = 0; angle < 360; angle += 30) {
//        // 创建角度线（从中心到外圆）
//        QVector<QPointF> linePoints;
//        linePoints.append(polarToCartesian(angle, 0));
//        linePoints.append(polarToCartesian(angle, 1.0));
//
//        QwtPlotCurve* line = new QwtPlotCurve();
//        line->setSamples(linePoints);
//        line->setPen(QPen(Qt::lightGray, 0.5, Qt::DotLine));
//        line->setItemAttribute(QwtPlotItem::Legend, false);
//        line->attach(this);
//        m_gridCurves.append(line);
//
//        // 添加角度标签
//        QwtPlotMarker* labelMarker = new QwtPlotMarker();
//        QPointF labelPos = polarToCartesian(angle, 1.1);  // 稍微超出圆
//        labelMarker->setValue(labelPos);
//
//        QwtText label(QString("%1°").arg(angle));
//        label.setColor(Qt::darkGray);
//        label.setFont(QFont("Arial", 9));
//        labelMarker->setLabel(label);
//
//        // 根据角度调整标签对齐方式
//        if (angle == 0) {
//            labelMarker->setLabelAlignment(Qt::AlignHCenter | Qt::AlignTop);
//        }
//        else if (angle == 90) {
//            labelMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//        }
//        else if (angle == 180) {
//            labelMarker->setLabelAlignment(Qt::AlignHCenter | Qt::AlignBottom);
//        }
//        else if (angle == 270) {
//            labelMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
//        }
//        else if (angle > 0 && angle < 90) {
//            labelMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
//        }
//        else if (angle > 90 && angle < 180) {
//            labelMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
//        }
//        else if (angle > 180 && angle < 270) {
//            labelMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
//        }
//        else {
//            labelMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
//        }
//
//        labelMarker->attach(this);
//        m_angularMarkers.append(labelMarker);
//    }
//}
//
//void IESPolarWidget::updateIES(double angle)
//{
//    m_currentAngle = angle;
//
//    // 获取最大半径（最大光强值）
//    // 注意：这里需要根据您的实际情况获取最大半径
//    // m_maxRadius = IESLoader::instance().light.max_candela * IESLoader::instance().light.multiplier;
//
//    // 清理旧的曲线
//    if (m_curve0_180) {
//        m_curve0_180->detach();
//        delete m_curve0_180;
//        m_curve0_180 = nullptr;
//    }
//    if (m_curve90_270) {
//        m_curve90_270->detach();
//        delete m_curve90_270;
//        m_curve90_270 = nullptr;
//    }
//
//    // 更新网格标记
//    updateRadialMarkers();
//    updateAngularMarkers();
//
//    // 生成极坐标数据
//    auto profile0_180 = generateC0C180Profile(angle);
//    auto profile90_270 = generateC90C270Profile(angle);
//
//    // 计算最大半径（从数据中）
//    if (!profile0_180.empty() || !profile90_270.empty()) {
//        m_maxRadius = 0;
//        for (const auto& point : profile0_180) {
//            m_maxRadius = std::max(m_maxRadius, point.y());
//        }
//        for (const auto& point : profile90_270) {
//            m_maxRadius = std::max(m_maxRadius, point.y());
//        }
//    }
//
//    if (m_maxRadius <= 0) {
//        m_maxRadius = 100.0;  // 默认值
//    }
//
//    // 创建C0-C180曲线
//    if (!profile0_180.empty()) {
//        QVector<QPointF> cartesianPoints;
//        cartesianPoints.reserve(profile0_180.size());
//
//        for (const auto& polarPoint : profile0_180) {
//            double angleDeg = polarPoint.x();
//            double radius = polarPoint.y();
//            double normalizedRadius = radius / m_maxRadius;
//
//            // 限制半径在[0,1]范围内
//            if (normalizedRadius > 1.0) normalizedRadius = 1.0;
//
//            cartesianPoints.append(polarToCartesian(angleDeg, normalizedRadius));
//        }
//
//        m_curve0_180 = new QwtPlotCurve("C0° - C180°");
//        m_curve0_180->setSamples(cartesianPoints);
//        m_curve0_180->setPen(QPen(m_color0_180, 2));
//        m_curve0_180->setRenderHint(QwtPlotItem::RenderAntialiased);
//        m_curve0_180->setItemAttribute(QwtPlotItem::Legend, true);
//
//        if (m_fill0_180) {
//            m_curve0_180->setBrush(QBrush(QColor(m_color0_180.red(),
//                m_color0_180.green(),
//                m_color0_180.blue(), 80)));
//        }
//
//        m_curve0_180->attach(this);
//    }
//
//    // 创建C90-C270曲线
//    if (!profile90_270.empty() && m_showCurve90_270) {
//        QVector<QPointF> cartesianPoints;
//        cartesianPoints.reserve(profile90_270.size());
//
//        for (const auto& polarPoint : profile90_270) {
//            double angleDeg = polarPoint.x();
//            double radius = polarPoint.y();
//            double normalizedRadius = radius / m_maxRadius;
//
//            // 限制半径在[0,1]范围内
//            if (normalizedRadius > 1.0) normalizedRadius = 1.0;
//
//            cartesianPoints.append(polarToCartesian(angleDeg, normalizedRadius));
//        }
//
//        m_curve90_270 = new QwtPlotCurve("C90° - C270°");
//        m_curve90_270->setSamples(cartesianPoints);
//        m_curve90_270->setPen(QPen(m_color90_270, 2));
//        m_curve90_270->setRenderHint(QwtPlotItem::RenderAntialiased);
//        m_curve90_270->setItemAttribute(QwtPlotItem::Legend, true);
//
//        if (m_fill90_270) {
//            m_curve90_270->setBrush(QBrush(QColor(m_color90_270.red(),
//                m_color90_270.green(),
//                m_color90_270.blue(), 80)));
//        }
//
//        m_curve90_270->attach(this);
//    }
//
//    // 重绘
//    replot();
//}
//
//std::vector<QPointF> IESPolarWidget::generateC0C180Profile(double angle) {
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
//            profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], r2[i]));
//        }
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//
//    return profile;
//}
//// 生成90-270°剖面数据
//std::vector<QPointF> IESPolarWidget::generateC90C270Profile(double angle) {
//    std::vector<QPointF> profile;
//
//
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
//                profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], r4[i]));
//        }
//
//        profile.insert(profile.end(), profile2.begin(), profile2.end());
//    }
//    return profile;
//}
//
//// ==================== 槽函数实现 ====================
//
//void IESPolarWidget::on_chkFillBlue_stateChanged(int value)
//{
//    m_fill0_180 = (value != 0);
//    m_color0_180 = Qt::blue;
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_chkFillRed_stateChanged(int value)
//{
//    m_fill90_270 = (value != 0);
//    m_color90_270 = Qt::red;
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_chkViewRed_stateChanged(int value)
//{
//    m_showCurve90_270 = (value != 0);
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_chkFillGreen_stateChanged(int value)
//{
//    m_fill0_180 = (value != 0);
//    m_color0_180 = Qt::green;
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_chkFillYellow_stateChanged(int value)
//{
//    m_fill90_270 = (value != 0);
//    m_color90_270 = QColor(190, 190, 71);
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_chkViewYellow_stateChanged(int value)
//{
//    m_showCurve90_270 = (value != 0);
//    updateIES(m_currentAngle);
//}
//
//void IESPolarWidget::on_horizontalSlider_valueChanged(int value)
//{
//    updateIES(value);
//}
//
//void IESPolarWidget::on_horizontalSlider_2_valueChanged(int value)
//{
//    // 这里可以实现其他功能，比如调整最大半径的缩放
//    // 暂时只更新图表
//    updateIES(m_currentAngle);
//}


#include "IESPolarWidget.h"
#include "IESLoader.h"
#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>
#include <qwt_painter.h>
#include <qwt_series_data.h>
#include <QDebug>
#include <cmath>
#include <qwt_symbol.h>


IESPolarWidget::IESPolarWidget(QWidget* parent)
    : QwtPolarPlot(parent)
    , m_curve0_180(nullptr)
    , m_curve90_270(nullptr)
    , m_grid(nullptr)
{
    initPlot();
}

IESPolarWidget::~IESPolarWidget()
{
    // Qwt会自动管理其对象的生命周期
}

void IESPolarWidget::initPlot()
{

    // 设置角度轴
    setScale(QwtPolar::Azimuth,
        0,      // 最小值
        360,    // 最大值
        30);    // 主刻度间隔

// 设置径向轴范围，稍后会在updateIES中更新
    setScale(QwtPolar::Radius, 0, 100, 20);

    // 设置极坐标网格
    m_grid = new QwtPolarGrid();
    m_grid->setPen(QPen(Qt::gray, 0, Qt::DotLine));

    // 显示角度网格线
    for (int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++) {
        m_grid->showGrid(scaleId);
    }

    // 设置网格的刻度样式
    m_grid->setAxisPen(QwtPolar::AxisAzimuth, QPen(Qt::black, 2));
    m_grid->setAxisPen(QwtPolar::AxisLeft, QPen(Qt::black, 2));
    m_grid->setAxisPen(QwtPolar::AxisRight, QPen(Qt::black, 2));
    m_grid->setAxisPen(QwtPolar::AxisTop, QPen(Qt::black, 2));
    m_grid->setAxisPen(QwtPolar::AxisBottom, QPen(Qt::black, 2));

    m_grid->attach(this);

    // 创建第一条曲线 (C0° - C180°)，使用自定义的填充曲线类
    m_curve0_180 = new FilledPolarCurve("C0-180°");
    m_pen0_180 = QPen(Qt::blue, 2);
    m_curve0_180->setPen(m_pen0_180);
    m_curve0_180->setStyle(QwtPolarCurve::CurveStyle::Lines);
    m_curve0_180->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
        QBrush(Qt::blue),
        QPen(Qt::blue, 1),
        QSize(2, 2)));
    m_brush0_180 = QBrush(Qt::NoBrush);
    static_cast<FilledPolarCurve*>(m_curve0_180)->setFillBrush(m_brush0_180);

    // 创建第二条曲线 (C90° - C270°)
    m_curve90_270 = new FilledPolarCurve("C90-C270°");
    m_pen90_270 = QPen(Qt::red, 2);
    m_curve90_270->setPen(m_pen90_270);
    m_curve90_270->setStyle(QwtPolarCurve::CurveStyle::Lines);
    m_curve90_270->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
        QBrush(Qt::red),
        QPen(Qt::red, 1),
        QSize(2, 2)));
    m_brush90_270 = QBrush(Qt::NoBrush);
    static_cast<FilledPolarCurve*>(m_curve90_270)->setFillBrush(m_brush90_270);

    // 将曲线附加到图上
    m_curve0_180->attach(this);
    m_curve90_270->attach(this);

    // 显示图例
    m_legend = new QwtLegend();
    m_legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    insertLegend(m_legend, QwtPolarPlot::RightLegend);

    // 设置画布背景
    //setCanvasBackground(QBrush(Qt::white));

    // 设置自动重绘策略
    setAutoReplot(true);
}

void IESPolarWidget::updateIES(double angle)
{
    // 更新径向轴范围
    double maxRadius = IESLoader::instance().light.max_candela * IESLoader::instance().light.multiplier;
    setScale(QwtPolar::Radius, 0, maxRadius, maxRadius / 5);

    // 根据IES类型生成数据
    if (IESLoader::instance().light.m_IESType > 4)
    {
        // 类型大于4：只显示C0-C180剖面
        auto profile0_180 = generateC0C180Profile(angle);
        auto profile90_270 = generateC90C270Profile(angle);

        // 创建数据对象
        PolarSeriesData* data0_180 = new PolarSeriesData(profile0_180);
        PolarSeriesData* data90_270 = new PolarSeriesData(profile90_270);


        m_curve0_180->setData(data0_180);
        m_curve90_270->setData(data90_270);
        //m_curve0_180->setVisible(true);
        //m_curve90_270->setVisible(true);


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

        static_cast<FilledPolarCurve*>(m_curve0_180)->m_angle = M_PI ;
        static_cast<FilledPolarCurve*>(m_curve90_270)->m_angle = M_PI ;
    }
    else
    {
        // 类型1-4：显示两个剖面
        auto profile0_180 = generateC0C180Profile(angle);
        auto profile90_270 = generateC90C270Profile(angle);

        // 创建数据对象
        PolarSeriesData* data0_180 = new PolarSeriesData(profile0_180);
        PolarSeriesData* data90_270 = new PolarSeriesData(profile90_270);

        m_curve0_180->setData(data0_180);
        m_curve90_270->setData(data90_270);

        //m_curve0_180->setVisible(true);
        //m_curve90_270->setVisible(true);

     
        m_curve0_180->setTitle(QString("C %1 deg").arg(angle));
        m_curve0_180->legendChanged();


        int twoEightyPhiIndex = angle + 90;
        if (twoEightyPhiIndex >= 360) {
            twoEightyPhiIndex -= 360;
        }
        m_curve90_270->setTitle(QString("C %1 deg").arg(twoEightyPhiIndex));
        m_curve90_270->legendChanged();

        static_cast<FilledPolarCurve*>(m_curve0_180)->m_angle = M_PI / 2;
        static_cast<FilledPolarCurve*>(m_curve90_270)->m_angle = M_PI / 2;
    }

    // 更新显示
    replot();
}

QVector<QwtPointPolar> IESPolarWidget::generateC0C180Profile(double angle)
{
    QVector<QwtPointPolar> profile;

    std::vector<double> r1;
    std::vector<double> r2;

    int oneEightyPhiIndex = IESLoader::instance().findPhiIndex(angle);
    if (oneEightyPhiIndex >= 0) {
        r1 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
    }

    // 添加第一部分数据
    for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
        double theta = IESLoader::instance().newThetas_r1[i];
        double radius = r1[i];
        profile.push_back(QwtPointPolar(theta, radius));
    }

    // 如果是类型1-4，添加第二部分数据
    //if (IESLoader::instance().light.m_IESType <= 4) 
    {
        int twoEightyPhiIndex = angle + 180;
        if (twoEightyPhiIndex >= 360) {
            twoEightyPhiIndex -= 360;
        }
        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);

        if (twoEightyPhiIndex >= 0) {
            r2 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
            std::reverse(r2.begin(), r2.end());
            if (r2.size() > 1) {
                r2.erase(r2.begin());
            }
        }

        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
            double theta = IESLoader::instance().newThetas_r2[i];
            double radius = r2[i];
            profile.push_back(QwtPointPolar(theta, radius));
        }
    }

    return profile;
}

QVector<QwtPointPolar> IESPolarWidget::generateC90C270Profile(double angle)
{
    QVector<QwtPointPolar> profile;

    //if (IESLoader::instance().light.m_IESType <= 4) 
    {
        std::vector<double> r3;
        std::vector<double> r4;

        int oneEightyPhiIndex = 90 + angle;
        if (oneEightyPhiIndex >= 360) {
            oneEightyPhiIndex -= 360;
        }
        oneEightyPhiIndex = IESLoader::instance().findPhiIndex(oneEightyPhiIndex);

        if (oneEightyPhiIndex >= 0) {
            r3 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
        }

        // 添加第一部分数据
        for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
            if (!std::isnan(r3[i])) {
                double theta = IESLoader::instance().newThetas_r1[i];
                double radius = r3[i];
                profile.push_back(QwtPointPolar(theta, radius));
            }
        }

        int twoEightyPhiIndex = angle + 270;
        if (twoEightyPhiIndex >= 360) {
            twoEightyPhiIndex -= 360;
        }
        twoEightyPhiIndex = IESLoader::instance().findPhiIndex(twoEightyPhiIndex);

        if (twoEightyPhiIndex >= 0) {
            r4 = IESLoader::instance().newValues_all[twoEightyPhiIndex];
            std::reverse(r4.begin(), r4.end());
            if (r4.size() > 1) {
                r4.erase(r4.begin());
            }
        }

        // 添加第二部分数据
        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
            if (!std::isnan(r4[i])) {
                double theta = IESLoader::instance().newThetas_r2[i];
                double radius = r4[i];
                profile.push_back(QwtPointPolar(theta, radius));
            }
        }
    }

    return profile;
}

void IESPolarWidget::on_chkFillBlue_stateChanged(int value)
{
    if (value) {
        m_brush0_180 = QBrush(QColor(0, 0, 255, 80));
    }
    else {
        m_brush0_180 = QBrush(Qt::NoBrush);
    }

    if (m_curve0_180) {
        static_cast<FilledPolarCurve*>(m_curve0_180)->setFillBrush(m_brush0_180);
        m_curve0_180->setPen(m_pen0_180);
        replot();
    }
}

void IESPolarWidget::on_chkFillRed_stateChanged(int value)
{
    if (value) {
        m_brush90_270 = QBrush(QColor(255, 0, 0, 80));
    }
    else {
        m_brush90_270 = QBrush(Qt::NoBrush);
    }

    if (m_curve90_270) {
        static_cast<FilledPolarCurve*>(m_curve90_270)->setFillBrush(m_brush90_270);
        m_curve90_270->setPen(m_pen90_270);
        replot();
    }
}

void IESPolarWidget::on_chkViewRed_stateChanged(int value)
{
    if (m_curve90_270) {
        m_curve90_270->setVisible(value);
        replot();
    }
}

void IESPolarWidget::on_chkFillGreen_stateChanged(int value)
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
        static_cast<FilledPolarCurve*>(m_curve0_180)->setFillBrush(m_brush0_180);
        m_curve0_180->setPen(m_pen0_180);
        replot();
    }
}

void IESPolarWidget::on_chkFillYellow_stateChanged(int value)
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
        static_cast<FilledPolarCurve*>(m_curve90_270)->setFillBrush(m_brush90_270);
        m_curve90_270->setPen(m_pen90_270);
        replot();
    }
}

void IESPolarWidget::on_chkViewYellow_stateChanged(int value)
{
    if (m_curve90_270) {
        m_curve90_270->setVisible(value);
        replot();
    }
}

void IESPolarWidget::on_horizontalSlider_valueChanged(int value)
{
    updateIES(value);
}

void IESPolarWidget::on_horizontalSlider_2_valueChanged(int value)
{
    // 如果需要单独控制第二个剖面，可以在这里实现
    // 当前实现中，两个剖面是同时更新的
}