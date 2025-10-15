/********************************************************************************
** Form generated from reading UI file 'polardialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLARDIALOG_H
#define UI_POLARDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PolarDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widgetPolar;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *chkFillBlue;
    QCheckBox *chkFillRed;
    QCheckBox *chkViewRed;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QCheckBox *chkFillGreen;
    QCheckBox *chkFillYellow;
    QCheckBox *chkViewYellow;
    QComboBox *comboBox;

    void setupUi(QDialog *PolarDialog)
    {
        if (PolarDialog->objectName().isEmpty())
            PolarDialog->setObjectName(QString::fromUtf8("PolarDialog"));
        PolarDialog->resize(711, 500);
        PolarDialog->setMinimumSize(QSize(711, 500));
        horizontalLayout = new QHBoxLayout(PolarDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widgetPolar = new QWidget(PolarDialog);
        widgetPolar->setObjectName(QString::fromUtf8("widgetPolar"));

        horizontalLayout->addWidget(widgetPolar);

        widget = new QWidget(PolarDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        chkFillBlue = new QCheckBox(groupBox);
        chkFillBlue->setObjectName(QString::fromUtf8("chkFillBlue"));

        gridLayout_3->addWidget(chkFillBlue, 0, 0, 1, 1);

        chkFillRed = new QCheckBox(groupBox);
        chkFillRed->setObjectName(QString::fromUtf8("chkFillRed"));

        gridLayout_3->addWidget(chkFillRed, 1, 0, 1, 1);

        chkViewRed = new QCheckBox(groupBox);
        chkViewRed->setObjectName(QString::fromUtf8("chkViewRed"));

        gridLayout_3->addWidget(chkViewRed, 2, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(widget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        chkFillGreen = new QCheckBox(groupBox_2);
        chkFillGreen->setObjectName(QString::fromUtf8("chkFillGreen"));

        gridLayout->addWidget(chkFillGreen, 0, 0, 1, 1);

        chkFillYellow = new QCheckBox(groupBox_2);
        chkFillYellow->setObjectName(QString::fromUtf8("chkFillYellow"));

        gridLayout->addWidget(chkFillYellow, 1, 0, 1, 1);

        chkViewYellow = new QCheckBox(groupBox_2);
        chkViewYellow->setObjectName(QString::fromUtf8("chkViewYellow"));

        gridLayout->addWidget(chkViewYellow, 2, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 1);

        comboBox = new QComboBox(widget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout_2->addWidget(comboBox, 2, 0, 1, 1);


        horizontalLayout->addWidget(widget);


        retranslateUi(PolarDialog);

        QMetaObject::connectSlotsByName(PolarDialog);
    } // setupUi

    void retranslateUi(QDialog *PolarDialog)
    {
        PolarDialog->setWindowTitle(QCoreApplication::translate("PolarDialog", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("PolarDialog", "Series 1", nullptr));
        chkFillBlue->setText(QCoreApplication::translate("PolarDialog", "Fill Blue Curve", nullptr));
        chkFillRed->setText(QCoreApplication::translate("PolarDialog", "Fill Red Curve", nullptr));
        chkViewRed->setText(QCoreApplication::translate("PolarDialog", "View Red Curve", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("PolarDialog", "Series 2", nullptr));
        chkFillGreen->setText(QCoreApplication::translate("PolarDialog", "Fill Green Curve", nullptr));
        chkFillYellow->setText(QCoreApplication::translate("PolarDialog", "Fill Yellow Curve", nullptr));
        chkViewYellow->setText(QCoreApplication::translate("PolarDialog", "View Yellow Curve", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("PolarDialog", "Polar", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("PolarDialog", "Cartesian", nullptr));

    } // retranslateUi

};

namespace Ui {
    class PolarDialog: public Ui_PolarDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLARDIALOG_H
