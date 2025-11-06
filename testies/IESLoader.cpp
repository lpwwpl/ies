#include "IESLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QOpenGLTexture>
#include <cmath>
#include "tiny_ies.hpp"
#include <qmath.h>
IESLoader::IESLoader()
{
    m_thetas_size = 180;
    m_phis_size = 360;
}

IESLoader::~IESLoader()
{
}

IESLoader& IESLoader::instance()
{
    static IESLoader signleton;
    return signleton;
}

bool IESLoader::loadFromFile(const QString& filename)
{
    return parseIESFile(filename);
}


// 生成线性空间
std::vector<double> IESLoader::linspace(double start, double end, int num) {
    if (num <= 1) {
        return std::vector<double>{start};
    }

    std::vector<double> result(num);
    double step = (end - start) / (num - 1);

    for (int i = 0; i < num; ++i) {
        result[i] = start + i * step;
    }

    return result;
}
bool IESLoader::parseIESFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open IES file:" << filename;
        return false;
    }   


    tiny_ies<float>::light light;
    std::string err_out;
    std::string warn_out;
    tiny_ies<float>::load_ies(filename.toStdString(), err_out, warn_out, light);// ies;

    int h_size = horizontalAngles.size();

    for (int i = 0; i < light.horizontal_angles.size(); i++)
    {
        horizontalAngles.push_back(light.horizontal_angles[i]);
    }
    for (int i = 0; i < light.vertical_angles.size(); i++)
    {
        verticalAngles.push_back(light.vertical_angles[i]);
    }
    for (int v = 0; v < verticalAngles.size(); v++)
    {
        for (int h = 0; h < horizontalAngles.size(); h++)
        {
            float intensity = light.candela[v * h_size + h];
            iesData.push_back(intensity/light.max_candela);
        }
    }   


}

void IESLoader::getCIntensityVectorized(int numThetas, int numPhis) {

    newThetas_all = linspace(0, m_thetas_size, numThetas);
    newPhis_all = linspace(0, m_phis_size, numPhis);
    newValues_all.resize(newPhis_all.size());


    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> p_t = meshgrid(newThetas_all, newPhis_all);
    auto tgrid = p_t.first;
    auto pgrid = p_t.second;

    tflat = flatten(tgrid);
    pflat = flatten(pgrid);

    auto phis_indices = searchsorted(newPhis, pflat, "left");
    auto theta_indices = searchsorted(newThetas, tflat, "left");

    phis_indices = clip_indices(phis_indices, 1, newPhis.size() - 1);
    theta_indices = clip_indices(theta_indices, 1, newThetas.size() - 1);

    std::vector<double> phi_weights;
    std::vector<double> theta_weights;
    for (long i = 0; i < phis_indices.size(); i++)
    {
        double val = (pflat[i] - newPhis[phis_indices[i] - 1]) / (newPhis[phis_indices[i]] - newPhis[phis_indices[i] - 1]);
        phi_weights.push_back(val);
    }
    for (long i = 0; i < theta_indices.size(); i++)
    {
        double val = (tflat[i] - newThetas[theta_indices[i] - 1]) / (newThetas[theta_indices[i]] - newThetas[theta_indices[i] - 1]);
        theta_weights.push_back(val);
    }
    std::vector<double> val1;
    std::vector<double> val2;
    std::vector<double> finalVal;
    for (long i = 0; i < phis_indices.size(); i++)
    {
        double v1 = newValues[phis_indices[i] - 1][theta_indices[i] - 1] * (1 - phi_weights[i]) + newValues[phis_indices[i]][theta_indices[i] - 1] * phi_weights[i];
        val1.push_back(v1);

        double v2 = newValues[phis_indices[i] - 1][theta_indices[i]] * (1 - phi_weights[i]) + newValues[phis_indices[i]][theta_indices[i]] * phi_weights[i];
        val2.push_back(v2);
    }

    for (long i = 0; i < phis_indices.size(); i++)
    {
        double v = val1[i] * (1 - theta_weights[i]) + val2[i] * theta_weights[i];
        finalVal.push_back(v);
    }
    newValues_all = reshape_2d(finalVal, newPhis_all.size(), newThetas_all.size());

    intensity = flatten(newValues_all);
    get_coords();




    // c0 - c180
    for (double theta : newThetas_all) {
        newThetas_r1.push_back(/*qDegreesToRadians*/(theta));
    }
    // 计算theta2: thetas + 180度，转换为弧度，并去掉第一个点
    if (newThetas_all.size() > 1) {
        newThetas_r2.reserve(newThetas_all.size() - 1);
        for (int i = 1; i < newThetas_all.size(); ++i) {
            newThetas_r2.push_back(/*qDegreesToRadians*/(newThetas_all[i] + 180.0));
        }
    }

    // 提取0度方向的光强值
    int zeroPhiIndex = findPhiIndex(0.0);
    if (zeroPhiIndex >= 0) {
        r1 = newValues_all[zeroPhiIndex];
    }
    else {
        qWarning() << "Phi values for 0 degrees not found";
    }

    // 提取180度方向的光强值，翻转并去掉第一个点
    int oneEightyPhiIndex = findPhiIndex(180.0);
    if (oneEightyPhiIndex >= 0) {
        r2 = newValues_all[oneEightyPhiIndex];
        // 翻转数组
        std::reverse(r2.begin(), r2.end());
        // 去掉第一个点
        if (r2.size() > 1) {
            r2.erase(r2.begin());
        }
    }
    else {
        qWarning() << "Phi values for 180 degrees not found";
    }


    // c90 - c270
    // 提取0度方向的光强值
    zeroPhiIndex = findPhiIndex(90);
    if (zeroPhiIndex >= 0) {
        r3 = newValues_all[zeroPhiIndex];
    }
    else {
        qWarning() << "Phi values for 90 degrees not found";
    }
    oneEightyPhiIndex = findPhiIndex(270);
    if (oneEightyPhiIndex >= 0) {
        r4 = newValues_all[oneEightyPhiIndex];
        // 翻转数组
        std::reverse(r4.begin(), r4.end());
        // 去掉第一个点
        if (r4.size() > 1) {
            r4.erase(r4.begin());
        }
    }
    else {
        qWarning() << "Phi values for 270 degrees not found";
    }
}

