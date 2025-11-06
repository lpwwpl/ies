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
#include "TableItemDelegate.h"
#include <iostream>
#include <QMessageBox>
#include "SpotDiagramWidget.h"
#include "FieldViewWidget.h"
#include "MTFViewWidget.h"
#include "FootprintWidget.h"
#include "IESLoader.h"
#include "newfiledialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    translator(nullptr)
{
    ui->setupUi(this);
    setupHeaderEditing();
    m_fileState = eFile_clean;
    m_isoDialog = new ISOLuxDialog();
    m_3dDialog = new ThreeDDialog();
    m_polarDialog = new PolarDialog();
    m_newFileDialog = new NewFileDialog();

    ui->action3DCurves->setIcon(QIcon(":/resources/threeDcurves.png"));
    ui->actionIsoLuxCurves->setIcon(QIcon(":/resources/isoluxcurves.png"));
    ui->actionPolarCurves->setIcon(QIcon(":/resources/polarcurves.png"));
    ui->actionNew->setIcon(QIcon(":/resources/new.png"));
    ui->actionOpen->setIcon(QIcon(":/resources/open.png"));
    ui->actionSave->setIcon(QIcon(":/resources/save.png"));
    QPixmap whlImage(":/resources/image.png");
    ui->lblImage->setPixmap(whlImage);


    DoubleItemDelegate* delegate = new DoubleItemDelegate(ui->tableWidget);
    delegate->setRange(0.0, 1000000.0, 4); // 与ValidatedTableWidget中的设置一致
    ui->tableWidget->setItemDelegate(delegate); // 设置委托用于单元格
    bool value = connect(delegate, SIGNAL(editingFinished()), this, SLOT(slotDoubleValueChanged()));

    // 行操作
    value = connect(ui->actionAdd_Horizontal_Angle, &QAction::triggered, this, &MainWindow::addColumn);
    connect(ui->actionDelete_Horizontal_Angle, &QAction::triggered, this, &MainWindow::deleteColumn);

    // 列操作
    connect(ui->actionAdd_Vertical_Angle, &QAction::triggered, this, &MainWindow::addRow);
    connect(ui->actionDelete_Vertical_Angle, &QAction::triggered, this, &MainWindow::deleteRow);
   
    connect(ui->pbVplus, SIGNAL(clicked()), this, SLOT(deleteRow()));
    connect(ui->pbVadd, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(ui->pbHplus, SIGNAL(clicked()), this, SLOT(deleteColumn()));
    connect(ui->pbHadd, SIGNAL(clicked()), this, SLOT(addColumn()));

    QPixmap typeAImage(":/resources/TypeA.png");
    ui->lblTypeA->setPixmap(typeAImage);
    QPixmap typeBImage(":/resources/TypeB.png");
    ui->lblTypeB->setPixmap(typeBImage);
    QPixmap typeCImage(":/resources/TypeC.png");
    ui->lblTypeC->setPixmap(typeCImage);

    ui->pbHadd->setIcon(QIcon(":/resources/Hadd.png"));
    ui->pbHadd->setIconSize(QSize(32, 32));
    ui->pbHplus->setIcon(QIcon(":/resources/Hminus.png"));
    ui->pbHplus->setIconSize(QSize(32, 32));
    ui->pbVadd->setIcon(QIcon(":/resources/Vadd.png"));
    ui->pbVadd->setIconSize(QSize(32, 32));
    ui->pbVplus->setIcon(QIcon(":/resources/Vminus.png"));
    ui->pbVplus->setIconSize(QSize(32, 32));
    // resize
    this->setBaseSize(1320, 845);
    QList<QDockWidget*> docks;
    QList<int> size;
    size.push_back(300);
    size.push_back(320);
    size.push_back(140);
    //resizeDocks(docks, size, Qt::Orientation::Vertical);

    // init qrc
    Q_INIT_RESOURCE(res);

    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgba(0,255,0,60%); }");
    ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section { background-color: rgba(0,0,255,60%); }");

  
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

    
    m_heditor = new QLineEdit(ui->tableWidget->horizontalHeader()->viewport());
    m_veditor = new QLineEdit(ui->tableWidget->verticalHeader()->viewport());
    m_heditor->installEventFilter(this);
    m_veditor->installEventFilter(this);
    QDoubleValidator* hvalidator = new QDoubleValidator(m_heditor);
    hvalidator->setNotation(QDoubleValidator::StandardNotation);
    hvalidator->setRange(0.0, 90, 2); // 设置范围 0-360，最多6位小数
    m_heditor->setValidator(hvalidator);

    QDoubleValidator* vvalidator = new QDoubleValidator(m_veditor);
    vvalidator->setNotation(QDoubleValidator::StandardNotation);
    vvalidator->setRange(0.0, 180, 2); // 设置范围 0-360，最多6位小数
    m_veditor->setValidator(vvalidator);

    m_heditor->close();
    m_veditor->close();

    // 当编辑器完成编辑时，更新表头并删除编辑器
    connect(m_heditor, &QLineEdit::editingFinished, this, &MainWindow::slot_heditor);
    connect(m_veditor, &QLineEdit::editingFinished, this, &MainWindow::slot_veditor);

    sortAction = new QAction("Sort", this);
    connect(sortAction, &QAction::triggered, this, &MainWindow::sortTable);

    autoSortEnabled = (false);
    autoSortAction = new QAction("Auto-Sort Enabled", this);
    autoSortAction->setCheckable(true);
    autoSortAction->setChecked(autoSortEnabled);
    connect(autoSortAction, &QAction::toggled, this, &MainWindow::enableAutoSort);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested,
        this, &MainWindow::showContextMenu);

    m_editor_focusOut = false;


    value = connect(ui->actionspot, &QAction::triggered, this, &MainWindow::showSpot);
    connect(ui->actiondistort, &QAction::triggered, this, &MainWindow::showDistort);
    value = connect(ui->actionfov, &QAction::triggered, this, &MainWindow::showFov);
    connect(ui->actionTrace, &QAction::triggered, this, &MainWindow::showTrace);
    connect(ui->actionMTF, &QAction::triggered, this, &MainWindow::showMTF);


    connect(m_newFileDialog, SIGNAL(signalCreateTypeC_0(QString,EIES_VType)), this, SLOT(slotCreateTypeC_0(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeC_0_90(QString, EIES_VType)), this, SLOT(slotCreateTypeC_0_90(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeC_0_180(QString, EIES_VType)), this, SLOT(slotCreateTypeC_0_180(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeC_90_270(QString, EIES_VType)), this, SLOT(slotCreateTypeC_90_270(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeC_0_360(QString, EIES_VType)), this, SLOT(slotCreateTypeC_0_360(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeB_m90_p90(QString, EIES_VType)), this, SLOT(slotCreateTypeB_m90_p90(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeB_0_90(QString, EIES_VType)), this, SLOT(slotCreateTypeB_0_90(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeA_m90_p90(QString, EIES_VType)), this, SLOT(slotCreateTypeA_m90_p90(QString, EIES_VType)));
    connect(m_newFileDialog, SIGNAL(signalCreateTypeA_0_90(QString, EIES_VType)), this, SLOT(slotCreateTypeA_0_90(QString, EIES_VType)));

    //default
    slotCreateTypeC_0_360("IESNA:LM-63-2002", eC_V180);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::slot_heditor()
{    
    if (m_editor_focusOut)return;

    if (m_heditor->text().isEmpty())
    {
        ui->tableWidget->setHorizontalHeaderItem(logicalIndex, new QTableWidgetItem(m_editor_old_value));
    }
    else
    {
        ui->tableWidget->setHorizontalHeaderItem(logicalIndex, new QTableWidgetItem(m_heditor->text()));
        m_fileState = eFile_dirty;
        this->setWindowTitle("[" + m_filename + " *]");
    }
    m_heditor->clearFocus();
    m_heditor->close();
    if (autoSortEnabled) {
        sortHorizontalAngles();
    }
    m_editor_focusOut = true;
}
void MainWindow::slot_veditor()
{
    if (m_editor_focusOut)return;

    if (m_heditor->text().isEmpty())
    {
        ui->tableWidget->setVerticalHeaderItem(logicalIndex, new QTableWidgetItem(m_editor_old_value));
    }
    else
    {
        ui->tableWidget->setVerticalHeaderItem(logicalIndex, new QTableWidgetItem(m_veditor->text()));
        m_fileState = eFile_dirty;
        this->setWindowTitle("[" + m_filename + " *]");
    }
    m_veditor->clearFocus();
    m_veditor->close();
    if (autoSortEnabled) {
        //sortHorizontalAngles();
        sortVerticalAngles();
    }
    m_editor_focusOut = true;
}
void MainWindow::setupHeaderEditing()
{
    // 启用表头双击编辑
    ui->tableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->tableWidget->verticalHeader()->setSectionsClickable(true);

    ui->tableWidget->horizontalHeader()->viewport()->installEventFilter(this);
    ui->tableWidget->verticalHeader()->viewport()->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (qobject_cast<QLineEdit*>(obj) && obj == m_heditor)
    {
        if (event->type() == QEvent::FocusOut) {
            // 失去焦点，直接关闭编辑器，不更新（视为取消）
            m_editor_focusOut = false;
            slot_heditor();
 
            return true;
        }
    }
    else if (qobject_cast<QLineEdit*>(obj) && obj == m_veditor)
    {
        if (event->type() == QEvent::FocusOut) {
            // 失去焦点，直接关闭编辑器，不更新（视为取消）
            m_editor_focusOut = false;
            slot_veditor();  

            return true;
        }
    }
    else if (obj == ui->tableWidget->horizontalHeader()->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            logicalIndex = ui->tableWidget->horizontalHeader()->logicalIndexAt(mouseEvent->pos());
            if (logicalIndex != -1) {   
                m_heditor->setGeometry(ui->tableWidget->horizontalHeader()->sectionViewportPosition(logicalIndex), 0,
                    ui->tableWidget->horizontalHeader()->sectionSize(logicalIndex), ui->tableWidget->horizontalHeader()->height());
                m_heditor->setText(ui->tableWidget->horizontalHeaderItem(logicalIndex)->text());
                m_editor_focusOut = false;
                m_editor_old_value = ui->tableWidget->horizontalHeaderItem(logicalIndex)->text();
                m_heditor->selectAll();
                m_heditor->show();
                m_heditor->setFocus();
                return true;
            }
        }
    }
    else if ( obj == ui->tableWidget->verticalHeader()->viewport())
    {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            logicalIndex = ui->tableWidget->verticalHeader()->logicalIndexAt(mouseEvent->pos());
            if (logicalIndex != -1) 
            {                
                m_veditor->setGeometry( 0, ui->tableWidget->verticalHeader()->sectionViewportPosition(logicalIndex),
                    ui->tableWidget->verticalHeader()->width(), ui->tableWidget->verticalHeader()->sectionSize(logicalIndex));
                m_veditor->setText(ui->tableWidget->verticalHeaderItem(logicalIndex)->text());
                m_editor_focusOut = false;
                m_editor_old_value = ui->tableWidget->verticalHeaderItem(logicalIndex)->text();
                m_veditor->selectAll();
                m_veditor->show();
                m_veditor->setFocus();
                return true;
            }
        } 
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::slotDoubleValueChanged()
{
    QModelIndex index = ui->tableWidget->currentIndex();
    uint row = index.row();
    uint col = index.column();

    double value = index.data(Qt::DisplayRole).toDouble();

    m_fileState = eFile_dirty;
    this->setWindowTitle("[" + m_filename + " *]");
    //IESLoader::instance().light.candela_hv[col][row] = value;
}
void MainWindow::addRow()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        currentRow = 0;
    }

    // 保存行数据到剪贴板
    rowClipboard.clear();
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QTableWidgetItem* item = ui->tableWidget->item(currentRow, col);
        if (item) {
            rowClipboard.append(item->text());
        }
        else {
            rowClipboard.append("");
        }
    }

    // 保存垂直表头
    QTableWidgetItem* headerItem = ui->tableWidget->verticalHeaderItem(currentRow);
    if (headerItem) {
        verticalHeaderClipboard = headerItem->text();
    }



    ui->tableWidget->insertRow(currentRow);

    // 设置垂直表头
    if (!verticalHeaderClipboard.isEmpty()) {
        QTableWidgetItem* headerItem = new QTableWidgetItem(verticalHeaderClipboard);
        ui->tableWidget->setVerticalHeaderItem(currentRow, headerItem);
    }

    // 粘贴数据
    for (int col = 0; col < ui->tableWidget->columnCount() && col < rowClipboard.size(); ++col) {
        QTableWidgetItem* item = new QTableWidgetItem(rowClipboard[col]);
        ui->tableWidget->setItem(currentRow, col, item);
    }
    m_fileState = eFile_dirty;
    this->setWindowTitle("[" + m_filename + " *]");
    //if (autoSortEnabled) {
    //    sortTableByHorizontalAngle();
    //    sortTableByVerticalAngle();
    //}
}


void MainWindow::NewHeader(tiny_ies<double>::light& light)
{
    std::string header;
    light.properties.emplace("TESTLAB","Test laboratory");
    light.properties.emplace("ISSUEDATE", "Creation date of this file");
    light.properties.emplace("TEST", "Test report number and laboratory");
    light.properties.emplace("MANUFAC", "Manufacturer of luminaire");
    light.properties.emplace("LUMCAT", "Luminaire catalog number");
    light.properties.emplace("LUMINAIRE", "Luminaire description");
    light.properties.emplace("LAMPCAT", "Lamp catalogue number");
    light.properties.emplace("LAMP", "Lamp description"); 
    light.tilt = "NONE";
}
void MainWindow::deleteRow()
{
    if (ui->tableWidget->rowCount() <= 2)
        return;
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        QMessageBox::warning(this, "Delete Row", "Please select a row to delete.");
        return;
    }

    //QMessageBox::StandardButton reply = QMessageBox::question(
    //    this, "Delete Row", "Are you sure you want to delete the selected row?",
    //    QMessageBox::Yes | QMessageBox::No);

    //if (reply == QMessageBox::Yes) {
    //    ui->tableWidget->removeRow(currentRow);
    //}
    ui->tableWidget->removeRow(currentRow);
    m_fileState = eFile_dirty;
    this->setWindowTitle("[" + m_filename + " *]");
}

void MainWindow::addColumn()
{
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn == -1) {
        currentColumn = 0;
    }

    // 保存列数据到剪贴板
    columnClipboard.clear();
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = ui->tableWidget->item(row, currentColumn);
        if (item) {
            columnClipboard.append(item->text());
        }
        else {
            columnClipboard.append("");
        }
    }

    // 保存水平表头
    QTableWidgetItem* headerItem = ui->tableWidget->horizontalHeaderItem(currentColumn);
    if (headerItem) {
        horizontalHeaderClipboard = headerItem->text();
    }

    ui->tableWidget->insertColumn(currentColumn);

    // 设置水平表头
    if (!horizontalHeaderClipboard.isEmpty()) {
        QTableWidgetItem* headerItem = new QTableWidgetItem(horizontalHeaderClipboard);
        ui->tableWidget->setHorizontalHeaderItem(currentColumn, headerItem);
    }

    // 粘贴数据
    for (int row = 0; row < ui->tableWidget->rowCount() && row < columnClipboard.size(); ++row) {
        QTableWidgetItem* item = new QTableWidgetItem(columnClipboard[row]);
        ui->tableWidget->setItem(row, currentColumn, item);
    }
    m_fileState = eFile_dirty;
    this->setWindowTitle("[" + m_filename + " *]");
}

void MainWindow::deleteColumn()
{
    if (ui->tableWidget->columnCount() <= 2)
        return;
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn == -1) {
        QMessageBox::warning(this, "Delete Column", "Please select a column to delete.");
        return;
    }

    //QMessageBox::StandardButton reply = QMessageBox::question(
    //    this, "Delete Column", "Are you sure you want to delete the selected column?",
    //    QMessageBox::Yes | QMessageBox::No);

    //if (reply == QMessageBox::Yes) {
    //    ui->tableWidget->removeColumn(currentColumn);
    //}
    ui->tableWidget->removeColumn(currentColumn);
    m_fileState = eFile_dirty;
    this->setWindowTitle("[" + m_filename + " *]");
}


