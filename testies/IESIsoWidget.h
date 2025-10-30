// IESSpotLightWidget.h
#pragma once
#include <QWidget>
#include "QCustomPlot.h"
#include "tiny_ies.hpp"
#include "contours.h"

// �նȼ����
struct IlluminancePoint {
	double x, y,z;
	double illuminance; // �ն�(lux)
};
// ���ն���
struct IsoluxCurve {
	double illuminanceLevel;
	std::vector<QPointF> points;
	QColor color;
};
// ��ɫ����
struct ColorScheme {
	QString name;
	std::vector<QColor> colors;
};

class IESIsoWidget:public QCustomPlot
{
	using ColorMapDataPtr = std::unique_ptr<QCPColorMapData>;
public:
	IESIsoWidget();
	~IESIsoWidget();

	void Init();

	void updateIESXY(double distance, double halfmap);
	void updateIESYZ(double distance,double halfmap);
	void updateIESXZ(double distance, double halfmap);
	// ///-x,-y,-z
	void updateIESXY_(double distance, double halfmap);
	void updateIESYZ_(double distance, double halfmap);
	void updateIESXZ_(double distance, double halfmap);
	void updateColorMapData();
	void calculateXZPlaneIlluminance();
	void calculateYZPlaneIlluminance();
	void calculateXYPlaneIlluminance();

	void calculateXZ_PlaneIlluminance();
	void calculateYZ_PlaneIlluminance();
	void calculateXY_PlaneIlluminance();

	//xyz,-x,-y
	double calculateIlluminanceAtPoint(double x, double y, double z);
	// ////////////////////////-z
	double calculateIlluminanceAtPoint_(double x, double y, double z);

	void updatePlot();

	//void setupColorSchemes();
public:
	QCPPolarAxisRadial* radialAxis = nullptr;
	QCPPolarAxisAngular* angularAxis = nullptr;
	QCPColorScale* m_colorScale;
	QVector<double> m_contourLevels;
	//ColorMapDataPtr m_colorMapData;
	QCPMarginGroup* m_margin;
	QCPColorMap* m_colorMap;
	double calculationWidth;
	double workplaneHeight;
	double gridSpacing;
	double maxIlluminance;
	double minIlluminance;

	std::vector<double> illuminanceLevels;
	std::vector<IsoluxCurve> isoluxCurves;
	std::vector<IlluminancePoint> illuminanceGrid;
	std::vector<vector<double>> illuminanceGriddata;
	double fixtureX;
	double fixtureY;
	double fixtureZ;
	int m_levelSize;
	bool m_bUseGrid;
	int m_numOfPoints;
	// ��ɫ����
	std::vector<ColorScheme> colorSchemes;
};

