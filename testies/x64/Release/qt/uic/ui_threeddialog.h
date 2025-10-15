/********************************************************************************
** Form generated from reading UI file 'threeddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THREEDDIALOG_H
#define UI_THREEDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ThreeDDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widgetThreeD;
    QGroupBox *groupDisplay;
    QCheckBox *chkOXY;
    QCheckBox *chkOYZ;
    QCheckBox *chkAxis;
    QCheckBox *chkOXZ;
    QGroupBox *grpView;
    QVBoxLayout *verticalLayout;
    QRadioButton *rbViewShading;
    QRadioButton *rbViewMesh;
    QGroupBox *grpType;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *rbShape;
    QRadioButton *rbColor;

    void setupUi(QDialog *ThreeDDialog)
    {
        if (ThreeDDialog->objectName().isEmpty())
            ThreeDDialog->setObjectName(QString::fromUtf8("ThreeDDialog"));
        ThreeDDialog->resize(697, 540);
        horizontalLayout = new QHBoxLayout(ThreeDDialog);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widgetThreeD = new QWidget(ThreeDDialog);
        widgetThreeD->setObjectName(QString::fromUtf8("widgetThreeD"));

        horizontalLayout->addWidget(widgetThreeD);

        groupDisplay = new QGroupBox(ThreeDDialog);
        groupDisplay->setObjectName(QString::fromUtf8("groupDisplay"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupDisplay->sizePolicy().hasHeightForWidth());
        groupDisplay->setSizePolicy(sizePolicy);
        groupDisplay->setMaximumSize(QSize(150, 300));
        groupDisplay->setLayoutDirection(Qt::LeftToRight);
        groupDisplay->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        chkOXY = new QCheckBox(groupDisplay);
        chkOXY->setObjectName(QString::fromUtf8("chkOXY"));
        chkOXY->setGeometry(QRect(1, 139, 91, 19));
        chkOYZ = new QCheckBox(groupDisplay);
        chkOYZ->setObjectName(QString::fromUtf8("chkOYZ"));
        chkOYZ->setGeometry(QRect(1, 177, 91, 19));
        chkAxis = new QCheckBox(groupDisplay);
        chkAxis->setObjectName(QString::fromUtf8("chkAxis"));
        chkAxis->setGeometry(QRect(1, 120, 59, 19));
        chkOXZ = new QCheckBox(groupDisplay);
        chkOXZ->setObjectName(QString::fromUtf8("chkOXZ"));
        chkOXZ->setGeometry(QRect(1, 158, 91, 19));
        grpView = new QGroupBox(groupDisplay);
        grpView->setObjectName(QString::fromUtf8("grpView"));
        grpView->setGeometry(QRect(1, 16, 125, 40));
        verticalLayout = new QVBoxLayout(grpView);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        rbViewShading = new QRadioButton(grpView);
        rbViewShading->setObjectName(QString::fromUtf8("rbViewShading"));

        verticalLayout->addWidget(rbViewShading);

        rbViewMesh = new QRadioButton(grpView);
        rbViewMesh->setObjectName(QString::fromUtf8("rbViewMesh"));

        verticalLayout->addWidget(rbViewMesh);

        grpType = new QGroupBox(groupDisplay);
        grpType->setObjectName(QString::fromUtf8("grpType"));
        grpType->setGeometry(QRect(1, 196, 69, 40));
        verticalLayout_2 = new QVBoxLayout(grpType);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        rbShape = new QRadioButton(grpType);
        rbShape->setObjectName(QString::fromUtf8("rbShape"));

        verticalLayout_2->addWidget(rbShape);

        rbColor = new QRadioButton(grpType);
        rbColor->setObjectName(QString::fromUtf8("rbColor"));

        verticalLayout_2->addWidget(rbColor);


        horizontalLayout->addWidget(groupDisplay);


        retranslateUi(ThreeDDialog);

        QMetaObject::connectSlotsByName(ThreeDDialog);
    } // setupUi

    void retranslateUi(QDialog *ThreeDDialog)
    {
        ThreeDDialog->setWindowTitle(QCoreApplication::translate("ThreeDDialog", "Dialog", nullptr));
        groupDisplay->setTitle(QCoreApplication::translate("ThreeDDialog", "Display", nullptr));
        chkOXY->setText(QCoreApplication::translate("ThreeDDialog", "Grid OXY", nullptr));
        chkOYZ->setText(QCoreApplication::translate("ThreeDDialog", "Grid OYZ", nullptr));
        chkAxis->setText(QCoreApplication::translate("ThreeDDialog", "Axis", nullptr));
        chkOXZ->setText(QCoreApplication::translate("ThreeDDialog", "Grid OXZ", nullptr));
        grpView->setTitle(QString());
        rbViewShading->setText(QCoreApplication::translate("ThreeDDialog", "View Shading", nullptr));
        rbViewMesh->setText(QCoreApplication::translate("ThreeDDialog", "View Mesh", nullptr));
        grpType->setTitle(QString());
        rbShape->setText(QCoreApplication::translate("ThreeDDialog", "shape", nullptr));
        rbColor->setText(QCoreApplication::translate("ThreeDDialog", "color", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ThreeDDialog: public Ui_ThreeDDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THREEDDIALOG_H
