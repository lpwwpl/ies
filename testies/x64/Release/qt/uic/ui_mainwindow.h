/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew;
    QAction *actionIsoLuxCurves;
    QAction *actionPolarCurves;
    QAction *action3DCurves;
    QAction *actionOpen;
    QAction *actionLight;
    QAction *actionDark;
    QAction *actionSave;
    QAction *actionSave_As;
    QAction *actionExit;
    QAction *actionAdd_Vertical_Angle;
    QAction *actionAdd_Horizontal_Angle;
    QAction *actionDelete_Vertical_Angle;
    QAction *actionDelete_Horizontal_Angle;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_7;
    QTabWidget *tabWidget;
    QWidget *tabGeneral;
    QHBoxLayout *horizontalLayout;
    QTextEdit *textEdit;
    QWidget *tabMisc;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *leNumberOf;
    QLabel *label_2;
    QLineEdit *leWatts;
    QLabel *label_3;
    QLineEdit *leLumenPer;
    QLabel *label_4;
    QLineEdit *leBallast;
    QLabel *label_5;
    QLineEdit *leCandela;
    QLabel *label_6;
    QLineEdit *leFutur;
    QGroupBox *groupLuminous;
    QGridLayout *gridLayout_2;
    QLabel *lblWidth;
    QLineEdit *leWidth;
    QLabel *lblWidthUnit;
    QLabel *lblLen;
    QLineEdit *leLen;
    QLabel *lblLenUnit;
    QLabel *lblHeight;
    QLineEdit *leHeight;
    QLabel *lblHeightUnit;
    QGroupBox *groupPhotometric;
    QGridLayout *gridLayout_4;
    QLabel *lblTypeA;
    QLabel *lblTypeB;
    QLabel *lblTypeC;
    QGroupBox *groupUnits;
    QGridLayout *gridLayout_3;
    QRadioButton *rbFeet;
    QRadioButton *rbMeters;
    QLabel *lblImage;
    QWidget *tabSymmetry;
    QGroupBox *groupHorizontal;
    QGridLayout *gridLayout_5;
    QRadioButton *rb0;
    QRadioButton *rb1;
    QRadioButton *rb2;
    QRadioButton *rb3;
    QRadioButton *rb4;
    QGroupBox *groupVertical;
    QGridLayout *gridLayout_6;
    QRadioButton *rbV0;
    QRadioButton *rbV1;
    QRadioButton *rbV2;
    QWidget *tabData;
    QTableWidget *tableWidget;
    QPushButton *pbHplus;
    QPushButton *pbHadd;
    QPushButton *pbVadd;
    QPushButton *pbVplus;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuOptions;
    QMenu *menuTheme;
    QMenu *menuView;
    QMenu *menuEdit;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QToolBar *viewBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(967, 721);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionNew->setCheckable(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon);
        actionIsoLuxCurves = new QAction(MainWindow);
        actionIsoLuxCurves->setObjectName(QString::fromUtf8("actionIsoLuxCurves"));
        actionPolarCurves = new QAction(MainWindow);
        actionPolarCurves->setObjectName(QString::fromUtf8("actionPolarCurves"));
        action3DCurves = new QAction(MainWindow);
        action3DCurves->setObjectName(QString::fromUtf8("action3DCurves"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionLight = new QAction(MainWindow);
        actionLight->setObjectName(QString::fromUtf8("actionLight"));
        actionDark = new QAction(MainWindow);
        actionDark->setObjectName(QString::fromUtf8("actionDark"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName(QString::fromUtf8("actionSave_As"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAdd_Vertical_Angle = new QAction(MainWindow);
        actionAdd_Vertical_Angle->setObjectName(QString::fromUtf8("actionAdd_Vertical_Angle"));
        actionAdd_Horizontal_Angle = new QAction(MainWindow);
        actionAdd_Horizontal_Angle->setObjectName(QString::fromUtf8("actionAdd_Horizontal_Angle"));
        actionDelete_Vertical_Angle = new QAction(MainWindow);
        actionDelete_Vertical_Angle->setObjectName(QString::fromUtf8("actionDelete_Vertical_Angle"));
        actionDelete_Horizontal_Angle = new QAction(MainWindow);
        actionDelete_Horizontal_Angle->setObjectName(QString::fromUtf8("actionDelete_Horizontal_Angle"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_7 = new QVBoxLayout(centralwidget);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabGeneral = new QWidget();
        tabGeneral->setObjectName(QString::fromUtf8("tabGeneral"));
        horizontalLayout = new QHBoxLayout(tabGeneral);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        textEdit = new QTextEdit(tabGeneral);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        horizontalLayout->addWidget(textEdit);

        tabWidget->addTab(tabGeneral, QString());
        tabMisc = new QWidget();
        tabMisc->setObjectName(QString::fromUtf8("tabMisc"));
        groupBox = new QGroupBox(tabMisc);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(30, 160, 651, 141));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        leNumberOf = new QLineEdit(groupBox);
        leNumberOf->setObjectName(QString::fromUtf8("leNumberOf"));

        gridLayout->addWidget(leNumberOf, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        leWatts = new QLineEdit(groupBox);
        leWatts->setObjectName(QString::fromUtf8("leWatts"));

        gridLayout->addWidget(leWatts, 0, 3, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        leLumenPer = new QLineEdit(groupBox);
        leLumenPer->setObjectName(QString::fromUtf8("leLumenPer"));

        gridLayout->addWidget(leLumenPer, 1, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 2, 1, 1);

        leBallast = new QLineEdit(groupBox);
        leBallast->setObjectName(QString::fromUtf8("leBallast"));

        gridLayout->addWidget(leBallast, 1, 3, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        leCandela = new QLineEdit(groupBox);
        leCandela->setObjectName(QString::fromUtf8("leCandela"));

        gridLayout->addWidget(leCandela, 2, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 2, 2, 1, 1);

        leFutur = new QLineEdit(groupBox);
        leFutur->setObjectName(QString::fromUtf8("leFutur"));

        gridLayout->addWidget(leFutur, 2, 3, 1, 1);

        groupLuminous = new QGroupBox(tabMisc);
        groupLuminous->setObjectName(QString::fromUtf8("groupLuminous"));
        groupLuminous->setGeometry(QRect(20, 10, 296, 132));
        gridLayout_2 = new QGridLayout(groupLuminous);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lblWidth = new QLabel(groupLuminous);
        lblWidth->setObjectName(QString::fromUtf8("lblWidth"));

        gridLayout_2->addWidget(lblWidth, 0, 0, 1, 1);

        leWidth = new QLineEdit(groupLuminous);
        leWidth->setObjectName(QString::fromUtf8("leWidth"));

        gridLayout_2->addWidget(leWidth, 0, 1, 1, 1);

        lblWidthUnit = new QLabel(groupLuminous);
        lblWidthUnit->setObjectName(QString::fromUtf8("lblWidthUnit"));

        gridLayout_2->addWidget(lblWidthUnit, 0, 2, 1, 1);

        lblLen = new QLabel(groupLuminous);
        lblLen->setObjectName(QString::fromUtf8("lblLen"));

        gridLayout_2->addWidget(lblLen, 1, 0, 1, 1);

        leLen = new QLineEdit(groupLuminous);
        leLen->setObjectName(QString::fromUtf8("leLen"));

        gridLayout_2->addWidget(leLen, 1, 1, 1, 1);

        lblLenUnit = new QLabel(groupLuminous);
        lblLenUnit->setObjectName(QString::fromUtf8("lblLenUnit"));

        gridLayout_2->addWidget(lblLenUnit, 1, 2, 1, 1);

        lblHeight = new QLabel(groupLuminous);
        lblHeight->setObjectName(QString::fromUtf8("lblHeight"));

        gridLayout_2->addWidget(lblHeight, 2, 0, 1, 1);

        leHeight = new QLineEdit(groupLuminous);
        leHeight->setObjectName(QString::fromUtf8("leHeight"));

        gridLayout_2->addWidget(leHeight, 2, 1, 1, 1);

        lblHeightUnit = new QLabel(groupLuminous);
        lblHeightUnit->setObjectName(QString::fromUtf8("lblHeightUnit"));

        gridLayout_2->addWidget(lblHeightUnit, 2, 2, 1, 1);

        groupPhotometric = new QGroupBox(tabMisc);
        groupPhotometric->setObjectName(QString::fromUtf8("groupPhotometric"));
        groupPhotometric->setGeometry(QRect(20, 310, 901, 251));
        gridLayout_4 = new QGridLayout(groupPhotometric);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        lblTypeA = new QLabel(groupPhotometric);
        lblTypeA->setObjectName(QString::fromUtf8("lblTypeA"));

        gridLayout_4->addWidget(lblTypeA, 0, 0, 1, 1);

        lblTypeB = new QLabel(groupPhotometric);
        lblTypeB->setObjectName(QString::fromUtf8("lblTypeB"));

        gridLayout_4->addWidget(lblTypeB, 0, 1, 1, 1);

        lblTypeC = new QLabel(groupPhotometric);
        lblTypeC->setObjectName(QString::fromUtf8("lblTypeC"));

        gridLayout_4->addWidget(lblTypeC, 0, 2, 1, 1);

        groupUnits = new QGroupBox(tabMisc);
        groupUnits->setObjectName(QString::fromUtf8("groupUnits"));
        groupUnits->setGeometry(QRect(320, 10, 161, 141));
        gridLayout_3 = new QGridLayout(groupUnits);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        rbFeet = new QRadioButton(groupUnits);
        rbFeet->setObjectName(QString::fromUtf8("rbFeet"));

        gridLayout_3->addWidget(rbFeet, 0, 0, 1, 1);

        rbMeters = new QRadioButton(groupUnits);
        rbMeters->setObjectName(QString::fromUtf8("rbMeters"));

        gridLayout_3->addWidget(rbMeters, 1, 0, 1, 1);

        lblImage = new QLabel(tabMisc);
        lblImage->setObjectName(QString::fromUtf8("lblImage"));
        lblImage->setGeometry(QRect(510, 10, 221, 153));
        tabWidget->addTab(tabMisc, QString());
        tabSymmetry = new QWidget();
        tabSymmetry->setObjectName(QString::fromUtf8("tabSymmetry"));
        groupHorizontal = new QGroupBox(tabSymmetry);
        groupHorizontal->setObjectName(QString::fromUtf8("groupHorizontal"));
        groupHorizontal->setGeometry(QRect(40, 40, 311, 441));
        gridLayout_5 = new QGridLayout(groupHorizontal);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        rb0 = new QRadioButton(groupHorizontal);
        rb0->setObjectName(QString::fromUtf8("rb0"));

        gridLayout_5->addWidget(rb0, 0, 0, 1, 1);

        rb1 = new QRadioButton(groupHorizontal);
        rb1->setObjectName(QString::fromUtf8("rb1"));

        gridLayout_5->addWidget(rb1, 1, 0, 1, 1);

        rb2 = new QRadioButton(groupHorizontal);
        rb2->setObjectName(QString::fromUtf8("rb2"));

        gridLayout_5->addWidget(rb2, 2, 0, 1, 1);

        rb3 = new QRadioButton(groupHorizontal);
        rb3->setObjectName(QString::fromUtf8("rb3"));

        gridLayout_5->addWidget(rb3, 3, 0, 1, 1);

        rb4 = new QRadioButton(groupHorizontal);
        rb4->setObjectName(QString::fromUtf8("rb4"));

        gridLayout_5->addWidget(rb4, 4, 0, 1, 1);

        groupVertical = new QGroupBox(tabSymmetry);
        groupVertical->setObjectName(QString::fromUtf8("groupVertical"));
        groupVertical->setGeometry(QRect(460, 50, 251, 411));
        gridLayout_6 = new QGridLayout(groupVertical);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        rbV0 = new QRadioButton(groupVertical);
        rbV0->setObjectName(QString::fromUtf8("rbV0"));

        gridLayout_6->addWidget(rbV0, 0, 0, 1, 1);

        rbV1 = new QRadioButton(groupVertical);
        rbV1->setObjectName(QString::fromUtf8("rbV1"));

        gridLayout_6->addWidget(rbV1, 1, 0, 1, 1);

        rbV2 = new QRadioButton(groupVertical);
        rbV2->setObjectName(QString::fromUtf8("rbV2"));

        gridLayout_6->addWidget(rbV2, 2, 0, 1, 1);

        tabWidget->addTab(tabSymmetry, QString());
        tabData = new QWidget();
        tabData->setObjectName(QString::fromUtf8("tabData"));
        tableWidget = new QTableWidget(tabData);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 60, 851, 511));
        pbHplus = new QPushButton(tabData);
        pbHplus->setObjectName(QString::fromUtf8("pbHplus"));
        pbHplus->setGeometry(QRect(710, 10, 38, 38));
        pbHadd = new QPushButton(tabData);
        pbHadd->setObjectName(QString::fromUtf8("pbHadd"));
        pbHadd->setGeometry(QRect(760, 10, 38, 38));
        pbVadd = new QPushButton(tabData);
        pbVadd->setObjectName(QString::fromUtf8("pbVadd"));
        pbVadd->setGeometry(QRect(870, 80, 38, 38));
        pbVplus = new QPushButton(tabData);
        pbVplus->setObjectName(QString::fromUtf8("pbVplus"));
        pbVplus->setGeometry(QRect(870, 130, 38, 38));
        tabWidget->addTab(tabData, QString());

        verticalLayout_7->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 967, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuOptions = new QMenu(menubar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menuTheme = new QMenu(menuOptions);
        menuTheme->setObjectName(QString::fromUtf8("menuTheme"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        viewBar = new QToolBar(MainWindow);
        viewBar->setObjectName(QString::fromUtf8("viewBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, viewBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuOptions->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuFile->addAction(actionExit);
        menuOptions->addAction(menuTheme->menuAction());
        menuTheme->addAction(actionLight);
        menuTheme->addAction(actionDark);
        menuView->addAction(actionIsoLuxCurves);
        menuView->addAction(actionPolarCurves);
        menuView->addAction(action3DCurves);
        menuEdit->addAction(actionAdd_Vertical_Angle);
        menuEdit->addAction(actionAdd_Horizontal_Angle);
        menuEdit->addAction(actionDelete_Vertical_Angle);
        menuEdit->addAction(actionDelete_Horizontal_Angle);
        toolBar->addAction(actionNew);
        toolBar->addSeparator();
        toolBar->addAction(actionOpen);
        toolBar->addSeparator();
        toolBar->addAction(actionSave);
        toolBar->addSeparator();
        viewBar->addAction(actionIsoLuxCurves);
        viewBar->addSeparator();
        viewBar->addAction(actionPolarCurves);
        viewBar->addSeparator();
        viewBar->addAction(action3DCurves);
        viewBar->addSeparator();

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "IES", nullptr));
        actionNew->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
#if QT_CONFIG(shortcut)
        actionNew->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionIsoLuxCurves->setText(QCoreApplication::translate("MainWindow", "IsoLux Curve", nullptr));
#if QT_CONFIG(tooltip)
        actionIsoLuxCurves->setToolTip(QCoreApplication::translate("MainWindow", "IsoLuxCurves", nullptr));
#endif // QT_CONFIG(tooltip)
        actionPolarCurves->setText(QCoreApplication::translate("MainWindow", "Polar Curves", nullptr));
#if QT_CONFIG(tooltip)
        actionPolarCurves->setToolTip(QCoreApplication::translate("MainWindow", "PolarCurves", nullptr));
#endif // QT_CONFIG(tooltip)
        action3DCurves->setText(QCoreApplication::translate("MainWindow", "3D Curve", nullptr));
#if QT_CONFIG(tooltip)
        action3DCurves->setToolTip(QCoreApplication::translate("MainWindow", "3DCurves", nullptr));
#endif // QT_CONFIG(tooltip)
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionLight->setText(QCoreApplication::translate("MainWindow", "Light", nullptr));
        actionDark->setText(QCoreApplication::translate("MainWindow", "Dark", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
#if QT_CONFIG(shortcut)
        actionSave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_As->setText(QCoreApplication::translate("MainWindow", "Save As", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAdd_Vertical_Angle->setText(QCoreApplication::translate("MainWindow", "Add Vertical Angle", nullptr));
        actionAdd_Horizontal_Angle->setText(QCoreApplication::translate("MainWindow", "Add Horizontal Angle", nullptr));
        actionDelete_Vertical_Angle->setText(QCoreApplication::translate("MainWindow", "Delete Vertical Angle", nullptr));
        actionDelete_Horizontal_Angle->setText(QCoreApplication::translate("MainWindow", "Delete Horizontal Angle", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabGeneral), QCoreApplication::translate("MainWindow", "General", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Info", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Number of", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Input Watts", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Lumen Per", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Ballast", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Candela", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Futur Use/ Ballast lamp photometic", nullptr));
        groupLuminous->setTitle(QCoreApplication::translate("MainWindow", "Luminous dimensions", nullptr));
        lblWidth->setText(QCoreApplication::translate("MainWindow", "Width:", nullptr));
        lblWidthUnit->setText(QCoreApplication::translate("MainWindow", "Feet", nullptr));
        lblLen->setText(QCoreApplication::translate("MainWindow", "Length:", nullptr));
        lblLenUnit->setText(QCoreApplication::translate("MainWindow", "Feet", nullptr));
        lblHeight->setText(QCoreApplication::translate("MainWindow", "Height:", nullptr));
        lblHeightUnit->setText(QCoreApplication::translate("MainWindow", "Feet", nullptr));
        groupPhotometric->setTitle(QCoreApplication::translate("MainWindow", "Photometric Type", nullptr));
        lblTypeA->setText(QString());
        lblTypeB->setText(QString());
        lblTypeC->setText(QString());
        groupUnits->setTitle(QCoreApplication::translate("MainWindow", "Units", nullptr));
        rbFeet->setText(QCoreApplication::translate("MainWindow", "Feet", nullptr));
        rbMeters->setText(QCoreApplication::translate("MainWindow", "Meters", nullptr));
        lblImage->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabMisc), QCoreApplication::translate("MainWindow", "Misc", nullptr));
        groupHorizontal->setTitle(QCoreApplication::translate("MainWindow", "Horizontal symmetry", nullptr));
        rb0->setText(QCoreApplication::translate("MainWindow", "Laterally symmetric in all \n"
"photometric planes.", nullptr));
        rb1->setText(QCoreApplication::translate("MainWindow", "Symetric in each quadrant.", nullptr));
        rb2->setText(QCoreApplication::translate("MainWindow", "Bilaterally symmetric about the \n"
"0-180 deg photometric plane.", nullptr));
        rb3->setText(QCoreApplication::translate("MainWindow", "Bilaterally symmetric about the \n"
"90-270 deg photometric plane.", nullptr));
        rb4->setText(QCoreApplication::translate("MainWindow", "No symmetry.", nullptr));
        groupVertical->setTitle(QCoreApplication::translate("MainWindow", "Vertical angles", nullptr));
        rbV0->setText(QCoreApplication::translate("MainWindow", "The first angle is 0 deg \n"
" and the last is 90 deg.", nullptr));
        rbV1->setText(QCoreApplication::translate("MainWindow", "The first angle is 0 deg \n"
" and the last is 180 deg.", nullptr));
        rbV2->setText(QCoreApplication::translate("MainWindow", "The first angle is 90 deg \n"
" and the last is 180 deg.", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSymmetry), QCoreApplication::translate("MainWindow", "Symmetry", nullptr));
        pbHplus->setText(QString());
        pbHadd->setText(QString());
        pbVadd->setText(QString());
        pbVplus->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabData), QCoreApplication::translate("MainWindow", "Data", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuOptions->setTitle(QCoreApplication::translate("MainWindow", "Options", nullptr));
        menuTheme->setTitle(QCoreApplication::translate("MainWindow", "Theme", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        viewBar->setWindowTitle(QCoreApplication::translate("MainWindow", "viewBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
