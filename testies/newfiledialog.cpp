#include "newfiledialog.h"
#include "ui_newfile.h"
#include "common.h"

NewFileDialog::NewFileDialog(QWidget* parent) : QDialog(parent)
, ui(new Ui::newFile)
{
    ui->setupUi(this);
    init();
}

NewFileDialog::~NewFileDialog()
{

}

void NewFileDialog::init()
{
    ui->rb4->setChecked(true);
    ui->rbV1->setChecked(true);

    ui->rb1_AB->setChecked(true);
    ui->rbV0_AB->setChecked(true);

    ui->rbTypeC->setChecked(true);

    
    //ui->rbTypeA->setEnabled(false);
    ui->rb3->setEnabled(false);
}
void NewFileDialog::on_pbOk_clicked()
{
    QString version = ui->comboBox->currentText();
    if (ui->rbTypeC->isChecked())
    {
        EIES_VType vtype = eVNotSupoort;
        if (ui->rbV0->isChecked())
        {
            vtype = eC_V90;
        }
        else if (ui->rbV1->isChecked())
        {
            vtype = eC_V180;
        }
        else if (ui->rbV2->isChecked())
        {
            vtype = eC_V90_180;
        }

        if (ui->rb0->isChecked())
        {
            signalCreateTypeC_0(version,vtype);
        }
        else if (ui->rb1->isChecked())
        {
            signalCreateTypeC_0_90(version, vtype);
        }
        else if (ui->rb2->isChecked())
        {
            signalCreateTypeC_0_180(version, vtype);
        }
        else if (ui->rb3->isChecked())
        {
            signalCreateTypeC_90_270(version, vtype);
        }
        else if (ui->rb4->isChecked())
        {
            signalCreateTypeC_0_360(version, vtype);
        }
    }
    else if (ui->rbTypeB->isChecked())
    {
        EIES_VType vtype = eVNotSupoort;
        if (ui->rbV0_AB->isChecked())
        {
            vtype = eB_V_90_90;
        }
        else if (ui->rbV1_AB->isChecked())
        {
            vtype = eB_V90;
        }
        if (ui->rb0_AB->isChecked())
        {
            signalCreateTypeB_0_90(version, vtype); 
        }
        else if (ui->rb1_AB->isChecked())
        {
            signalCreateTypeB_m90_p90(version, vtype);
        }


    }
    else if (ui->rbTypeA->isChecked())
    {
        EIES_VType vtype = eVNotSupoort;
        if (ui->rbV0_AB->isChecked())
        {
            vtype = eA_V_90_90;
        }
        else if (ui->rbV1_AB->isChecked())
        {
            vtype = eA_V90;
        }
        if (ui->rb0_AB->isChecked())
        {
            signalCreateTypeA_0_90(version, vtype);
        }
        else if (ui->rb1_AB->isChecked())
        {
            signalCreateTypeA_m90_p90(version, vtype); 
        }
    }
    accept();
}
void NewFileDialog::on_pbCancel_clicked()
{
    accept();
}

void NewFileDialog::on_rbTypeC_toggled()
{
    ui->stackedWidget->setCurrentWidget(ui->pageC);
}
void NewFileDialog::on_rbTypeB_toggled()
{
    ui->stackedWidget->setCurrentWidget(ui->page_AB);
}
void NewFileDialog::on_rbTypeA_toggled()
{
    ui->stackedWidget->setCurrentWidget(ui->page_AB);
}