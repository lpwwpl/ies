#ifndef IESLOADER_H
#define IESLOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QOpenGLTexture>
#include "tiny_ies.hpp"


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
    void loadIES(QString filename);
    void getCIntensityVectorized(int numThetas = 181, int numPhis = 361);
    void getBIntensityVectorized(int numThetas = 181, int numPhis = 180);
    int findPhiIndex(double targetPhi);
    int findThetaIndex(double targetTheta);
    std::vector<double> linspace(double start, double end, int num);
    void get_coords();
    QVector3D polar_to_cartesian(double theta,double phi,double distance =1);
    double getCandelaValue(double vertical, double horizontal) ;
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
};

#endif // IESLOADER_H