int IESLoader::findThetaIndex(double targetTheta)
{
    // 规范化phi值到0-360范围
    //targetTheta = std::fmod(targetTheta, 180);
    //if (targetTheta < 0) targetTheta += 180.0;

    // 查找最接近的索引
    int closestIndex = -1;
    double minDiff = std::numeric_limits<double>::max();

    for (int i = 0; i < newThetas_all.size(); ++i) {
        double diff = std::abs(newThetas_all[i] - targetTheta);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = i;
        }
    }

    // 如果找到的索引对应的角度与目标角度相差较大，则认为没找到
    //if (closestIndex >= 0 /*&& minDiff > 1.0*/) { // 1度容差
    //    return -1;
    //}

    return closestIndex;
}
int IESLoader::findPhiIndex(double targetPhi) {
    // 规范化phi值到0-360范围
    //targetPhi = std::fmod(targetPhi, 360.0);
    //if (targetPhi < 0) targetPhi += 360.0;

    // 查找最接近的索引
    int closestIndex = -1;
    double minDiff = std::numeric_limits<double>::max();

    for (int i = 0; i < newPhis_all.size(); ++i) {
        double diff = std::abs(newPhis_all[i] - targetPhi);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = i;
        }
    }

    // 如果找到的索引对应的角度与目标角度相差较大，则认为没找到
    //if (closestIndex >= 0 ) { // 1度容差
    //    return -1;
    //}

    return closestIndex;
}

void IESLoader::get_coords()
{
    coords.clear();
    for (long i = 0; i < tflat.size(); i++)
    {
        double t = tflat[i];
        double p = pflat[i];
        QVector3D v = polar_to_cartesian(t, p, 1);
        coords.push_back(v);
    }
    //intensity

}

