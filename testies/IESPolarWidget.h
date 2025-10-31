// IESSpotLightWidget.h
#pragma once
#include <QWidget>
#include "QCustomPlot.h"
#include "tiny_ies.hpp"


class IESPolarWidget:public QCustomPlot
{
	Q_OBJECT
public:
	IESPolarWidget();
	~IESPolarWidget();
	void updateIES();
	
	
	void Init();

	std::vector<QPointF> generateC0C180Profile();  //c0-c180
	std::vector<QPointF> generateC90C270Profile(); //c90 - c270
	//void addContourLines(QCPColorMap* colorMap, const QVector<double>& isoLevels);

public Q_SLOTS:
	void on_chkFillBlue_stateChanged(int value);
	void on_chkFillRed_stateChanged(int);
	void on_chkViewRed_stateChanged(int);

	void on_chkFillGreen_stateChanged(int);
	void on_chkFillYellow_stateChanged(int);
	void on_chkViewYellow_stateChanged(int);

public:
	QCPPolarAxisRadial* radialAxis = nullptr;
	QCPPolarAxisAngular* angularAxis = nullptr;
	QCPPolarGraph* graph0_180 = nullptr;
	QCPPolarGraph* graph90_270 = nullptr;


	//tiny_ies<double>::light light;
	//QCPLayer* backgroundLayer;
	//QCPLayer* foregroundLayer;
};