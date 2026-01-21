#include "IESLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QOpenGLTexture>
#include <cmath>
#include "tiny_ies.hpp"
#include <qmath.h>
#include <QMessageBox>
IESLoader::IESLoader(): calculationWidth(20)
, gridSpacing(0.1)
, maxIlluminance(0)
, minIlluminance(1e9)
, fixtureX(0)
, fixtureY(0)
, fixtureZ(0)
, m_levelSize(5)
, m_bUseGrid(false)
, m_numOfPoints(200)
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

    //if (light.m_IESType > 4 && horizontal < 180)
    //{
    //    return 0;
    //}
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
int IESLoader::findClosestIndex(const std::vector<double>& array, double value)
{
    if (array.empty()) return -1;

    int closestIndex = 0;
    double minDiff = fabs(array[0] - value);

    for (int i = 1; i < array.size(); i++) {
        double diff = fabs(array[i] - value);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = i;
        }
    }

    // 如果差异太大，返回-1
    if (minDiff > 10.0) { // 10度容差
        return -1;
    }

    return closestIndex;
}
void IESLoader::fillData_extend()
{

}
void IESLoader::fillData()
{
    switch (light.m_IESType)
    {
    case eC0:
    {
        QVector<double> phis1;
        QVector<double> phis2;
        QVector<double> phis3;
        QVector<double> phis4;
        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]);
        }
        //for (int i = 0; i < light.horizontal_angles.size(); i++)
        //{
            phis2.push_back(light.horizontal_angles[0] + 90);
        //    phis3.push_back(light.horizontal_angles[i] + 180);
        //    phis4.push_back(light.horizontal_angles[i] + 270);
        //}

        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        //newPhis.insert(newPhis.end(), phis3.begin(), phis3.end());
        //newPhis.insert(newPhis.end(), phis4.begin(), phis4.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        for (int i = 0; i < light.candela_hv.size() ; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        vals3.insert(vals3.end(), vals1.begin(), vals1.end());
        vals3.insert(vals3.end(), vals2.begin(), vals2.end());

        //vals4.insert(vals4.end(), vals3.begin(), vals3.end());
        //vals4.pop_back();
        //std::reverse(vals4.begin(), vals4.end()); // 反转整个 vector
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        //newValues.insert(newValues.end(), vals4.begin(), vals4.end());
    }
        break;
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

        std::vector<std::vector<double>> vals1;// = light.candela_hv;
        std::vector<std::vector<double>> vals2;// = light.candela_hv;
        for (int i = 0; i < light.candela_hv.size() - 1; i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        vals2.insert(vals2.end(), light.candela_hv.rbegin(), light.candela_hv.rend());
        newValues.insert(newValues.end(), vals1.begin(), vals1.end());
        newValues.insert(newValues.end(), vals2.begin(), vals2.end());
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
    }
    break;
    case eA090:
    case eB090:
    {
        // 原始数据只有0-90度的水平角度
// 需要扩展到完整的0-360度

// 1. 获取原始数据
        ////std::vector<double> original_h_angles = light.horizontal_angles;
        ////std::vector<std::vector<double>> original_candela = light.candela_hv;
        ////std::vector<double> original_v_angles = light.vertical_angles;

        ////// 2. 创建完整的360度角度网格
        ////// 方法1: 使用原始角度分辨率扩展到360度
        ////std::vector<double> full_h_angles;

        ////// 第一象限: 0-90度 (原始数据)
        ////for (double angle : original_h_angles) {
        ////    if (angle <= 90.0) {
        ////        full_h_angles.push_back(angle);
        ////    }
        ////}

        ////// 第二象限: 90-180度 (对称)
        ////// 从接近90度开始递减到接近0度
        ////for (int i = original_h_angles.size() - 1; i >= 0; i--) {
        ////    double angle = original_h_angles[i];
        ////    if (angle > 0 && angle < 90) {  // 排除0度和90度
        ////        full_h_angles.push_back(180.0 - angle);
        ////    }
        ////}

        ////// 添加180度
        ////full_h_angles.push_back(180.0);

        ////// 第三象限: 180-270度 (与第一象限相同但偏移180度)
        ////for (double angle : original_h_angles) {
        ////    if (angle > 0 && angle <= 90) {  // 排除0度
        ////        full_h_angles.push_back(180.0 + angle);
        ////    }
        ////}

        ////// 第四象限: 270-360度 (对称)
        ////for (int i = original_h_angles.size() - 1; i >= 0; i--) {
        ////    double angle = original_h_angles[i];
        ////    if (angle > 0 && angle < 90) {  // 排除0度和90度
        ////        full_h_angles.push_back(360.0 - angle);
        ////    }
        ////}

        ////// 排序并去重
        ////std::sort(full_h_angles.begin(), full_h_angles.end());
        ////auto last = std::unique(full_h_angles.begin(), full_h_angles.end());
        //////full_h_angles.erase(last, full_h_angles.end());

        ////// 3. 为每个水平角度分配光强值
        ////newPhis = full_h_angles;
        ////newValues.clear();

        ////// 创建查找函数，找到原始数据中最接近的角度索引
        ////auto find_closest_index = [&](double target_angle) -> int {
        ////    int closest_idx = 0;
        ////    double min_diff = fabs(original_h_angles[0] - target_angle);

        ////    for (size_t i = 1; i < original_h_angles.size(); i++) {
        ////        double diff = fabs(original_h_angles[i] - target_angle);
        ////        if (diff < min_diff) {
        ////            min_diff = diff;
        ////            closest_idx = i;
        ////        }
        ////    }
        ////    return closest_idx;
        ////};

        ////for (double phi : newPhis) {
        ////    double mapped_phi = phi;

        ////    // 映射到原始角度范围(0-90度)
        ////    if (phi > 90 && phi <= 180) {
        ////        mapped_phi = 180.0 - phi;
        ////    }
        ////    else if (phi > 180 && phi <= 270) {
        ////        mapped_phi = phi - 180.0;
        ////    }
        ////    else if (phi > 270 && phi < 360) {
        ////        mapped_phi = 360.0 - phi;
        ////    }

        ////    // 确保在0-90范围内
        ////    mapped_phi = std::max(0.0, std::min(90.0, mapped_phi));

        ////    int idx = find_closest_index(mapped_phi);
        ////    newValues.push_back(original_candela[idx]);
        ////}

        ////// 4. 垂直角度保持不变
        ////newThetas = original_v_angles;

        QVector<double> phis1;
        QVector<double> phis2;
        QVector<double> phis3;
        QVector<double> phis4;

        // 原始水平角度 (0-90度)
        for (int i = 0; i < light.horizontal_angles.size() - 1; i++)
        {
            phis1.push_back(light.horizontal_angles[i]);
        }

        // 扩展水平角度到完整360度
        // 第二象限: 90-180度
        for (int i = light.horizontal_angles.size() - 1; i >= 0; i--)
        {
            // 使用对称关系: 第二象限的角度 = 180 - 原始角度
            double sym_angle = 180.0 - light.horizontal_angles[i];
            phis2.push_back(sym_angle);
        }

        // 第三象限: 180-270度 (使用对称)
        for (int i = 1; i < light.horizontal_angles.size(); i++)
        {
            // 第三象限的角度 = 180 + 原始角度
            double third_angle = 180.0 + light.horizontal_angles[i];
            phis3.push_back(third_angle);
        }

        // 第四象限: 270-360度 (使用对称)
        for (int i = light.horizontal_angles.size() - 1; i > 0; i--)
        {
            // 第四象限的角度 = 360 - 原始角度
            double fourth_angle = 360.0 - light.horizontal_angles[i];
            phis4.push_back(fourth_angle);
        }

        // 合并所有水平角度
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        newPhis.insert(newPhis.end(), phis3.begin(), phis3.end());
        newPhis.insert(newPhis.end(), phis4.begin(), phis4.end());

        // 对水平角度排序（确保顺序）
        std::sort(newPhis.begin(), newPhis.end());

        // 处理光强值
        std::vector<std::vector<double>> vals1;  // 第一象限: 0-90度
        std::vector<std::vector<double>> vals2;  // 第二象限: 90-180度
        std::vector<std::vector<double>> vals3;  // 第三象限: 180-270度
        std::vector<std::vector<double>> vals4;  // 第四象限: 270-360度


        int h_size = light.horizontal_angles.size();
        int v_size = light.vertical_angles.size();

        for (int i = 0; i < h_size - 1; i++)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals1.push_back(row);
        }

        for (int i = h_size - 1; i >= 0; i--)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals2.push_back(row);
        }

        for (int i = 1; i < h_size ; i++)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals3.push_back(row);
        }
        for (int i = h_size - 1; i > 0; i--)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals4.push_back(row);
        }

        for (auto& row : vals1) {
            std::fill(row.begin(), row.end(), 0.0);
        }
        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }
        // 合并所有象限的数据
        newValues.insert(newValues.end(), vals1.begin(), vals1.end());
        newValues.insert(newValues.end(), vals2.begin(), vals2.end());
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());
    }
    break;
    case eA_9090:
    case eB_9090:
    {
        // 原始水平角度范围：-90到90度
        // 步骤1：将原始角度转换为0-180度范围
        //std::vector<double> phis1; // 存储转换后的0-180度角度
        //for (int i = 0; i < light.horizontal_angles.size(); i++)
        //{
        //    // 将角度从[-90, 90]映射到[0, 180]
        //    double mapped_angle = light.horizontal_angles[i] + 90.0;
        //    phis1.push_back(mapped_angle);
        //}

        //// 步骤2：生成180-360度的角度，利用对称性（假设关于0-180度轴对称）
        //std::vector<double> phis2;
        //for (int i = light.horizontal_angles.size() - 1; i >= 0; i--)
        //{
        //    // 对称角度：将0-180度映射到180-360度，注意顺序要反过来以保持递增
        //    double sym_angle = 360.0 - phis1[i];
        //    // 避免重复180度和360度（0度）点
        //    if (sym_angle > 180.0 && sym_angle < 360.0)
        //    {
        //        phis2.push_back(sym_angle);
        //    }
        //}

        //// 合并角度
        //newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        //newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());

        //// 对水平角度排序（确保递增）
        //std::sort(newPhis.begin(), newPhis.end());

        //// 处理光强值
        //// 对于每个水平角度，我们有一组垂直角度的光强值
        //// 首先，将原始光强值按照转换后的水平角度顺序存储（即原始顺序，因为角度映射是单调的）
        //std::vector<std::vector<double>> vals1; // 对应phis1
        //for (int i = 0; i < light.candela_hv.size(); i++)
        //{
        //    vals1.push_back(light.candela_hv[i]);
        //}

        //// 然后，生成180-360度的光强值，利用对称性
        //std::vector<std::vector<double>> vals2; // 对应phis2
        //// 注意：光强值在对称方向上应该相同，所以我们将vals1的顺序反转，然后对应到phis2
        //for (int i = vals1.size() - 1; i >= 0; i--)
        //{
        //    vals2.push_back(vals1[i]);
        //}

        //// 合并光强值
        //newValues.insert(newValues.end(), vals1.begin(), vals1.end());
        //newValues.insert(newValues.end(), vals2.begin(), vals2.end());
        std::vector<double> phis1;
        std::vector<double> phis2;

        for (int i = 0; i < light.horizontal_angles.size(); i++)
        {
            phis1.push_back(light.horizontal_angles[i]  + 90);
        }

        for (int i = light.horizontal_angles.size() - 1; i > 0; i--)
        {
            phis2.push_back(270 - light.horizontal_angles[i]);
        }

        //for (auto& row : phis2) {
        //    row = 0;
        //}
        // /////////////////////////////////
        newPhis.insert(newPhis.end(), phis1.begin(), phis1.end());
        newPhis.insert(newPhis.end(), phis2.begin(), phis2.end());
        ////////////////////////////////////////////////////
        //std::sort(newPhis.begin(), newPhis.end());

        std::vector<std::vector<double>> vals1;
        std::vector<std::vector<double>> vals2;
        std::vector<std::vector<double>> vals3;
        std::vector<std::vector<double>> vals4;
        int h_size = light.horizontal_angles.size();
        int v_size = light.vertical_angles.size();

        for (int i = 0; i < light.candela_hv.size(); i++)
        {
            vals1.push_back(light.candela_hv[i]);
        }
        for (int i = vals1.size() - 1; i > 0; i--)
        {
            vals2.push_back(vals1[i]);
        }
        for (int i = 1; i < h_size; i++)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals3.push_back(row);
        }
        for (int i = h_size - 1; i > 0; i--)
        {
            std::vector<double> row;
            for (int j = 0; j < v_size; j++)
            {
                row.push_back(light.candela_hv[i][j]);
            }
            vals4.push_back(row);
        }
        //vals3.insert(vals3.end(), vals1.begin(), vals1.end());
        //vals3.insert(vals3.end(), vals2.begin(), vals2.end());
        //vals4 = vals3;
        //vals4.pop_back();
        //for (auto& row : vals4) {
        //    std::fill(row.begin(), row.end(), 0.0);
        //}

        for (auto& row : vals1) {
            std::fill(row.begin(), row.end(), 0.0);
        }
        for (auto& row : vals4) {
            std::fill(row.begin(), row.end(), 0.0);
        }

 
        newValues.insert(newValues.end(), vals1.begin(), vals1.end());
        newValues.insert(newValues.end(), vals2.begin(), vals2.end());
        newValues.insert(newValues.end(), vals3.begin(), vals3.end());
        newValues.insert(newValues.end(), vals4.begin(), vals4.end());
    }
    break;
    default:
    {
        //QMessageBox::warning(this, "错误", "软件不支持，待完善.");
        return;
    }
        break;
    }
    switch (light.m_vType)
    {
    case eC_V180:
    {
        newThetas = linspace(0, 180, light.candela_hv[0].size());
        //double vertical_size = light.number_vertical_angles;
        //double step = light.vertical_angles[vertical_size - 1] - light.vertical_angles[vertical_size - 2];
        //std::vector<double> vals1;
        //std::vector<double> vals2;
        //vals1.insert(vals1.end(), light.vertical_angles.begin(), light.vertical_angles.end());
        //for (double val = light.vertical_angles[vertical_size - 1] + step; val <= 180; val += step)
        //{
        //    vals2.push_back(val);
        //}
        //newThetas.insert(newThetas.end(), vals1.begin(), vals1.end());
        //newThetas.insert(newThetas.end(), vals2.begin(), vals2.end());
        //std::vector<std::vector<double>> extravals;
        //for (int i = 0; i < newValues.size(); i++)
        //{
        //    std::vector<double> one;
        //    one.resize(vals2.size());
        //    for (auto& row : one) {
        //        row = 0;
        //    }
        //    extravals.push_back(one);

        //}
        //for (int i = 0; i < newValues.size(); i++)
        //{
        //    std::vector<double>& v = newValues[i];
        //    std::vector<double> extra = extravals[i];
        //    v.insert(v.end(), extra.begin(), extra.end());
        //}
    }
    break;
    case eC_V90:
    {
        double vertical_size = light.number_vertical_angles;
        double step = light.vertical_angles[vertical_size - 1] - light.vertical_angles[vertical_size - 2];
        std::vector<double> vals1;
        std::vector<double> vals2;
        vals1.insert(vals1.end(), light.vertical_angles.begin(), light.vertical_angles.end());
        for (double val = light.vertical_angles[vertical_size - 1]+step; val <= 180; val+=step)
        {
            vals2.push_back(val);
        }
        newThetas.insert(newThetas.end(), vals1.begin(), vals1.end());
        newThetas.insert(newThetas.end(), vals2.begin(), vals2.end());
        std::vector<std::vector<double>> extravals;
        for (int i = 0; i < newValues.size(); i++)
        {
            std::vector<double> one; 
            one.resize(vals2.size());
            for (auto& row : one) {
                row = 0;
            }
            extravals.push_back(one);
          
        }
        for (int i = 0; i < newValues.size(); i++)
        {
            std::vector<double>& v = newValues[i];
            std::vector<double> extra = extravals[i];
            v.insert(v.end(), extra.begin(), extra.end());
        }
    }
    break;
    case eC_V90_180:
    {
        newThetas = linspace(90, 180, light.candela_hv[0].size());
    }
    break;
    case eB_V90:
    case eA_V90:
    {
        //newThetas = linspace(0, 90, light.candela_hv[0].size());
        double vertical_size = light.number_vertical_angles;
        //double step = light.vertical_angles[vertical_size - 1] - light.vertical_angles[vertical_size - 2];
        std::vector<double> vals1;
        for (int i = 0; i < light.vertical_angles.size(); i++) 
        {
            vals1.push_back(90 + light.vertical_angles[i]);
        }
        for (int i = light.vertical_angles.size() - 1; i > 0; i--) 
        {
            vals1.push_back(180 - light.vertical_angles[i]);
        }
        std::sort(vals1.begin(), vals1.end());
        newThetas = vals1;



        // 1. 获取原始垂直角度和光强值
        //std::vector<double> original_v_angles = light.vertical_angles;
        //std::vector<std::vector<double>> original_candela = light.candela_hv;

        //// 2. 创建完整的0-180度垂直角度数组
        //// 方法1: 使用原始角度分辨率扩展到180度
        //std::vector<double> full_v_angles;

        //// 第一半: 0-90度 (对称扩展)
        //// 使用原始角度，但顺序可能相反，取决于灯具对称性
        //// 通常假设灯具关于水平面对称
        //for (int i = original_v_angles.size() - 1; i >= 0; i--) {
        //    double angle = original_v_angles[i];
        //    // 将角度映射到0-90度范围
        //    double mapped_angle = 90.0 - angle;
        //    if (mapped_angle >= 0) {
        //        full_v_angles.push_back(mapped_angle);
        //    }
        //}

        //// 添加90度点
        //full_v_angles.push_back(90.0);

        //// 第二半: 90-180度 (原始数据)
        //for (int i = 0; i < original_v_angles.size(); i++) {
        //    double angle = original_v_angles[i];
        //    if (angle > 0) {  // 避免重复90度
        //        double mapped_angle = 90.0 + angle;
        //        if (mapped_angle <= 180.0) {
        //            full_v_angles.push_back(mapped_angle);
        //        }
        //    }
        //}

        //// 排序并去重
        //std::sort(full_v_angles.begin(), full_v_angles.end());
        //auto last = std::unique(full_v_angles.begin(), full_v_angles.end());
        //full_v_angles.erase(last, full_v_angles.end());

        //// 3. 为每个水平角度扩展光强值
        //std::vector<std::vector<double>> extended_values;

        //// 对于每个水平角度
        //for (const auto& h_intensities : original_candela) {
        //    std::vector<double> extended_intensities(full_v_angles.size(), 0.0);

        //    // 映射函数：找到原始角度对应的索引
        //    auto find_original_index = [&](double target_angle) -> int {
        //        int closest_idx = 0;
        //        double min_diff = fabs(original_v_angles[0] - target_angle);

        //        for (size_t i = 1; i < original_v_angles.size(); i++) {
        //            double diff = fabs(original_v_angles[i] - target_angle);
        //            if (diff < min_diff) {
        //                min_diff = diff;
        //                closest_idx = i;
        //            }
        //        }
        //        return closest_idx;
        //    };

        //    // 为每个新垂直角度分配光强值
        //    for (size_t v_idx = 0; v_idx < full_v_angles.size(); v_idx++) {
        //        double target_angle = full_v_angles[v_idx];

        //        if (target_angle <= 90.0) {
        //            // 上半部分 (0-90度)：对称映射
        //            // 映射到原始角度范围
        //            double mapped_angle = 90.0 - target_angle;
        //            int orig_idx = find_original_index(mapped_angle);
        //            if (orig_idx >= 0 && orig_idx < h_intensities.size()) {
        //                extended_intensities[v_idx] = h_intensities[orig_idx];
        //            }
        //        }
        //        else {
        //            // 下半部分 (90-180度)：直接使用
        //            // 映射到原始角度范围
        //            double mapped_angle = target_angle - 90.0;
        //            int orig_idx = find_original_index(mapped_angle);
        //            if (orig_idx >= 0 && orig_idx < h_intensities.size()) {
        //                extended_intensities[v_idx] = h_intensities[orig_idx];
        //            }
        //        }
        //    }

        //    extended_values.push_back(extended_intensities);
        //}

        //// 4. 更新类成员变量
        //newThetas = full_v_angles;

        //// 注意：newValues应该已经包含了水平方向的数据
        //// 现在我们需要用扩展后的垂直数据替换它
        //if (newValues.size() == original_candela.size()) {
        //    newValues = extended_values;
        //}
    }
    break;
    case eB_V_90_90:
    case eA_V_90_90:
    {
        std::vector<double> vertical_angles_mapped;
        std::vector<std::vector<double>> candela_values_mapped;

        // 1. 首先处理原始垂直角度（-90° 到 90°）
        for (int i = 0; i < light.vertical_angles.size(); i++) {
            vertical_angles_mapped.push_back(90 + light.vertical_angles[i]);
        }
        for (int i = light.vertical_angles.size() - 1; i > 0; i--) {
            double mapped_angle = 270 - light.vertical_angles[i];
            vertical_angles_mapped.push_back(mapped_angle);
        }

        // 对映射后的角度排序（确保递增）
        std::sort(vertical_angles_mapped.begin(), vertical_angles_mapped.end());

        // 4. 赋值给类成员变量
        newThetas = vertical_angles_mapped;
            
        //// 原始垂直角度范围：-90到90度
        //// 映射到B型的0到180度：new_theta = original_theta + 90
        //std::vector<double> vertical_angles_mapped;
        //for (int i = 0; i < light.vertical_angles.size(); i++)
        //{
        //    double mapped_angle = light.vertical_angles[i] + 90.0;
        //    vertical_angles_mapped.push_back(mapped_angle);
        //}

        //// 确保角度在0-180度范围内
        //// 由于原始角度是-90到90，映射后就是0到180，所以不需要额外处理

        //// 对映射后的角度排序（确保递增）
        //std::sort(vertical_angles_mapped.begin(), vertical_angles_mapped.end());

        //// 赋值给类成员变量
        //newThetas = vertical_angles_mapped;

        //// 注意：光强值不需要改变，因为每个垂直角度对应的光强值不变，只是角度值重新映射了。
        //// 但是，由于我们改变了垂直角度的顺序（排序后），我们需要相应地调整光强值的顺序。
        //// 由于映射是单调的，我们可以在映射的同时保持光强值的对应关系。

        //// 然而，在之前的处理中，我们已经将光强值按照原始垂直角度顺序存储在了newValues中（每个水平角度对应一个vector，其中是每个垂直角度的光强值）。
        //// 现在，我们需要重新排列每个水平角度下的光强值，使其与排序后的垂直角度对应。

        //// 首先，我们需要建立原始垂直角度到映射后角度的索引映射。
        //// 由于我们映射后进行了排序，所以需要找到排序后的角度对应的原始索引。

        //// 步骤：
        //// 1. 创建一个向量，存储映射后的角度和原始索引
        //std::vector<std::pair<double, int>> mapped_with_index;
        //for (int i = 0; i < light.vertical_angles.size(); i++)
        //{
        //    mapped_with_index.push_back(std::make_pair(light.vertical_angles[i] + 90.0, i));
        //}
        //// 2. 按照角度排序
        //std::sort(mapped_with_index.begin(), mapped_with_index.end(),
        //    [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
        //        return a.first < b.first;
        //    });

        //// 3. 现在，mapped_with_index中存储了排序后的映射角度和对应的原始索引
        //// 我们需要重新排列每个水平角度下的光强值
        //// 注意：newValues已经在之前的部分填充了，每个元素是一个vector<double>，对应一个水平角度，每个vector中的元素对应原始垂直角度的光强值。

        //// 创建一个新的二维向量，用于存储重新排列后的光强值
        //std::vector<std::vector<double>> reordered_values;

        //// 遍历每个水平角度
        //for (int h = 0; h < newValues.size(); h++)
        //{
        //    std::vector<double> original_intensities = newValues[h]; // 注意：这个vector的长度应该等于原始垂直角度数量
        //    std::vector<double> reordered_intensities(original_intensities.size());

        //    // 按照排序后的顺序，将光强值重新排列
        //    for (int i = 0; i < mapped_with_index.size(); i++)
        //    {
        //        int original_index = mapped_with_index[i].second;
        //        reordered_intensities[i] = original_intensities[original_index];
        //    }

        //    reordered_values.push_back(reordered_intensities);
        //}

        //// 用重新排列后的值替换newValues
        //bool isSame = (newValues == reordered_values);
        //newValues = reordered_values;
    }
    break;
    default:break;
    }    

    getCIntensityVectorized();
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



