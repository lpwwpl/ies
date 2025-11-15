// IESSpotLightWidget.h
#pragma once
#include <QWidget>
#include "QCustomPlot.h"
#include "tiny_ies.hpp"


class IESCartesianWidget:public QCustomPlot
{
	Q_OBJECT
public:
	IESCartesianWidget(QWidget* parent=0);
	~IESCartesianWidget();
	void updateIES(double angle=0);
	
	
	void Init();


	std::vector<QPointF> generateC0C180Profile(double angle=0);  //c0-c180
	std::vector<QPointF> generateC90C270Profile(double angle = 0); //c90 - c270
	//void addContourLines(QCPColorMap* colorMap, const QVector<double>& isoLevels);

public Q_SLOTS:
	void on_chkFillBlue_stateChanged(int value);
	void on_chkFillRed_stateChanged(int);
	void on_chkViewRed_stateChanged(int);

	void on_chkFillGreen_stateChanged(int);
	void on_chkFillYellow_stateChanged(int);
	void on_chkViewYellow_stateChanged(int);

	void on_horizontalSlider_valueChanged(int);
	void on_horizontalSlider_2_valueChanged(int);
public:

	QCPGraph* graph0_180 = nullptr;
	QCPGraph* graph90_270 = nullptr;


	//tiny_ies<double>::light light;
	//QCPLayer* backgroundLayer;
	//QCPLayer* foregroundLayer;
};