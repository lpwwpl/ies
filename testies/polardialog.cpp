#include "polardialog.h"
#include "ui_polardialog.h"
#include "IESPolarWidget.h"
#include "IESCartesianWidget.h"
#include <QHBoxLayout>
#include "IESLoader.h"
PolarDialog::PolarDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PolarDialog)
{
    ui->setupUi(this);

    m_cartesianWidget = new IESCartesianWidget();
     m_polarWidget = new IESPolarWidget();

     QHBoxLayout* hLayout = new QHBoxLayout();
     ui->widgetPolar->setLayout(hLayout);
     hLayout->addWidget(m_polarWidget);


     QHBoxLayout* hLayout_cartesian = new QHBoxLayout();
     ui->widgetCartesian->setLayout(hLayout_cartesian);
     hLayout_cartesian->addWidget(m_cartesianWidget);


     Qt::WindowFlags flags = Qt::Dialog;
     // 添加最大化和最小化按钮
     flags |= Qt::WindowMinMaxButtonsHint;
     // 添加关闭按钮
     flags |= Qt::WindowCloseButtonHint;
     setWindowFlags(flags);


     ui->horizontalSlider->setRange(0, 360);
     //ui->horizontalSlider_2->setRange(0, 360);
     connect(ui->chkFillBlue,SIGNAL(stateChanged(int)), this,SLOT(on_chkFillBlue_stateChanged(int)));
     connect(ui->chkFillRed, SIGNAL(stateChanged(int)), this, SLOT(on_chkFillRed_stateChanged(int)));
     connect(ui->chkViewRed, SIGNAL(stateChanged(int)), this, SLOT(on_chkViewRed_stateChanged(int)));

     connect(ui->chkFillGreen, SIGNAL(stateChanged(int)), this, SLOT(on_chkFillGreen_stateChanged(int)));
     connect(ui->chkFillYellow, SIGNAL(stateChanged(int)), this, SLOT(on_chkFillYellow_stateChanged(int)));
     connect(ui->chkViewYellow, SIGNAL(stateChanged(int)), this, SLOT(on_chkViewYellow_stateChanged(int)));

     ui->chkFillBlue->setChecked(true);
     ui->chkViewRed->setChecked(true);

     ui->chkFillYellow->setEnabled(false);
     ui->chkViewYellow->setChecked(false);
}

PolarDialog::~PolarDialog()
{
    delete ui;
}

void PolarDialog::updateCartesianIES()
{
    if (IESLoader::instance().light.m_IESType > 4)
    {
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setRange(0, 360);
        ui->horizontalSlider->setValue(90);
        ui->horizontalSlider->blockSignals(false);
        m_cartesianWidget->updateIES(90);
    }
    else
    {
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setRange(0, 360);
        ui->horizontalSlider->setValue(0);
        ui->horizontalSlider->blockSignals(false);
        m_cartesianWidget->updateIES(0);
    }
}
void PolarDialog::updatePolarIES()
{
    if (IESLoader::instance().light.m_IESType > 4)
    {
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setRange(0, 360);
        ui->horizontalSlider->setValue(90);
        ui->horizontalSlider->blockSignals(false);
        m_polarWidget->updateIES(90);
    }
    else
    {
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setRange(0, 360);
        ui->horizontalSlider->setValue(0);
        ui->horizontalSlider->blockSignals(false);
        m_polarWidget->updateIES(0);
    }
}

void PolarDialog::on_chkFillBlue_stateChanged(int value)
{
    m_polarWidget->on_chkFillBlue_stateChanged(value);
    m_cartesianWidget->on_chkFillBlue_stateChanged(value);
}
void PolarDialog::on_chkFillRed_stateChanged(int value)
{
    m_polarWidget->on_chkFillRed_stateChanged(value);
    m_cartesianWidget->on_chkFillRed_stateChanged(value);
}
void PolarDialog::on_chkViewRed_stateChanged(int value)
{     
    if (value)
    {
        ui->chkFillRed->setEnabled(true);
    }
    else
    {
        ui->chkFillRed->setEnabled(false);
    }
    m_polarWidget->on_chkViewRed_stateChanged(value);
    m_cartesianWidget->on_chkViewRed_stateChanged(value);

    on_chkFillRed_stateChanged(ui->chkFillRed->isChecked());
}

void PolarDialog::on_chkFillGreen_stateChanged(int value)
{
    m_polarWidget->on_chkFillGreen_stateChanged(value);
    m_cartesianWidget->on_chkFillGreen_stateChanged(value);
}
void PolarDialog::on_chkFillYellow_stateChanged(int value)
{
    m_polarWidget->on_chkFillYellow_stateChanged(value);
    m_cartesianWidget->on_chkFillYellow_stateChanged(value);
}
void PolarDialog::on_chkViewYellow_stateChanged(int value)
{
    if (value)
    {
        ui->chkFillYellow->setEnabled(true);
    }
    else
    {
        ui->chkFillYellow->setEnabled(false);
    }
    m_polarWidget->on_chkViewYellow_stateChanged(value);
    m_cartesianWidget->on_chkViewYellow_stateChanged(value);

    on_chkFillYellow_stateChanged(ui->chkFillYellow->isChecked());
   
}

void PolarDialog::on_horizontalSlider_valueChanged(int angle)
{
    m_polarWidget->on_horizontalSlider_valueChanged(angle);
    m_cartesianWidget->on_horizontalSlider_valueChanged(angle);
}

void PolarDialog::on_horizontalSlider_2_valueChanged(int angle)
{
    m_polarWidget->on_horizontalSlider_2_valueChanged(angle);
    m_cartesianWidget->on_horizontalSlider_valueChanged(angle);
}


void PolarDialog::on_cmbWidget_currentIndexChanged(int value)
{
    switch (value)
    {
    case ePolar:
    {
        ui->stackedWidget->setCurrentWidget(ui->widgetPolar);
        updatePolarIES();
    }
        break;
    case eCartesian:
    {
        ui->stackedWidget->setCurrentWidget(ui->widgetCartesian);
        updateCartesianIES();
    }
        break;
    default:
        break;
    }
}
void PolarDialog::updateIES()
{
    on_cmbWidget_currentIndexChanged(ui->cmbWidget->currentIndex());
}