//double IESLoader::getCandelaValue(double vertical, double horizontal) const {
//    int vIdx = 0, hIdx = 0;
//    double minVDiff = m_phis_size, minHDiff = m_phis_size;
//
//    for (int i = 0; i < verticalAngles.size(); ++i) {
//        double diff = fabs(verticalAngles[i] - vertical);
//        if (diff < minVDiff) {
//            minVDiff = diff;
//            vIdx = i;
//        }
//    }
//
//    for (int j = 0; j < horizontalAngles.size(); ++j) {
//        double diff = fabs(horizontalAngles[j] - horizontal);
//        if (diff < minHDiff) {
//            minHDiff = diff;
//            hIdx = j;
//        }
//    }
//
//    return light.candela_hv[vIdx][hIdx];
//}
//
//double IESLoader::interpolateCandela(double vertical, double horizontal) 
//{
//    // 角度归一化
//    while (horizontal < 0) horizontal += 360;
//    while (horizontal >= 360) horizontal -= 360;
//
//    vertical = std::max(0.0, std::min(180.0, vertical));
//
//    if (0/*interpolationMethod*/ == 0) {
//        // 双线性插值
//        int vIdx1 = -1, vIdx2 = -1;
//        int hIdx1 = -1, hIdx2 = -1;
//
//        // 找到垂直方向的插值区间
//        for (int i = 0; i < light.vertical_angles.size() - 1; ++i) {
//            if (vertical >= light.vertical_angles[i] && vertical <= light.vertical_angles[i + 1]) {
//                vIdx1 = i;
//                vIdx2 = i + 1;
//                break;
//            }
//        }
//
//        // 找到水平方向的插值区间
//        for (int j = 0; j < light.horizontal_angles.size() - 1; ++j) {
//            if (horizontal >= light.horizontal_angles[j] && horizontal <= light.horizontal_angles[j + 1]) {
//                hIdx1 = j;
//                hIdx2 = j + 1;
//                break;
//            }
//        }
//
//        if (vIdx1 == -1 || hIdx1 == -1) {
//            // 使用最近邻
//            return getCandelaValue(vertical, horizontal);
//        }
//
//        // 双线性插值
//        double vAlpha = (vertical - light.vertical_angles[vIdx1]) /
//            (light.vertical_angles[vIdx2] - light.vertical_angles[vIdx1]);
//        double hAlpha = (horizontal - light.horizontal_angles[hIdx1]) /
//            (light.horizontal_angles[hIdx2] - light.horizontal_angles[hIdx1]);
//
//        double val1 = light.candela_hv[hIdx1][vIdx1] * (1 - hAlpha) +
//            light.candela_hv[hIdx2][vIdx1] * hAlpha;
//        double val2 = light.candela_hv[hIdx1][vIdx2] * (1 - hAlpha) +
//            light.candela_hv[hIdx2][vIdx2] * hAlpha;
//
//        return val1 * (1 - vAlpha) + val2 * vAlpha;
//    }
//    else {
//        // 最近邻插值
//        return getCandelaValue(vertical, horizontal);
//    }
//}


