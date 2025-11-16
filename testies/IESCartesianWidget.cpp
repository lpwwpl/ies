#include "IESCartesianWidget.h"
#include "IESLoader.h"
#include <QDebug>

IESCartesianWidget::IESCartesianWidget(QWidget* parent):QCustomPlot(parent)
{
    Init();

}

IESCartesianWidget::~IESCartesianWidget()
{

}


void IESCartesianWidget::Init()
{    
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(10);

    
    //this->plotLayout()->clear(); // 清空默认布局

   
    addLayer("background1", nullptr, QCustomPlot::limBelow);
    addLayer("foreground", nullptr, QCustomPlot::limAbove);



    graph0_180 = addGraph(xAxis, yAxis);
    graph0_180->setLayer("foreground");

    // 8. 设置图形样式 (可选)
    graph0_180->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
    graph0_180->setPen(QPen(Qt::blue, 2)); // 线宽和颜色
    //graph0_180->setBrush(QBrush(QColor(0, 0, 250, 150)));

    graph90_270 = addGraph(xAxis, yAxis);
    graph90_270->setLayer("background1");
    // 8. 设置图形样式 (可选)
    graph90_270->setScatterStyle(QCPScatterStyle::ssDot); // 散点样式
    graph90_270->setPen(QPen(Qt::red, 2)); // 线宽和颜色
    //graph90_270->setBrush(QBrush(QColor(255, 0, 0, 150)));


    moveLayer(layer("foreground"), layer("background1"));
}


void IESCartesianWidget::updateIES(double angle)
{
    //clearPlottables();
    //clearGraphs();

    graph0_180->data().clear();
    graph90_270->data().clear();

    /// ///////////////////////////////////////////////////////////////
    

    if (IESLoader::instance().light.m_IESType > 4)
    {
        auto profile0_180 = generateC0C180Profile(angle);
        // 创建0-180°曲线
        QVector<double> angles0_180, values0_180;
        for (const auto& point : profile0_180) {
            angles0_180.append(point.x());
            values0_180.append(point.y());
        }
        //graph0_180->setBrush(Qt::NoBrush);
        graph0_180->setData(angles0_180, values0_180);
        //graph0_180->setPen(QPen(Qt::red, 2));
        //graph0_180->setName("C0° - C180°");



        QVector<double> angles90_270, values90_270;
        graph90_270->setData(angles90_270, values90_270);
    }
    else
    {
        auto profile0_180 = generateC0C180Profile(angle);
        // 创建0-180°曲线
        QVector<double> angles0_180, values0_180;
        for (const auto& point : profile0_180) {
            angles0_180.append(point.x());
            values0_180.append(point.y());
        }
        //graph0_180->setBrush(Qt::NoBrush);
        graph0_180->setData(angles0_180, values0_180);
        //graph0_180->setPen(QPen(Qt::red, 2));
        //graph0_180->setName("C0° - C180°");

        // 生成90-270°剖面数据 (C90° - C270°)

        auto profile90_270 = generateC90C270Profile(angle);

        //// 创建90-270°曲线
        QVector<double> angles90_270, values90_270;
        for (const auto& point : profile90_270) {
            angles90_270.append(point.x());
            values90_270.append(point.y());
        }
        //graph90_270->setBrush(Qt::NoBrush);
        graph90_270->setData(angles90_270, values90_270);
        //graph90_270->setPen(QPen(Qt::blue, 2));
        //graph90_270->setName("C90° - C270°");
    }
    rescaleAxes();
    replot();
}


// 生成0-180°剖面数据
std::vector<QPointF> IESCartesianWidget::generateC0C180Profile(double angle) {
    std::vector<QPointF> profile;

    std::vector<double> r1;
    std::vector<double> r2;

    int oneEightyPhiIndex = IESLoader::instance().findPhiIndex(angle);
    if (oneEightyPhiIndex >= 0) {
        r1 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
    }
    std::vector<QPointF> profile1;
    profile1.reserve(IESLoader::instance().newThetas_r1.size());

    for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
        profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r1[i]));
    }
    profile.insert(profile.end(), profile1.begin(), profile1.end());
    if (IESLoader::instance().light.m_IESType <= 4)
    {
        int twoEightyPhiIndex = angle + 180;
        if (twoEightyPhiIndex >= 360)twoEightyPhiIndex = twoEightyPhiIndex - 360;
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
        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
            profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i]-360, r2[i]));
        }
        profile.insert(profile.end(), profile2.begin(), profile2.end());
    }

    return profile;
}
// 生成90-270°剖面数据
std::vector<QPointF> IESCartesianWidget::generateC90C270Profile(double angle) {
    std::vector<QPointF> profile;

    std::vector<double> r3;
    std::vector<double> r4;

    if (IESLoader::instance().light.m_IESType <= 4)
    {
        int oneEightyPhiIndex = 90 + angle;
        if (oneEightyPhiIndex >= 360)oneEightyPhiIndex = oneEightyPhiIndex - 360;
        oneEightyPhiIndex = IESLoader::instance().findPhiIndex(oneEightyPhiIndex);
        if (oneEightyPhiIndex >= 0) {
            r3 = IESLoader::instance().newValues_all[oneEightyPhiIndex];
        }
        std::vector<QPointF> profile1;
        profile1.reserve(IESLoader::instance().newThetas_r1.size());
        for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
            if (std::isnan(r3[i]))
                ;
            else
                profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], r3[i]));
        }

        profile.insert(profile.end(), profile1.begin(), profile1.end());
        // /////////////////////////////////////
        int twoEightyPhiIndex = angle + 270;
        if (twoEightyPhiIndex >= 360)twoEightyPhiIndex = twoEightyPhiIndex - 360;
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
        for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
            if (std::isnan(r4[i]))
                ;
            else
                profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i] - 360, r4[i]));
        }

        profile.insert(profile.end(), profile2.begin(), profile2.end());
    }
    return profile;
}


