// IESSpotLightWidget.h
#pragma once
#include <QWidget>
#include "QCustomPlot.h"
#include "tiny_ies.hpp"



class IESPolarWidget:public QCustomPlot
{
public:
	IESPolarWidget();
	~IESPolarWidget();
	void updateIES();
	
	
	void Init();

	std::vector<QPointF> generateC0C180Profile();  //c0-c180
	std::vector<QPointF> generateC90C270Profile(); //c90 - c270
	//void addContourLines(QCPColorMap* colorMap, const QVector<double>& isoLevels);

public:
	QCPPolarAxisRadial* radialAxis = nullptr;
	QCPPolarAxisAngular* angularAxis = nullptr;
	QCPPolarGraph* graph0_180 = nullptr;
	QCPPolarGraph* graph90_270 = nullptr;
	//tiny_ies<double>::light light;

};