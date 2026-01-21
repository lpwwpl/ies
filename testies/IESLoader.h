#ifndef IESLOADER_H
#define IESLOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QOpenGLTexture>
#include "tiny_ies.hpp"

// 照度计算点
struct IlluminancePoint {
    double x, y, z;
    double illuminance; // 照度(lux)
    IlluminancePoint() {}
    IlluminancePoint(double x_,double y_,double z_,double ill_):x(x_),y(y_),z(z_),illuminance(ill_) {}
};

class IESLoader
{
public:
    IESLoader();
    ~IESLoader();

    static IESLoader& instance();

    // 加载IES文件
    bool loadFromFile(const QString& filename);

    // 获取光强分布数据
    const QVector<float>& getIESData() const { return iesData; }
    const QVector<float>& getVerticalAngles() const { return verticalAngles; }
    const QVector<float>& getHorizontalAngles() const { return horizontalAngles; }

    // 创建OpenGL纹理
    QOpenGLTexture* createTexture();
    void clear();
    void fillData();
    void fillData_extend();
    void loadIES(QString filename);
    void getCIntensityVectorized(int numThetas = 181, int numPhis = 361);
    int findPhiIndex(double targetPhi);
    int findThetaIndex(double targetTheta);
    std::vector<double> linspace(double start, double end, int num);
    void get_coords();
    QVector3D polar_to_cartesian(double theta,double phi,double distance =1);
    double getCandelaValue(double vertical, double horizontal) ;
    int findClosestIndex(const std::vector<double>& array, double value);


    // 计算函数
    void calculateXZPlaneIlluminance();
    void calculateYZPlaneIlluminance();
    void calculateXYPlaneIlluminance();
    void calculateXZ_PlaneIlluminance();
    void calculateYZ_PlaneIlluminance();
    void calculateXY_PlaneIlluminance();
    double calculateIlluminanceAtPoint(double x, double y, double z);
    double calculateIlluminanceAtPoint_(double x, double y, double z);
    //double interpolateCandela(double vertical, double horizontal);
    //double getCandelaValue(double vertical, double horizontal) const;
private:
    // 解析IES文件
    bool parseIESFile(const QString& filename);

public:
    // IES数据
    QVector<float> iesData;
    QVector<float> verticalAngles;
    QVector<float> horizontalAngles;

    tiny_ies<double>::light light;

    double m_thetas_size;
    double m_phis_size ;

    //181*361
    std::vector<std::vector<double>> newValues_all;
    std::vector<double> newThetas_all;
    std::vector<double> newPhis_all;

    //37*17
    std::vector<std::vector<double>> newValues;
    std::vector<double> newThetas;
    std::vector<double> newPhis;

    std::vector<double> newThetas_r1;
    std::vector<double> newPhis_r1;
    std::vector<double> r1;

    std::vector<double> newThetas_r2;
    std::vector<double> newPhis_r2;
    std::vector<double> r2;

    //std::vector<double> newThetas_r3;
    //std::vector<double> newPhis_r3;
    std::vector<double> r3;

    //std::vector<double> newThetas_r4;
    //std::vector<double> newPhis_r4;
    std::vector<double> r4;
    std::vector<double> tflat;
    std::vector<double> pflat;
    std::vector< QVector3D> coords;
    std::vector<double> intensity;



    double calculationWidth;
    double gridSpacing;
    double maxIlluminance;
    double minIlluminance;

    std::vector<double> illuminanceLevels;
    //std::vector<IsoluxCurve> isoluxCurves;
    std::vector<IlluminancePoint> illuminanceGrid;
    std::vector<std::vector<double>> illuminanceGriddata;

    double fixtureX;
    double fixtureY;
    double fixtureZ;
    int m_levelSize;
    bool m_bUseGrid;
    int m_numOfPoints;
};

#endif // IESLOADER_H