void IESCartesianWidget::on_chkFillBlue_stateChanged(int value)
{
   
    if (value)
    {
        graph0_180->setBrush(QBrush(QColor(0, 0, 255, 80)));
    }
    else
    {
        graph0_180->setBrush(Qt::NoBrush);
    }   
    graph0_180->setPen(QPen(Qt::blue, 2)); // 线宽和颜色
    replot();
}
void IESCartesianWidget::on_chkFillRed_stateChanged(int value)
{
    
    if (value)
    {
        graph90_270->setBrush(QBrush(QColor(255, 0, 0, 80)));
    }
    else
    {
        graph90_270->setBrush(Qt::NoBrush);
    }
    graph90_270->setPen(QPen(Qt::red, 2)); // 线宽和颜色
    replot();
}
void IESCartesianWidget::on_chkViewRed_stateChanged(int value)
{
    
    if (value)
    {
        graph90_270->setVisible(true);
    }
    else
    {
        graph90_270->setVisible(false);
    }

    replot();
}

void IESCartesianWidget::on_chkFillGreen_stateChanged(int value)
{
   
    if (value)
    {
        graph0_180->setBrush(QBrush(QColor(0, 255, 0, 80)));
    }
    else
    {
        graph0_180->setBrush(Qt::NoBrush);
    }
    graph0_180->setPen(QPen(Qt::green, 2)); // 线宽和颜色
    replot();
}

void IESCartesianWidget::on_chkFillYellow_stateChanged(int value)
{
   
    if (value)
    {
        graph90_270->setBrush(QBrush(QColor(190, 190, 71,80)));
    }
    else
    {
        graph90_270->setBrush(Qt::NoBrush);
    }
    graph90_270->setPen(QPen(QColor(190, 190, 71), 2)); // 线宽和颜色
    replot();
}

void IESCartesianWidget::on_chkViewYellow_stateChanged(int value)
{
    if (value)
    {
        graph90_270->setVisible(true);
    }
    else
    {
        graph90_270->setVisible(false);
    }


    replot();
}

void IESCartesianWidget::on_horizontalSlider_valueChanged(int value)
{
    //if (IESLoader::instance().light.m_IESType <= 4)
    //{
    //    value 
    //}
    graph0_180->data().clear();

    auto profile0_180 = generateC0C180Profile(value);
    // 创建0-180°曲线
    QVector<double> angles0_180, values0_180;
    for (const auto& point : profile0_180) {
        angles0_180.append(point.x());
        values0_180.append(point.y());
    }
    //graph0_180->setBrush(Qt::NoBrush);
    graph0_180->setData(angles0_180, values0_180);

    graph90_270->data()->clear();

    auto profile90_270 = generateC90C270Profile(value);

    //// 创建90-270°曲线
    QVector<double> angles90_270, values90_270;
    for (const auto& point : profile90_270) {
        angles90_270.append(point.x());
        values90_270.append(point.y());
    }
    //graph90_270->setBrush(Qt::NoBrush);
    graph90_270->setData(angles90_270, values90_270);
    //graph90_270->setPen(QPen(Qt::blue, 2));
    //graph90_270->setName("C90° - C270°");

    rescaleAxes();
    replot();
}

void IESCartesianWidget::on_horizontalSlider_2_valueChanged(int value)
{
    //graph90_270->data()->clear();

    //auto profile90_270 = generateC90C270Profile(value);

    ////// 创建90-270°曲线
    //QVector<double> angles90_270, values90_270;
    //for (const auto& point : profile90_270) {
    //    angles90_270.append(point.x());
    //    values90_270.append(point.y());
    //}
    ////graph90_270->setBrush(Qt::NoBrush);
    //graph90_270->setData(angles90_270, values90_270);
    ////graph90_270->setPen(QPen(Qt::blue, 2));
    //graph90_270->setName("C90° - C270°");

    //rescaleAxes();
    //replot();
}