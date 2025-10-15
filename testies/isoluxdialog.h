#ifndef ISOLUXDIALOG_H
#define ISOLUXDIALOG_H

#include <QDialog>
//#include "qwt3d_plot.h"
#include "vtkMath.h"
#include "QVTKOpenGLNativeWidget.h"
//struct ImPlot3DPoint;
class IESIsoWidget;
//using namespace Qwt3D;

enum EPlaneType
{
    ePlaneX=0,
    ePlaneY,
    ePlaneZ
};
namespace Ui {
class ISOLuxDialog;
}

//class ISOLuxPlot : public QImGuiInterface, public QImguiSingleton<ISOLuxPlot>
//{
//public:
//    ISOLuxPlot();
//    void OnImguiFrameShow()  override;
//    void preGlWidgetChange() override;
//    void onGlWidgetChanged() override;
//    void generateMeshIndices();
//
//
//    ImPlot3DPoint* vtx;
//    unsigned int* itx;
//};

class ISOLuxPlot : public QVTKOpenGLNativeWidget
{
public:
    ISOLuxPlot();
    void updateData();
};

class ISOLuxDialog : public QDialog
{
    Q_OBJECT
        
public:
    explicit ISOLuxDialog(QWidget *parent = nullptr);
    ~ISOLuxDialog();
    void updateIES();

public Q_SLOTS:
    void on_rb2D_toggled();
    void on_rb3D_toggled();
    void on_spinHalfMapWidth_valueChanged(int);
    void on_spinDistance_valueChanged(int);
    void on_cmbPlane_currentIndexChanged(int);
    void on_spinLevels_valueChanged(int);
    void on_chkGrid_stateChanged(int);
private:
    Ui::ISOLuxDialog*ui;

    IESIsoWidget* m_isoWidget;
    ISOLuxPlot* m_3dplot;
};

#endif // ISOLUXDIALOG_H
