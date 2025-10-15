#include "IESPolarWidget.h"
#include "IESLoader.h"
#include <QDebug>

IESPolarWidget::IESPolarWidget()
{
    Init();

}

IESPolarWidget::~IESPolarWidget()
{

}


void IESPolarWidget::Init()
{

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(10);

    //������
    this->plotLayout()->clear(); // ���Ĭ�ϲ���
    angularAxis = new QCPPolarAxisAngular(this);
    this->plotLayout()->addElement(0, 0, angularAxis); // ���Ƕ�����ӵ���������

    // 3. ���������� (����������)
    radialAxis = angularAxis->radialAxis();
    angularAxis->setTickLabelFont(font);
    angularAxis->setLabel(tr("�Ƕ��� (��)"));
    angularAxis->setRange(0, 360); // �Ƕȷ�Χ (0-360��)
    angularAxis->grid()->setVisible(true);
    angularAxis->setTicks(true);
   
   
    radialAxis->setLabel(tr("������ (r)"));
    radialAxis->setAngle(0);

    radialAxis->setSubTicks(false);
    radialAxis->setTickLabelFont(font);
    graph0_180 = new QCPPolarGraph(angularAxis, radialAxis);

    // 8. ����ͼ����ʽ (��ѡ)
    graph0_180->setLineStyle(QCPPolarGraph::lsLine); // ����
    graph0_180->setScatterStyle(QCPScatterStyle::ssDot); // ɢ����ʽ
    graph0_180->setPen(QPen(Qt::blue, 1)); // �߿����ɫ
    graph0_180->setBrush(QBrush(QColor(0, 0, 250, 150)));

    graph90_270 = new QCPPolarGraph(angularAxis, radialAxis);
    // 8. ����ͼ����ʽ (��ѡ)
    graph90_270->setLineStyle(QCPPolarGraph::lsLine); // ����
    graph90_270->setScatterStyle(QCPScatterStyle::ssDot); // ɢ����ʽ
    graph90_270->setPen(QPen(Qt::blue, 1)); // �߿����ɫ
    graph90_270->setBrush(QBrush(QColor(0, 0, 250, 150)));
}


void IESPolarWidget::updateIES()
{
    radialAxis->setRange(0, IESLoader::instance().light.max_candela * IESLoader::instance().light.multiplier);
    clearPlottables();
    //clearGraphs();

    graph0_180->data().clear();
    graph90_270->data().clear();

    auto profile0_180 = generateC0C180Profile();
    // ����90-270���������� (C90�� - C270��)
    auto profile90_270 = generateC90C270Profile();


    // ����0-180������
    QVector<double> angles0_180, values0_180;
    for (const auto& point : profile0_180) {
        angles0_180.append(point.x());
        values0_180.append(point.y());
    }
    graph0_180->setBrush(Qt::NoBrush);
    graph0_180->setData(angles0_180, values0_180);
    graph0_180->setPen(QPen(Qt::red, 2));
    graph0_180->setName("C0�� - C180��");

    //// ����90-270������
    QVector<double> angles90_270, values90_270;
    for (const auto& point : profile90_270) {
        angles90_270.append(point.x());
        values90_270.append(point.y());
    }
    graph90_270->setBrush(Qt::NoBrush);
    graph90_270->setData(angles90_270, values90_270);
    graph90_270->setPen(QPen(Qt::blue, 2));
    graph90_270->setName("C90�� - C270��");



    rescaleAxes();
    replot();
}


// ����0-180����������
std::vector<QPointF> IESPolarWidget::generateC0C180Profile() {
    std::vector<QPointF> profile;

    std::vector<QPointF> profile1;
    profile1.reserve(IESLoader::instance().newThetas_r1.size());

    for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
        profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], IESLoader::instance().r1[i]));
    }

    std::vector<QPointF> profile2;
    profile2.reserve(IESLoader::instance().newThetas_r2.size());
    for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
        profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], IESLoader::instance().r2[i]));
    }
    profile.insert(profile.end(), profile1.begin(), profile1.end());
    profile.insert(profile.end(), profile2.begin(), profile2.end());
    return profile;
}
// ����90-270����������
std::vector<QPointF> IESPolarWidget::generateC90C270Profile() {
    std::vector<QPointF> profile;

    std::vector<QPointF> profile1;
    profile1.reserve(IESLoader::instance().newThetas_r1.size());

    for (int i = 0; i < IESLoader::instance().newThetas_r1.size(); ++i) {
        profile1.push_back(QPointF(IESLoader::instance().newThetas_r1[i], IESLoader::instance().r3[i]));
    }

    std::vector<QPointF> profile2;
    profile2.reserve(IESLoader::instance().newThetas_r2.size());

    for (int i = 0; i < IESLoader::instance().newThetas_r2.size(); ++i) {
        profile2.push_back(QPointF(IESLoader::instance().newThetas_r2[i], IESLoader::instance().r4[i]));
    }
    profile.insert(profile.end(), profile1.begin(), profile1.end());
    profile.insert(profile.end(), profile2.begin(), profile2.end());
    return profile;
}