double IESLoader::getCandelaValue(double vertical, double horizontal) 
{
    // 角度归一化
    while (horizontal < 0) horizontal += 360;
    while (horizontal >= 360) horizontal -= 360;

    if (light.m_IESType > 4)
    {
        while (horizontal >180) horizontal -= 180;
    }
    // 简单的最近邻插值 - 实际应用中应使用双线性插值
    int vIdx = 0, hIdx = 0;
    double minVDiff = 360, minHDiff = 360;

    //360 h
    hIdx = findPhiIndex(horizontal);
    vIdx = findThetaIndex(vertical);
    //for (int i = 0; i < verticalAngles.size(); ++i) {
    //    double diff = fabs(verticalAngles[i] - vertical);
    //    if (diff < minVDiff) {
    //        minVDiff = diff;
    //        vIdx = i;
    //    }
    //}

    //for (int j = 0; j < horizontalAngles.size(); ++j) {
    //    double diff = fabs(horizontalAngles[j] - horizontal);
    //    if (diff < minHDiff) {
    //        minHDiff = diff;
    //        hIdx = j;
    //    }
    //}
    if (hIdx == -1 || vIdx == -1)
        return 0;
    return newValues_all[hIdx][vIdx];
}
QVector3D IESLoader::polar_to_cartesian(double theta, double phi,double distance)
{
    double theta_rad = qDegreesToRadians(m_thetas_size - theta);
    double phi_rad = qDegreesToRadians(phi);


    double x = distance * sin(theta_rad) * sin(phi_rad);
    double y = distance * sin(theta_rad) * cos(phi_rad);
    double z = distance * cos(theta_rad);

    return QVector3D(x, y, z);
}
void IESLoader::clear()
{
    newPhis.clear();
    newThetas.clear();
    newValues_all.clear();
    newThetas_all.clear();
    newPhis_all.clear();

    //37*17
    newValues.clear();
    newThetas.clear();
    newPhis.clear();

    newThetas_r1.clear();
    newPhis_r1.clear();
    r1.clear();

    newThetas_r2.clear();
    newPhis_r2.clear();
    r2.clear();

    r3.clear();
    r4.clear();

    tflat.clear();
    pflat.clear();
    coords.clear();
    intensity.clear();
}
void IESLoader::loadIES(QString filename)
{

    clear();

    std::string err_out;
    std::string warn_out;
    tiny_ies<double>::load_ies(filename.toStdString(), err_out, warn_out, light);// ies;


    fillData();
}
void IESLoader::fillData_extend()
{

}
void IESLoader::fillData()
{
    switch (light.m_IESType)
    {
    case eC90:
    {
        QVector<double> phis1;
        QVector<double> phis2;
        QVector<double> phis3;
        QVector<double> phis4;
        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]);
        }
        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 90);
            phis3.push_back(light.horizontal_angles[i] + 180);
            phis4.push_back(light.horizontal_angles[i] + 270);
        }

        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        newPhis.insert(newPhis.end(), phis3.begin(), phis3.end());
        newPhis.insert(newPhis.end(), phis4.begin(), phis4.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());

        vals4.insert(vals4.end(), vals3.begin(), vals3.end());
        vals4.pop_back();
        std::reverse(vals4.begin(), vals4.end()); // 反转整个 vector
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());

        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    case eC180:
    {
        QVector<double> phis1;
        QVector<double> phis2;
        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]);
        }
        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 180);
        }
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());

        std::vector<std::vector<double>> vals1 = light.candela_hv;
        std::vector<std::vector<double>> vals2 = light.candela_hv;
        vals2.pop_back();
        std::reverse(vals2.begin(), vals2.end()); // 反转整个 vector
        newValues.insert(newValues.end(), vals1.begin(), vals1.end());
        newValues.insert(newValues.end(), vals2.begin(), vals2.end());

        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    case eC270:
    {

    }
    break;
    case eC360:
    {
        newPhis = light.horizontal_angles;
        newThetas = light.vertical_angles;
        newValues = light.candela_hv;

        getCIntensityVectorized();
    }
    break;
    case eB090:
    {
        QVector<double> phis1;
        QVector<double> phis2;
        QVector<double> phis3;
        QVector<double> phis4;
        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i] );
        }
        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 90);
            phis3.push_back(0);
            phis4.push_back(0);
        }
       
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        newPhis.insert(newPhis.end(), phis3.begin(), phis3.end());
        newPhis.insert(newPhis.end(), phis4.begin(), phis4.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());
     

        vals4.insert(vals4.end(), vals3.begin(), vals3.end());
        vals4.pop_back();
        //std::reverse(vals4.begin(), vals4.end()); // 反转整个 vector

        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }

        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());


        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    case eB_9090:
    {
        std::vector<double> phis1;
        std::vector<double> phis2;

        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]  +90);
        }

        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 270);
        }
        for (auto& row : phis2) {
            row = 0;
        }
        // /////////////////////////////////
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());

        vals4 = vals3;
        vals4.pop_back();
        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }     
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());

        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    case eA090:
    {
        QVector<double> phis1;
        QVector<double> phis2;
        QVector<double> phis3;
        QVector<double> phis4;
        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]);
        }
        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 90);
            phis3.push_back(0);
            phis4.push_back(0);
        }

        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        newPhis.insert(newPhis.end(), phis3.begin(), phis3.end());
        newPhis.insert(newPhis.end(), phis4.begin(), phis4.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());


        vals4.insert(vals4.end(), vals3.begin(), vals3.end());
        vals4.pop_back();
        //std::reverse(vals4.begin(), vals4.end()); // 反转整个 vector

        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }

        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());


        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    case eA_9090:
    {
        std::vector<double> phis1;
        std::vector<double> phis2;

        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i] + 90);
        }

        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            phis2.push_back(light.horizontal_angles[i] + 270);
        }
        for (auto& row : phis2) {
            row = 0;
        }
        // /////////////////////////////////
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());

        vals4 = vals3;
        vals4.pop_back();
        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());

        newThetas = linspace(0, m_thetas_size, light.candela_hv[0].size());

        getCIntensityVectorized();
    }
    break;
    default:
        break;
    }
}
QOpenGLTexture* IESLoader::createTexture()
{
    if (iesData.isEmpty() || horizontalAngles.isEmpty() || verticalAngles.isEmpty()) {
        return nullptr;
    }

    int width = horizontalAngles.size();
    int height = verticalAngles.size();

    // 创建OpenGL纹理
    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setSize(width, height);
    texture->setFormat(QOpenGLTexture::R32F);
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);

    // 分配存储空间
    texture->allocateStorage();

    // 上传纹理数据
    texture->setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, iesData.constData());

    return texture;
}