void MainWindow::populateTableFromIESData()
{
    if (!IESLoader::instance().light.candela_hv.size() > 0) {
        return;
    }

    std::vector<double> verticalAngles = IESLoader::instance().light.vertical_angles;
    std::vector<double> horizontalAngles = IESLoader::instance().light.horizontal_angles;
    std::vector<std::vector<double>> intensityData = IESLoader::instance().light.candela_hv;

    // 设置表格尺寸
    ui->tableWidget->setColumnCount(horizontalAngles.size());
    ui->tableWidget->setRowCount(verticalAngles.size());

    // 设置垂直表头（垂直角度）
    for (int row = 0; row < verticalAngles.size(); ++row) {
        QTableWidgetItem* headerItem = new QTableWidgetItem(
            QString("%1").arg(verticalAngles[row], 0, 'f', 1));
        ui->tableWidget->setVerticalHeaderItem(row, headerItem);
    }

    // 设置水平表头（水平角度）
    for (int col = 0; col < horizontalAngles.size(); ++col) {
        QTableWidgetItem* headerItem = new QTableWidgetItem(
            QString("%1").arg(horizontalAngles[col], 0, 'f', 1));
        ui->tableWidget->setHorizontalHeaderItem(col, headerItem);
    }

    // 填充光强数据
    for (int row = 0; row < verticalAngles.size(); ++row) {
        for (int col = 0; col < horizontalAngles.size(); ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(
                QString::number(intensityData[col][row], 'f', 2));
            ui->tableWidget->setItem(row, col, item);
        }
    }

    // 调整列宽
    ui->tableWidget->resizeColumnsToContents();
    m_fileState = eFile_clean;
    this->setWindowTitle("[" + m_filename + "]");
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
    //ui->tableWidget->clear();

    if (m_filepath.isEmpty())
    {
        SaveAsFile();
    }
    else
    {
        SaveFile();
    }

    m_newFileDialog->init();
    m_newFileDialog->exec();


    //m_filepath = "";
    //m_fileState = eFile_clean;
    //m_filename = "No Name";
    //this->setWindowTitle("[" + m_filename + "]");
}