// 修正数据存储顺序，以匹配Qwt的坐标系
void IESLoader::calculateXYPlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType <= 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    // 确保最小值合理
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESLoader::calculateYZPlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType <= 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESLoader::calculateXZPlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType <= 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESLoader::calculateXY_PlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType > 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESLoader::calculateYZ_PlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType > 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

void IESLoader::calculateXZ_PlaneIlluminance()
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
    if (IESLoader::instance().light.m_IESType > 4)
        std::reverse(illuminanceGriddata.begin(), illuminanceGriddata.end());
    if (minIlluminance >= maxIlluminance) {
        minIlluminance = maxIlluminance * 0.1;
    }
}

// 修正数据索引访问，确保正确的坐标系
// 同时需要修正 IlluminanceRasterData 的 value 函数
// 在 IESLoader.h 中修改 IlluminanceRasterData 的 value 函数：

double IESLoader::calculateIlluminanceAtPoint(double x, double y, double z)
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

double IESLoader::calculateIlluminanceAtPoint_(double x, double y, double z)
{
    double dx = -x + fixtureX;
    double dy = -y + fixtureY;
    double dz = -z + fixtureZ;
    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);

    if (totalDistance == 0) return 0;

    double verticalAngle = acos(dz / totalDistance) * 180.0 / M_PI;
    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;

    //20260121
    //if (IESLoader::instance().light.m_IESType > 4 && x == 0)
    //    return 0.0;
    //if (horizontalAngle < 0) horizontalAngle += 360;

    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);
    double cosIncidence = 0;
    if (z == 0)
        cosIncidence = dz / totalDistance;
    else if (x == 0)
        cosIncidence = dx / totalDistance;
    else if (y == 0)
        cosIncidence = dy / totalDistance;
    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;

    return std::max(0.0, illuminance);
}
