/********************************************************************************
** Form generated from reading UI file 'isoluxdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ISOLUXDIALOG_H
#define UI_ISOLUXDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ISOLuxDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QStackedWidget *isoLuxwidget;
    QWidget *iso2d;
    QWidget *iso3d;
    QWidget *configWidget;
    QGridLayout *gridLayout_5;
    QGroupBox *groupUnit;
    QGridLayout *gridLayout_4;
    QRadioButton *rbUnit0;
    QRadioButton *rbUnit1;
    QGroupBox *groupDisplay;
    QGridLayout *gridLayout_3;
    QCheckBox *chkGrid;
    QRadioButton *rb2D;
    QRadioButton *rb3D;
    QComboBox *cmbPlane;
    QGroupBox *groupLux;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QSpinBox *spinDistance;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *spinHalfMapWidth;
    QLabel *label_6;
    QLabel *label_4;
    QSpinBox *spinLevels;
    QLabel *label_5;
    QSpinBox *spinNbOfPoints;

    void setupUi(QDialog *ISOLuxDialog)
    {
        if (ISOLuxDialog->objectName().isEmpty())
            ISOLuxDialog->setObjectName(QString::fromUtf8("ISOLuxDialog"));
        ISOLuxDialog->resize(754, 488);
        ISOLuxDialog->setMinimumSize(QSize(754, 488));
        ISOLuxDialog->setLayoutDirection(Qt::LeftToRight);
        horizontalLayout = new QHBoxLayout(ISOLuxDialog);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        isoLuxwidget = new QStackedWidget(ISOLuxDialog);
        isoLuxwidget->setObjectName(QString::fromUtf8("isoLuxwidget"));
        iso2d = new QWidget();
        iso2d->setObjectName(QString::fromUtf8("iso2d"));
        isoLuxwidget->addWidget(iso2d);
        iso3d = new QWidget();
        iso3d->setObjectName(QString::fromUtf8("iso3d"));
        isoLuxwidget->addWidget(iso3d);

        horizontalLayout->addWidget(isoLuxwidget);

        configWidget = new QWidget(ISOLuxDialog);
        configWidget->setObjectName(QString::fromUtf8("configWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(configWidget->sizePolicy().hasHeightForWidth());
        configWidget->setSizePolicy(sizePolicy);
        configWidget->setMaximumSize(QSize(200, 460));
        gridLayout_5 = new QGridLayout(configWidget);
        gridLayout_5->setSpacing(0);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        groupUnit = new QGroupBox(configWidget);
        groupUnit->setObjectName(QString::fromUtf8("groupUnit"));
        sizePolicy.setHeightForWidth(groupUnit->sizePolicy().hasHeightForWidth());
        groupUnit->setSizePolicy(sizePolicy);
        groupUnit->setMaximumSize(QSize(300, 16777215));
        gridLayout_4 = new QGridLayout(groupUnit);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        rbUnit0 = new QRadioButton(groupUnit);
        rbUnit0->setObjectName(QString::fromUtf8("rbUnit0"));

        gridLayout_4->addWidget(rbUnit0, 0, 0, 1, 1);

        rbUnit1 = new QRadioButton(groupUnit);
        rbUnit1->setObjectName(QString::fromUtf8("rbUnit1"));

        gridLayout_4->addWidget(rbUnit1, 1, 0, 1, 1);


        gridLayout_5->addWidget(groupUnit, 1, 0, 1, 1);

        groupDisplay = new QGroupBox(configWidget);
        groupDisplay->setObjectName(QString::fromUtf8("groupDisplay"));
        sizePolicy.setHeightForWidth(groupDisplay->sizePolicy().hasHeightForWidth());
        groupDisplay->setSizePolicy(sizePolicy);
        groupDisplay->setMaximumSize(QSize(300, 16777215));
        gridLayout_3 = new QGridLayout(groupDisplay);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        chkGrid = new QCheckBox(groupDisplay);
        chkGrid->setObjectName(QString::fromUtf8("chkGrid"));

        gridLayout_3->addWidget(chkGrid, 0, 0, 1, 1);

        rb2D = new QRadioButton(groupDisplay);
        rb2D->setObjectName(QString::fromUtf8("rb2D"));

        gridLayout_3->addWidget(rb2D, 1, 0, 1, 1);

        rb3D = new QRadioButton(groupDisplay);
        rb3D->setObjectName(QString::fromUtf8("rb3D"));

        gridLayout_3->addWidget(rb3D, 2, 0, 1, 1);


        gridLayout_5->addWidget(groupDisplay, 2, 0, 1, 1);

        cmbPlane = new QComboBox(configWidget);
        cmbPlane->addItem(QString());
        cmbPlane->addItem(QString());
        cmbPlane->addItem(QString());
        cmbPlane->setObjectName(QString::fromUtf8("cmbPlane"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(cmbPlane->sizePolicy().hasHeightForWidth());
        cmbPlane->setSizePolicy(sizePolicy1);
        cmbPlane->setMaximumSize(QSize(180, 16777215));

        gridLayout_5->addWidget(cmbPlane, 3, 0, 1, 1);

        groupLux = new QGroupBox(configWidget);
        groupLux->setObjectName(QString::fromUtf8("groupLux"));
        sizePolicy.setHeightForWidth(groupLux->sizePolicy().hasHeightForWidth());
        groupLux->setSizePolicy(sizePolicy);
        groupLux->setMaximumSize(QSize(300, 16777215));
        gridLayout_2 = new QGridLayout(groupLux);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(groupLux);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(100, 16777215));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        spinDistance = new QSpinBox(groupLux);
        spinDistance->setObjectName(QString::fromUtf8("spinDistance"));
        spinDistance->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(spinDistance, 0, 1, 1, 1);

        label_2 = new QLabel(groupLux);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setMinimumSize(QSize(30, 0));
        label_2->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(label_2, 0, 2, 1, 1);

        label_3 = new QLabel(groupLux);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMaximumSize(QSize(150, 16777215));

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        spinHalfMapWidth = new QSpinBox(groupLux);
        spinHalfMapWidth->setObjectName(QString::fromUtf8("spinHalfMapWidth"));
        spinHalfMapWidth->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(spinHalfMapWidth, 1, 1, 1, 1);

        label_6 = new QLabel(groupLux);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);
        label_6->setMinimumSize(QSize(30, 0));
        label_6->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(label_6, 1, 2, 1, 1);

        label_4 = new QLabel(groupLux);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        spinLevels = new QSpinBox(groupLux);
        spinLevels->setObjectName(QString::fromUtf8("spinLevels"));
        spinLevels->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(spinLevels, 2, 1, 1, 1);

        label_5 = new QLabel(groupLux);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 3, 0, 1, 1);

        spinNbOfPoints = new QSpinBox(groupLux);
        spinNbOfPoints->setObjectName(QString::fromUtf8("spinNbOfPoints"));

        gridLayout_2->addWidget(spinNbOfPoints, 3, 1, 1, 1);


        gridLayout_5->addWidget(groupLux, 0, 0, 1, 1);


        horizontalLayout->addWidget(configWidget);


        retranslateUi(ISOLuxDialog);

        QMetaObject::connectSlotsByName(ISOLuxDialog);
    } // setupUi

    void retranslateUi(QDialog *ISOLuxDialog)
    {
        ISOLuxDialog->setWindowTitle(QCoreApplication::translate("ISOLuxDialog", "Dialog", nullptr));
        groupUnit->setTitle(QCoreApplication::translate("ISOLuxDialog", "Uint", nullptr));
        rbUnit0->setText(QCoreApplication::translate("ISOLuxDialog", "International", nullptr));
        rbUnit1->setText(QCoreApplication::translate("ISOLuxDialog", "US", nullptr));
        groupDisplay->setTitle(QCoreApplication::translate("ISOLuxDialog", "Display", nullptr));
        chkGrid->setText(QCoreApplication::translate("ISOLuxDialog", "Grid", nullptr));
        rb2D->setText(QCoreApplication::translate("ISOLuxDialog", "2 D", nullptr));
        rb3D->setText(QCoreApplication::translate("ISOLuxDialog", "3 D", nullptr));
        cmbPlane->setItemText(0, QCoreApplication::translate("ISOLuxDialog", "Map along: X", nullptr));
        cmbPlane->setItemText(1, QCoreApplication::translate("ISOLuxDialog", "Map along: Y", nullptr));
        cmbPlane->setItemText(2, QCoreApplication::translate("ISOLuxDialog", "Map along: Z", nullptr));

        groupLux->setTitle(QCoreApplication::translate("ISOLuxDialog", "lluminance Unit:Lux", nullptr));
        label->setText(QCoreApplication::translate("ISOLuxDialog", "Distance:", nullptr));
        label_2->setText(QCoreApplication::translate("ISOLuxDialog", "m", nullptr));
        label_3->setText(QCoreApplication::translate("ISOLuxDialog", "Half map width:", nullptr));
        label_6->setText(QCoreApplication::translate("ISOLuxDialog", "m", nullptr));
        label_4->setText(QCoreApplication::translate("ISOLuxDialog", "Levels:", nullptr));
        label_5->setText(QCoreApplication::translate("ISOLuxDialog", "Nb of points:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ISOLuxDialog: public Ui_ISOLuxDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ISOLUXDIALOG_H