void MainWindow::on_actionLight_triggered()
{
    QFile qss;
    qss.setFileName(":/resources/qss/dark_style_sheet/qdarkstyle/lightstyle.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}
void MainWindow::on_actionDark_triggered()
{
    QFile qss;
    qss.setFileName(":/resources/qss/dark_style_sheet/qdarkstyle/style.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}

void MainWindow::fillUI(/*EIESType htype, EIES_VType vtype*/)
{
    std::string header;
    for (std::map<std::string, std::string>::iterator it = IESLoader::instance().light.properties.begin(); it != IESLoader::instance().light.properties.end(); ++it)
    {      
        std::string temp = "[";
        temp += it->first;
        temp += "]";
        temp += " ";
        temp += it->second;
        temp += "\n";
        header += temp;
    }
    ui->textEdit->setText(QString::fromStdString(header));
    ui->leNumberOf->setText(QString::number(IESLoader::instance().light.number_lights));
    ui->leLumenPer->setText(QString::number(IESLoader::instance().light.lumens_per_lamp));
    //ui->leCandela->setText(QString::number(IESLoader::instance().light.num));
    ui->leWatts->setText(QString::number(IESLoader::instance().light.input_watts));
    ui->leBallast->setText(QString::number(IESLoader::instance().light.ballast_factor));
    ui->leFutur->setText(QString::number(IESLoader::instance().light.future_use));

    ui->leWidth->setText(QString::number(IESLoader::instance().light.width));
    ui->leLen->setText(QString::number(IESLoader::instance().light.length));
    ui->leHeight->setText(QString::number(IESLoader::instance().light.height));

    EIESType htype = IESLoader::instance().light.m_IESType;
    EIES_VType vtype = IESLoader::instance().light.m_vType;
    switch (htype)
    {
    case eC0:
    {
        ui->rb0->setEnabled(true);
        ui->rb0->setChecked(true);
        ui->rb1->setEnabled(false);
        ui->rb2->setEnabled(false);
        ui->rb3->setEnabled(false);
        ui->rb4->setEnabled(false);

        ui->lblTypeC->setEnabled(true);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(false);

        ui->stackedWidget->setCurrentWidget(ui->pageC);
    }
    break;
    case eC90:
    {
        ui->rb0->setEnabled(false);
        ui->rb1->setChecked(true);
        ui->rb1->setEnabled(true);
        ui->rb2->setEnabled(false);
        ui->rb3->setEnabled(false);
        ui->rb4->setEnabled(false);

        ui->lblTypeC->setEnabled(true);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(false);

        ui->stackedWidget->setCurrentWidget(ui->pageC);
    }
    break;
    case eC180:
    {
        ui->rb0->setEnabled(false);
        ui->rb1->setEnabled(false);
        ui->rb2->setEnabled(true);
        ui->rb2->setChecked(true);
        ui->rb3->setEnabled(false);
        ui->rb4->setEnabled(false);

        ui->lblTypeC->setEnabled(true);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(false);

        ui->stackedWidget->setCurrentWidget(ui->pageC);
    }
    break;
    case eC270:
    {
        ui->rb0->setEnabled(false);
        ui->rb1->setEnabled(false);
        ui->rb2->setEnabled(false);
        ui->rb3->setEnabled(true);
        ui->rb3->setChecked(true);
        ui->rb4->setEnabled(false);

        ui->lblTypeC->setEnabled(true);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(false);

        ui->stackedWidget->setCurrentWidget(ui->pageC);
    }
    break;
    case eC360:
    {
        ui->rb0->setEnabled(false);
        ui->rb1->setEnabled(false);
        ui->rb2->setEnabled(false);
        ui->rb3->setEnabled(false);
        ui->rb4->setEnabled(true);
        ui->rb4->setChecked(true);

        ui->lblTypeC->setEnabled(true);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(false);

        ui->stackedWidget->setCurrentWidget(ui->pageC);
    }
    break;
    case eB_9090:
    {
        ui->lblTypeC->setEnabled(false);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(true);


        ui->rb0_AB->setEnabled(false);
        ui->rb1_AB->setChecked(true);
        ui->rb1_AB->setEnabled(true);

        ui->stackedWidget->setCurrentWidget(ui->page_AB);
    }
    break;
    case eB090:
    {
        ui->lblTypeC->setEnabled(false);
        ui->lblTypeA->setEnabled(false);
        ui->lblTypeB->setEnabled(true);

        ui->rb1_AB->setEnabled(false);
        ui->rb0_AB->setChecked(true);
        ui->rb0_AB->setEnabled(true);

        ui->stackedWidget->setCurrentWidget(ui->page_AB);
    }
    break;
    case eA_9090:
    {
        ui->lblTypeC->setEnabled(false);
        ui->lblTypeA->setEnabled(true);
        ui->lblTypeB->setEnabled(false);

        ui->rb0_AB->setEnabled(false);
        ui->rb1_AB->setChecked(true);
        ui->rb1_AB->setEnabled(true);

        ui->stackedWidget->setCurrentWidget(ui->page_AB);
    }
    break;
    case eA090:
    {
        ui->lblTypeC->setEnabled(false);
        ui->lblTypeA->setEnabled(true);
        ui->lblTypeB->setEnabled(false);

        ui->rb1_AB->setEnabled(false);
        ui->rb0_AB->setChecked(true);
        ui->rb0_AB->setEnabled(true);

        ui->stackedWidget->setCurrentWidget(ui->page_AB);
    }
    default:
        break;
    }
    switch (vtype)
    {
    case eC_V180:
    {
        ui->rbV0->setCheckable(false);
        ui->rbV2->setCheckable(false);
        ui->rbV1->setCheckable(true);
        ui->rbV1->setChecked(true);
    }
    break;
    case eC_V90:
    {
        ui->rbV0->setCheckable(true);
        ui->rbV0->setChecked(true);
        ui->rbV2->setCheckable(false);
        ui->rbV1->setCheckable(false);
    }
    break;
    case eC_V90_180:
    {
        ui->rbV2->setCheckable(true);
        ui->rbV2->setChecked(true);
        ui->rbV0->setCheckable(false);
        ui->rbV1->setCheckable(false);
    }
        break;
    case eB_V90:
    case eA_V90:
    {
        ui->rbV1_AB->setCheckable(true);
        ui->rbV1_AB->setChecked(true);
        ui->rbV0_AB->setChecked(false);
        ui->rbV0_AB->setCheckable(false);
    }
    break;
    case eB_V_90_90:
    case eA_V_90_90:
    {
        ui->rbV0_AB->setCheckable(true);
        ui->rbV0_AB->setChecked(true);
        ui->rbV1_AB->setChecked(false);
        ui->rbV1_AB->setCheckable(false);
    }
        break;
    }

    ui->rbFeet->setChecked(true);
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


    fillUI();

    populateTableFromIESData();

    m_fileState = eFile_clean;
    m_filepath = filename;
    QFileInfo fileInfo(m_filepath);
    m_filename = fileInfo.fileName();
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::on_actionSave_triggered()
{
    QFile file(m_filepath);
    if (file.exists())
    {
        tiny_ies<double>::light light =
            IESLoader::instance().light;
        // static bool write_ies(const std::string& filename, const light& ies, const uint32_t precision = std::numeric_limits<T>::max_digits10) 
        sortTable(true);
        light.candela_hv = getIntensityData();
        light.horizontal_angles = getHorizontalAngles();
        light.vertical_angles = getVerticalAngles();
        light.number_horizontal_angles = light.horizontal_angles.size();
        light.number_vertical_angles = light.vertical_angles.size();
        light.candela.clear();
        for (uint32_t i = 0; i < light.number_horizontal_angles; i++) {
            for (uint32_t j = 0; j < light.number_vertical_angles; j++)
            {
                light.candela.push_back(light.candela_hv[j][i] / light.multiplier);
            }
        }
        tiny_ies<double>::write_ies(m_filepath.toStdString(), light);
    }
    else
    {
        on_actionSave_As_triggered();
    }
    m_fileState = eFile_clean;
    this->setWindowTitle("[" + m_filename + "]");
}

void MainWindow::on_actionSave_As_triggered()
{
    auto filepath = QFileDialog::getSaveFileName(
        this, tr("Save IES"), "", tr("Ies File(*.ies);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    sortTable(true);
    tiny_ies<double>::light light =
        IESLoader::instance().light;
    light.candela_hv = getIntensityData();
    light.horizontal_angles = getHorizontalAngles();
    light.vertical_angles = getVerticalAngles();
    light.number_horizontal_angles = light.horizontal_angles.size();
    light.number_vertical_angles = light.vertical_angles.size();
    light.candela.clear();
    for (uint32_t i = 0; i < light.number_horizontal_angles; i++) {
        for (uint32_t j = 0; j < light.number_vertical_angles; j++)
        {
            light.candela.push_back(light.candela_hv[j][i] / light.multiplier);
        }
    }
    tiny_ies<double>::write_ies(filepath.toStdString(), light);
    m_fileState = eFile_clean;
    m_filepath = filepath;
    QFileInfo fileInfo(m_filepath);
    m_filename = fileInfo.fileName();
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::SaveFile()
{
    switch (m_fileState)
    {
    case eFile_clean:
    {
       
    }
    break;
    case eFile_dirty:
    {
        if (!m_filepath.isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::warning(this, "IESNA LM-63", QString("Save changes to %1 ?").arg(m_filepath),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                QMessageBox::Yes);

            if (reply == QMessageBox::Yes) {
                // Handle Save action
                on_actionSave_triggered();
            }
            else if (reply == QMessageBox::No) {
                // Handle Discard action

            }
            else if (reply == QMessageBox::Cancel) {
                // Handle Cancel action
            }
        }
    }
    break;
    default:
        break;
    }
}
void MainWindow::SaveAsFile()
{
    switch (m_fileState)
    {
    case eFile_clean:
    {
       
    }
    break;
    case eFile_dirty:
    {
        if (m_filepath.isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::warning(this, "IESNA LM-63", "Save changes",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                QMessageBox::Yes);

            if (reply == QMessageBox::Yes) {
                // Handle Save action
                on_actionSave_As_triggered();
            }
            else if (reply == QMessageBox::No) {
                // Handle Discard action

            }
            else if (reply == QMessageBox::Cancel) {
                // Handle Cancel action
            }
        }
    }
    break;
    default:
        break;
    }
}
void MainWindow::on_actionExit_triggered()
{
    close();
}



std::vector<double> MainWindow::getHorizontalAngles() const
{
    std::vector<double> angles;
    for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
        QTableWidgetItem* header = ui->tableWidget->horizontalHeaderItem(col);
        if (header) {
            bool ok;
            double angle = header->text().toDouble(&ok);
            if (ok) angles.push_back(angle);
        }
    }
    return angles;
}

std::vector<double> MainWindow::getVerticalAngles() const
{
    std::vector<double> angles;
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        QTableWidgetItem* header = ui->tableWidget->verticalHeaderItem(row);
        if (header) {
            bool ok;
            double angle = header->text().toDouble(&ok);
            if (ok) angles.push_back(angle);
        }
    }
    return angles;
}

std::vector<std::vector<double>> MainWindow::getIntensityData() const
{
    std::vector<std::vector<double>> data;
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        std::vector<double> rowData;
        for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            if (item) {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok) rowData.push_back(value);
                else rowData.push_back(0.0);
            }
            else {
                rowData.push_back(0.0);
            }
        }
        data.push_back(rowData);
    }
    return data;
}

void MainWindow::sortTable(bool)
{
    sortHorizontalAngles();
    sortVerticalAngles();
}
void MainWindow::enableAutoSort(bool enable)
{
    autoSortEnabled = enable;
    autoSortAction->setChecked(enable);
    if (enable) {
        // 启用时立即排序
        sortHorizontalAngles();
        sortVerticalAngles();
    }
}


void MainWindow::sortHorizontalAngles()
{
    // 获取当前水平角度
    std::vector<double> horizontalAngles = getHorizontalAngles();

    // 创建索引-角度对
    std::vector<QPair<double, int>> angleIndexPairs;
    for (int i = 0; i < horizontalAngles.size(); i++) {
        angleIndexPairs.push_back(qMakePair(horizontalAngles[i], i));
    }

    // 按角度升序排序
    std::sort(angleIndexPairs.begin(), angleIndexPairs.end());

    // 创建新的列顺序
    std::vector<int> newColumnOrder;
    for (const auto& pair : angleIndexPairs) {
        newColumnOrder.push_back(pair.second);
    }

    // 重新排列表格数据
    rearrangeTableDataAfterHorizontalSort(newColumnOrder);
}

void MainWindow::rearrangeTableDataAfterHorizontalSort(const std::vector<int>& newColumnOrder)
{
    // 保存当前数据
    std::vector<double> horizontalAngles = getHorizontalAngles();
    std::vector<double> verticalAngles = getVerticalAngles();
    std::vector<std::vector<double>> intensityData = getIntensityData();

    // 创建新的水平角度和强度数据
    std::vector<double> newHorizontalAngles(horizontalAngles.size());
    std::vector<std::vector<double>> newIntensityData(verticalAngles.size());

    for (int i = 0; i < verticalAngles.size(); i++) {
        newIntensityData[i].resize(horizontalAngles.size());
    }

    // 按照新的列顺序重新排列数据
    for (int newCol = 0; newCol < newColumnOrder.size(); newCol++) {
        int oldCol = newColumnOrder[newCol];
        newHorizontalAngles[newCol] = horizontalAngles[oldCol];

        for (int row = 0; row < verticalAngles.size(); row++) {
            newIntensityData[row][newCol] = intensityData[row][oldCol];
        }
    }

    // 更新表格
    setTableData(newHorizontalAngles, verticalAngles, newIntensityData);
}

void MainWindow::setTableData(const std::vector<double>& horizontalAngles,
    const std::vector<double>& verticalAngles,
    const std::vector<std::vector<double>>& intensityData)
{
    // 设置表格尺寸
    ui->tableWidget->setRowCount(verticalAngles.size());
    ui->tableWidget->setColumnCount(horizontalAngles.size());

    // 设置水平表头
    for (int col = 0; col < horizontalAngles.size(); col++) {
        ui->tableWidget->setHorizontalHeaderItem(col,
            new QTableWidgetItem(QString::number(horizontalAngles[col], 'f', 2)));
    }

    // 设置垂直表头
    for (int row = 0; row < verticalAngles.size(); row++) {
        ui->tableWidget->setVerticalHeaderItem(row,
            new QTableWidgetItem(QString::number(verticalAngles[row], 'f', 2)));
    }

    // 填充强度数据
    for (int row = 0; row < verticalAngles.size(); row++) {
        for (int col = 0; col < horizontalAngles.size(); col++) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            if (!item) {
                item = new QTableWidgetItem;
                ui->tableWidget->setItem(row, col, item);
            }
            item->setText(QString::number(intensityData[row][col], 'f', 2));
        }
    }
}

void MainWindow::sortVerticalAngles()
{
    // 获取当前垂直角度
    std::vector<double> verticalAngles = getVerticalAngles();

    // 创建索引-角度对
    std::vector<QPair<double, int>> angleIndexPairs;
    for (int i = 0; i < verticalAngles.size(); i++) {
        angleIndexPairs.push_back(qMakePair(verticalAngles[i], i));
    }

    // 按角度升序排序
    std::sort(angleIndexPairs.begin(), angleIndexPairs.end());

    // 创建新的行顺序
    std::vector<int> newRowOrder;
    for (const auto& pair : angleIndexPairs) {
        newRowOrder.push_back(pair.second);
    }

    // 重新排列表格数据
    rearrangeTableDataAfterVerticalSort(newRowOrder);
}

void MainWindow::rearrangeTableDataAfterVerticalSort(const std::vector<int>& newRowOrder)
{
    // 保存当前数据
    std::vector<double> horizontalAngles = getHorizontalAngles();
    std::vector<double> verticalAngles = getVerticalAngles();
    std::vector<std::vector<double>> intensityData = getIntensityData();

    // 创建新的垂直角度和强度数据
    std::vector<double> newVerticalAngles(verticalAngles.size());
    std::vector<std::vector<double>> newIntensityData(verticalAngles.size());

    for (int i = 0; i < verticalAngles.size(); i++) {
        newIntensityData[i].resize(horizontalAngles.size());
    }

    // 按照新的行顺序重新排列数据
    for (int newRow = 0; newRow < newRowOrder.size(); newRow++) {
        int oldRow = newRowOrder[newRow];
        newVerticalAngles[newRow] = verticalAngles[oldRow];

        for (int col = 0; col < horizontalAngles.size(); col++) {
            newIntensityData[newRow][col] = intensityData[oldRow][col];
        }
    }

    // 更新表格
    setTableData(horizontalAngles, newVerticalAngles, newIntensityData);
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu("Context Menu", this);

    contextMenu.addAction(autoSortAction);  
    contextMenu.addAction(sortAction);
    contextMenu.exec(ui->tableWidget->viewport()->mapToGlobal(pos));
}


void MainWindow::showSpot()
{
    auto filepath = QFileDialog::getOpenFileName(
        this, tr("Open txt"), "", tr("txt File(*.txt);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    SpotDiagramPlotter* w = new SpotDiagramPlotter();
    w->loadDataFromFile(filepath);
    w->plotSpotDiagrams();
    w->show();
}
void MainWindow::showTrace()
{
    auto filepath = QFileDialog::getOpenFileName(
        this, tr("Open txt"), "", tr("txt File(*.txt);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    FootprintWidget* footprint = new FootprintWidget();
    footprint->loadDataFromFile(filepath);
    footprint->plotFootprints();
    footprint->show();
}
void MainWindow::showMTF()
{
    auto filepath = QFileDialog::getOpenFileName(
        this, tr("Open txt"), "", tr("txt File(*.txt);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    MTFViewer* m_mtfViewer = new MTFViewer();
    m_mtfViewer->setTitle("MTF曲线分析");
    m_mtfViewer->loadFromFile(filepath);
    m_mtfViewer->show();
}
void MainWindow::showFov()
{
    
    auto filepath = QFileDialog::getOpenFileName(
        this, tr("Open txt"), "", tr("txt File(*.txt);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    FieldViewWidget* fieldView = new FieldViewWidget();
    fieldView->loadFieldDataFile(filepath);
    //fieldView->updatePlot();
    fieldView->show();
}
void MainWindow::showDistort()
{
    auto filepath = QFileDialog::getOpenFileName(
        this, tr("Open txt"), "", tr("txt File(*.txt);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
}


void MainWindow::slotCreateTypeC_0(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eC0;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 1;
    light.horizontal_angles.push_back(0);
    switch (vtype)
    {
    case eC_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eC_V180:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(180);
    }
    break;
    case eC_V90_180:
    {
        light.vertical_angles.push_back(90);
        light.vertical_angles.push_back(180);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 1;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();
}


void MainWindow::fillLight(tiny_ies<double>::light& ies_out)
{
    ies_out.candela_hv.resize(static_cast<uint64_t>(ies_out.number_horizontal_angles));
    ies_out.candela.resize(static_cast<uint64_t>(ies_out.number_vertical_angles) * static_cast<uint64_t>(ies_out.number_horizontal_angles));
    ies_out.max_candela = 0.0;
    for (uint32_t i = 0; i < ies_out.number_vertical_angles * ies_out.number_horizontal_angles; i++) {
        ies_out.candela[i] = 1000;
        ies_out.max_candela = std::max(ies_out.max_candela, ies_out.candela[i]);
    }
    for (uint32_t i = 0; i < ies_out.number_horizontal_angles; i++) {
        std::vector<double> h;
        for (uint32_t j = 0; j < ies_out.number_vertical_angles; j++)
        {
            h.push_back(ies_out.candela[i * ies_out.number_vertical_angles + j] * ies_out.multiplier);
        }
        ies_out.candela_hv[i] = h;
    }

    switch (ies_out.photometric_type)
    {
    case 1:
    {
        if (ies_out.horizontal_angles.size() > 0)
        {
            int maxAngle = ies_out.horizontal_angles[ies_out.horizontal_angles.size() - 1];
            switch (maxAngle)
            {
            case 0:
                ies_out.m_IESType = eC0;
                break;
            case 90:
                ies_out.m_IESType = eC90;
                break;
            case 180:
                ies_out.m_IESType = eC180;
                break;
            case 270:
                ies_out.m_IESType = eC270;
                break;
            case 360:
                ies_out.m_IESType = eC360;
                break;
            default:
                break;
            }

            int minV = ies_out.vertical_angles[/*ies_out.vertical_angles.size() - 1*/0];
            int maxV = ies_out.vertical_angles[ies_out.vertical_angles.size() - 1];
            switch (minV)
            {
            case 0:
            {
                switch (maxV)
                {
                case 90:
                    ies_out.m_vType = eC_V90;
                    break;
                case 180:
                    ies_out.m_vType = eC_V180;
                    break;
                default:
                    break;
                }
            }
            break;
            case 90:
            {
                switch (maxV)
                {
                case 180:
                    ies_out.m_vType = eC_V90_180;
                    break;
                default:
                    break;
                }
            }
            break;
            default:
                break;
            }
        }
    }
    break;
    case 2:
    {
        if (ies_out.horizontal_angles.size() > 0)
        {
            int maxAngle = ies_out.horizontal_angles[ies_out.horizontal_angles.size() - 1];
            int minAngle = ies_out.horizontal_angles[0];
            switch (minAngle)
            {
            case 0:
                ies_out.m_IESType = eB090;
                break;
            case -90:
                ies_out.m_IESType = eB_9090;
                break;
            default:
                break;
            }
        }

        if (ies_out.vertical_angles.size() > 0)
        {
            int minV = ies_out.vertical_angles[/*ies_out.vertical_angles.size() - 1*/0];
            switch (minV)
            {
            case 0:
            {
                ies_out.m_vType = eB_V90;
            }
            break;
            case -90:
            {
                ies_out.m_vType = eB_V_90_90;
            }
            break;
            default:
                break;
            }
        }

    }
    break;
    case 3:
    {
        if (ies_out.horizontal_angles.size() > 0)
        {
            int maxAngle = ies_out.horizontal_angles[ies_out.horizontal_angles.size() - 1];
            int minAngle = ies_out.horizontal_angles[0];
            switch (minAngle)
            {
            case 0:
                ies_out.m_IESType = eA090;
                break;
            case -90:
                ies_out.m_IESType = eA_9090;
                break;
            default:
                break;
            }
        }

        if (ies_out.vertical_angles.size() > 0)
        {
            int minV = ies_out.vertical_angles[/*ies_out.vertical_angles.size() - 1*/0];
            switch (minV)
            {
            case 0:
            {
                ies_out.m_vType = eA_V90;
            }
            break;
            case -90:
            {
                ies_out.m_vType = eA_V_90_90;
            }
            break;
            default:
                break;
            }
        }
    }
    break;
    default:
        break;
    }
}
void MainWindow::slotCreateTypeC_0_90(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eC90;
    light.m_vType = vtype;
    light.horizontal_angles.push_back(0);
    light.horizontal_angles.push_back(90);
    light.ies_version = version.toStdString();
    light.photometric_type = 1;
    switch (vtype)
    {
    case eC_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eC_V180:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(180);
    }
        break;
    case eC_V90_180:
    {
        light.vertical_angles.push_back(90);
        light.vertical_angles.push_back(180);
    }
        break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;

    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeC_0_180(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eC180;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 1;
    light.horizontal_angles.push_back(0);
    light.horizontal_angles.push_back(180);
    light.ies_version = version.toStdString();
    switch (vtype)
    {
    case eC_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eC_V180:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(180);
    }
    break;
    case eC_V90_180:
    {
        light.vertical_angles.push_back(90);
        light.vertical_angles.push_back(180);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeC_90_270(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eC270;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.horizontal_angles.push_back(90);
    light.horizontal_angles.push_back(270);
    light.ies_version = version.toStdString();
    light.photometric_type = 1;
    switch (vtype)
    {
    case eC_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eC_V180:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(180);
    }
    break;
    case eC_V90_180:
    {
        light.vertical_angles.push_back(90);
        light.vertical_angles.push_back(180);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeC_0_360(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eC360;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.horizontal_angles.push_back(0);
    light.horizontal_angles.push_back(360);
    light.ies_version = version.toStdString();
    light.photometric_type = 1;
    switch (vtype)
    {
    case eC_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eC_V180:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(180);
    }
    break;
    case eC_V90_180:
    {
        light.vertical_angles.push_back(90);
        light.vertical_angles.push_back(180);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeB_m90_p90(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eB_9090;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 2;
    light.horizontal_angles.push_back(-90);
    light.horizontal_angles.push_back(90);
    light.ies_version = version.toStdString();
    switch (vtype)
    {
    case eB_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eB_V_90_90:
    {
        light.vertical_angles.push_back(-90);
        light.vertical_angles.push_back(90);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    //IESLoader::instance().m_thetas_size = 180;
    //IESLoader::instance().m_phis_size = 180;
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeB_0_90(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eB090;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 2;
    light.horizontal_angles.push_back(0);
    light.horizontal_angles.push_back(90);
    light.ies_version = version.toStdString();
    switch (vtype)
    {
    case eB_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eB_V_90_90:
    {
        light.vertical_angles.push_back(-90);
        light.vertical_angles.push_back(90);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    //IESLoader::instance().m_thetas_size = 180;
    //IESLoader::instance().m_phis_size = 180;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeA_m90_p90(QString version, EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eA_9090;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 3;
    light.horizontal_angles.push_back(-90);
    light.horizontal_angles.push_back(90);
    light.ies_version = version.toStdString();
    switch (vtype)
    {
    case eA_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eA_V_90_90:
    {
        light.vertical_angles.push_back(-90);
        light.vertical_angles.push_back(90);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}
void MainWindow::slotCreateTypeA_0_90(QString version , EIES_VType vtype)
{
    tiny_ies<double>::light light;
    light.m_IESType = eA090;
    light.m_vType = vtype;
    light.ies_version = version.toStdString();
    light.photometric_type = 3;
    light.horizontal_angles.push_back(0);
    light.horizontal_angles.push_back(90);
    light.ies_version = version.toStdString();
    switch (vtype)
    {
    case eA_V90:
    {
        light.vertical_angles.push_back(0);
        light.vertical_angles.push_back(90);
    }
    break;
    case eA_V_90_90:
    {
        light.vertical_angles.push_back(-90);
        light.vertical_angles.push_back(90);
    }
    break;
    default:
        break;
    }
    light.number_horizontal_angles = 2;
    light.number_vertical_angles = 2;
    light.multiplier = 1;
    fillLight(light);
    light.width = 0;
    light.height = 0;
    light.length = 0;
    light.number_lights = 1;
    light.lumens_per_lamp = 5000;
    light.ballast_factor = 1;
    light.input_watts = 1000;
    light.future_use = 1;
    NewHeader(light);
    IESLoader::instance().light = light;
    IESLoader::instance().clear();
    IESLoader::instance().fillData();
    fillUI();
    populateTableFromIESData();

    m_filepath = "";
    m_fileState = eFile_clean;
    m_filename = "No Name";
    this->setWindowTitle("[" + m_filename + "]");
}