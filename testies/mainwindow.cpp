#include "mainwindow.h"


#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QFileDialog>
#include "isoluxdialog.h"
#include "polardialog.h"
#include "threeDdialog.h"

#include <iostream>

#include "IESLoader.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    translator(nullptr)
{
    ui->setupUi(this);

    m_isoDialog = new ISOLuxDialog();
    m_3dDialog = new ThreeDDialog();
    m_polarDialog = new PolarDialog();

    ui->action3DCurves->setIcon(QIcon(":/resources/threeDcurves.png"));
    ui->actionIsoLuxCurves->setIcon(QIcon(":/resources/isoluxcurves.png"));
    ui->actionPolarCurves->setIcon(QIcon(":/resources/polarcurves.png"));
    ui->actionNew->setIcon(QIcon(":/resources/new.png"));
    ui->actionOpen->setIcon(QIcon(":/resources/open.png"));
    ui->actionSave->setIcon(QIcon(":/resources/save.png"));
    QPixmap whlImage(":/resources/image.png");
    ui->lblImage->setPixmap(whlImage);

    QPixmap typeAImage(":/resources/TypeA.png");
    ui->lblTypeA->setPixmap(typeAImage);
    QPixmap typeBImage(":/resources/TypeB.png");
    ui->lblTypeB->setPixmap(typeBImage);
    QPixmap typeCImage(":/resources/TypeC.png");
    ui->lblTypeC->setPixmap(typeCImage);

    ui->pbHadd->setIcon(QIcon(":/resources/Hadd.png"));
    ui->pbHplus->setIcon(QIcon(":/resources/Hminus.png"));
    ui->pbVadd->setIcon(QIcon(":/resources/Vadd.png"));
    ui->pbVplus->setIcon(QIcon(":/resources/Vminus.png"));
    // resize
    this->setBaseSize(1320, 845);
    QList<QDockWidget*> docks;
    QList<int> size;
    size.push_back(300);
    size.push_back(320);
    size.push_back(140);
    resizeDocks(docks, size, Qt::Orientation::Vertical);

    // init qrc
    Q_INIT_RESOURCE(res);

  
    ui->leNumberOf->setText(QString::number(1));
    ui->leLumenPer->setText(QString::number(5000));
    ui->leCandela->setText(QString::number(1));
    ui->leWatts->setText(QString::number(1000));
    ui->leBallast->setText(QString::number(1));
    ui->leFutur->setText(QString::number(1));

    ui->lblTypeA->setEnabled(false);
    ui->lblTypeB->setEnabled(false);

    ui->leWidth->setText(QString::number(0));
    ui->leLen->setText(QString::number(0));
    ui->leHeight->setText(QString::number(0));

    ui->rb4->setChecked(true);
    ui->rbV1->setChecked(true);
    ui->rbFeet->setChecked(true);

    ui->rbV0->setCheckable(false);
    ui->rbV2->setCheckable(false);
}

MainWindow::~MainWindow() { delete ui; }



void MainWindow::changeTheme(int index)
{
    QFile qss;
    switch (index)
    {
    case 0:
        qss.setFileName(":/res/theme/origin.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        //ui->statusBar->showMessage(tr("Origin Theme"), 2000);
        break;
    case 1:
        qss.setFileName(":/res/theme/light.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        //ui->statusBar->showMessage(tr("Light Theme"), 2000);
        break;
    case 2:
        qss.setFileName(":/res/theme/dark.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        //ui->statusBar->showMessage(tr("Dark Theme"), 2000);
        break;
    }
}

void MainWindow::changeLanguage(int index)

{
    switch (index)
    {
    case 0:
        if (translator != nullptr)
        {
            qApp->removeTranslator(translator);
            ui->retranslateUi(this);
        }
        break;
    case 1:
        if (translator == nullptr)
        {
            translator = new QTranslator;
            translator->load(":/res/trans/zh_CN.qm");
        }
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        break;
    }
}


void MainWindow::moveEvent(QMoveEvent* event)
{
    //QPoint pos = ui->cloudview->mapToGlobal(QPoint(0, 0));
    //emit ui->cloudview->posChanged(pos);
    return QMainWindow::moveEvent(event);
}

void MainWindow::on_actionIsoLuxCurves_triggered()
{
    m_isoDialog->updateIES();
    m_isoDialog->exec();

}
void MainWindow::on_actionPolarCurves_triggered()
{
    m_polarDialog->updateIES();
    m_polarDialog->exec();

}
void MainWindow::on_action3DCurves_triggered()
{
    m_3dDialog->updateIES();
    m_3dDialog->exec();
 
}
void MainWindow::on_actionNew_triggered()
{

}
void MainWindow::on_actionOpen_triggered()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"),
        "./", tr("IES files(*.ies);;All files(*.*)"));
    QFile file(filename);
    if (!file.exists())
        return;
    IESLoader::instance().loadIES(filename);


    ui->textEdit->setText(QString::fromStdString(IESLoader::instance().light.header));
    ui->leNumberOf->setText(QString::number(IESLoader::instance().light.number_lights));
    ui->leLumenPer->setText(QString::number(IESLoader::instance().light.lumens_per_lamp));
    //ui->leCandela->setText(QString::number(IESLoader::instance().light.num));
    ui->leWatts->setText(QString::number(IESLoader::instance().light.input_watts));
    ui->leBallast->setText(QString::number(IESLoader::instance().light.ballast_factor));
    ui->leFutur->setText(QString::number(IESLoader::instance().light.future_use));

    ui->leWidth->setText(QString::number(IESLoader::instance().light.width));
    ui->leLen->setText(QString::number(IESLoader::instance().light.length));
    ui->leHeight->setText(QString::number(IESLoader::instance().light.height));

    ui->lblTypeA->setEnabled(false);
    ui->lblTypeB->setEnabled(false);

    ui->rb1->setChecked(true);
    ui->rbV1->setChecked(true);
    ui->rbFeet->setChecked(true);

    ui->rb0->setEnabled(false);
    ui->rb2->setEnabled(false);
    ui->rb3->setEnabled(false);
    ui->rb4->setEnabled(false);

    ui->rbV0->setCheckable(false);
    ui->rbV2->setCheckable(false);
}
void MainWindow::on_actionSave_triggered()
{

}