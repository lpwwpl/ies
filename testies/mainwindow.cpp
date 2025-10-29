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

#include "IESLoader.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    translator(nullptr)
{
    ui->setupUi(this);
    setupHeaderEditing();

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


    DoubleItemDelegate* delegate = new DoubleItemDelegate(ui->tableWidget);
    delegate->setRange(0.0, 1000000.0, 4); // 与ValidatedTableWidget中的设置一致
    ui->tableWidget->setItemDelegate(delegate); // 设置委托用于单元格
    bool value = connect(delegate, SIGNAL(editingFinished()), this, SLOT(slotDoubleValueChanged()));

    // 行操作
    value = connect(ui->actionAdd_Horizontal_Angle, &QAction::triggered, this, &MainWindow::addRow);
    connect(ui->actionDelete_Horizontal_Angle, &QAction::triggered, this, &MainWindow::deleteRow);

    // 列操作
    connect(ui->actionAdd_Vertical_Angle, &QAction::triggered, this, &MainWindow::addColumn);
    connect(ui->actionDelete_Vertical_Angle, &QAction::triggered, this, &MainWindow::deleteColumn);
   
    connect(ui->pbHplus, SIGNAL(clicked()), this, SLOT(deleteRow()));
    connect(ui->pbHadd, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(ui->pbVplus, SIGNAL(clicked()), this, SLOT(deleteColumn()));
    connect(ui->pbVadd, SIGNAL(clicked()), this, SLOT(addColumn()));

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
    resizeDocks(docks, size, Qt::Orientation::Vertical);

    // init qrc
    Q_INIT_RESOURCE(res);

    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(0,255,0,60); }");
    ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(0,0,255,60); }");

  
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
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::slot_heditor()
{    
    ui->tableWidget->setHorizontalHeaderItem(logicalIndex, new QTableWidgetItem(m_heditor->text()));
    //m_heditor->deleteLater();
}
void MainWindow::slot_veditor()
{
    ui->tableWidget->setVerticalHeaderItem(logicalIndex, new QTableWidgetItem(m_veditor->text()));
    //m_veditor->deleteLater();
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
            m_heditor->close();
            return true;
        }
    }
    else if (qobject_cast<QLineEdit*>(obj) && obj == m_veditor)
    {
        if (event->type() == QEvent::FocusOut) {
            // 失去焦点，直接关闭编辑器，不更新（视为取消）
            m_veditor->close();
            return true;
        }
    }
    if (obj == ui->tableWidget->horizontalHeader()->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            logicalIndex = ui->tableWidget->horizontalHeader()->logicalIndexAt(mouseEvent->pos());
            if (logicalIndex != -1) {   
                m_heditor->setGeometry(ui->tableWidget->horizontalHeader()->sectionViewportPosition(logicalIndex), 0,
                    ui->tableWidget->horizontalHeader()->sectionSize(logicalIndex), ui->tableWidget->horizontalHeader()->height());
                m_heditor->setText(ui->tableWidget->horizontalHeaderItem(logicalIndex)->text());
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

    //IESLoader::instance().light.candela_hv[col][row] = value;
}
void MainWindow::addRow()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        currentRow = ui->tableWidget->rowCount();
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

}

void MainWindow::deleteRow()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        QMessageBox::warning(this, "Delete Row", "Please select a row to delete.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Row", "Are you sure you want to delete the selected row?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ui->tableWidget->removeRow(currentRow);
    }

}

void MainWindow::addColumn()
{
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn == -1) {
        currentColumn = ui->tableWidget->columnCount();
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
}

void MainWindow::deleteColumn()
{
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn == -1) {
        QMessageBox::warning(this, "Delete Column", "Please select a column to delete.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Column", "Are you sure you want to delete the selected column?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ui->tableWidget->removeColumn(currentColumn);
    }
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
    ui->tableWidget->clear();



    addColumn();
    addColumn();
    addRow();
    addRow();
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
void MainWindow::on_actionOpen_triggered()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"),
        "./", tr("IES files(*.ies);;All files(*.*)"));
    QFile file(filename);
    if (!file.exists())
        return;
    IESLoader::instance().loadIES(filename);

    m_filePath = filename;

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

    populateTableFromIESData();
}
void MainWindow::on_actionSave_triggered()
{
    tiny_ies<double>::light light =
        IESLoader::instance().light;
    // static bool write_ies(const std::string& filename, const light& ies, const uint32_t precision = std::numeric_limits<T>::max_digits10) 
   

    QFile file(m_filePath);
    if (file.exists())
    {
        tiny_ies<double>::write_ies(m_filePath.toStdString(), light);
    }
    else
    {
        on_actionSave_As_triggered();
    }
   
}

void MainWindow::on_actionSave_As_triggered()
{
    auto filepath = QFileDialog::getSaveFileName(
        this, tr("Save IES"), "", tr("Ies File(*.ies);;All Files(*)"));
    if (filepath.isEmpty()) {
        return;
    }
    tiny_ies<double>::light light =
        IESLoader::instance().light;
    tiny_ies<double>::write_ies(filepath.toStdString(), light);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}




