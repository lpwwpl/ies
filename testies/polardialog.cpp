#include "polardialog.h"
#include "ui_polardialog.h"
#include "IESPolarWidget.h"
#include <QHBoxLayout>
PolarDialog::PolarDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PolarDialog)
{
    ui->setupUi(this);

     m_polarWidget = new IESPolarWidget();
     QHBoxLayout* hLayout = new QHBoxLayout();
     ui->widgetPolar->setLayout(hLayout);
     hLayout->addWidget(m_polarWidget);

     Qt::WindowFlags flags = Qt::Dialog;

     // 添加最大化和最小化按钮
     flags |= Qt::WindowMinMaxButtonsHint;

     // 添加关闭按钮
     flags |= Qt::WindowCloseButtonHint;

     setWindowFlags(flags);
}

PolarDialog::~PolarDialog()
{
    delete ui;
}

void PolarDialog::updateIES()
{
    m_polarWidget->updateIES();
}