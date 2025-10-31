#ifndef ISOLUXDIALOG_H
#define ISOLUXDIALOG_H

#include <QDialog>
#include "vtkMath.h"
#include "QVTKOpenGLNativeWidget.h"
#include <omp.h>
#include "IESIsoWidget.h"

class IESIsoWidget;
class vtkRenderer;
class vtkPoints;
class vtkFloatArray;
class vtkPolyData;
class vtkVertexGlyphFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkColorTransferFunction;
class vtkScalarBarActor;
class vtkMapper2D;
class vtkStructuredGrid;
class vtkDataSetSurfaceFilter;
class vtkLookupTable;
class vtkCubeAxesActor;
class vtkOrientationMarkerWidget;
class vtkAxesActor;
class vtkRenderWindowInteractor;
class vtkPolyDataNormals;
class vtkContourFilter;
class vtkExtractEdges;
class vtkDelaunay2D;
class vtkSurfaceReconstructionFilter;
class vtkGaussianSplatter;
class vtkElevationFilter;
class vtkSmoothPolyDataFilter;
class vtkVertexGlyphFilter;
//using namespace Qwt3D;
class vtkExtractSurface;

enum EPlaneType
{
    ePlaneX=0,
    ePlaneY,
    ePlaneZ,
    ePlaneX_,
    ePlaneY_,
    ePlaneZ_
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


    double fixtureX;
    double fixtureY;
    double fixtureZ;
    double calculationWidth;
    double workplaneHeight;
    double gridSpacing;
    int m_levelSize;
    bool m_bUseGrid;
    double maxIlluminance;
    double minIlluminance;
    int m_numOfPoints;

    void setAxisVisibility(bool visible);
    void setGridVisibility(bool visible);
    void clearPoints();
    void updateIESXY(double distance, double halfmap);
    void updateIESYZ(double distance, double halfmap);
    void updateIESXZ(double distance, double halfmap);

    void updateIESXY_(double distance, double halfmap);
    void updateIESYZ_(double distance, double halfmap);
    void updateIESXZ_(double distance, double halfmap);

    void setViewToIsometric45();
    double calculateIlluminanceAtPoint(double x, double y, double z);
    // ///////-z
    double calculateIlluminanceAtPoint_(double x, double y, double z);
    void updateCubeAxesBounds();

    vtkSmartPointer<vtkLookupTable> createColorLookupTable();
    vtkSmartPointer<vtkScalarBarActor> createScalarBar(vtkLookupTable* lut);
    vtkSmartPointer<vtkActor> createCoordinateGrid();

    vtkSmartPointer<vtkLookupTable> m_lut;
    vtkSmartPointer<vtkRenderer> m_renderer;          // 场景渲染器
    vtkSmartPointer<vtkPoints> m_points;              // 点云坐标
    vtkSmartPointer<vtkFloatArray> m_intensities;     // 强度值
    vtkSmartPointer<vtkPolyData> m_polyData;          // 点云数据
    vtkSmartPointer<vtkVertexGlyphFilter> m_glyphFilter; // 将点显示为顶点
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;      // 数据映射器
    vtkSmartPointer<vtkActor> m_actor;                // 场景中的演员
    vtkSmartPointer<vtkColorTransferFunction> m_colorTransferFunction; // 颜色映射函数
    vtkSmartPointer<vtkScalarBarActor> m_scalarBarActor;  // 色标演员
    vtkSmartPointer<vtkStructuredGrid> structuredGrid;
    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor;
    vtkSmartPointer<vtkAxesActor> axes;
    vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;
    vtkSmartPointer<vtkContourFilter> contourFilter;
    vtkSmartPointer<vtkActor> contourActor;
    vtkSmartPointer<vtkPolyDataMapper> contourMapper;



    vtkSmartPointer<vtkDelaunay2D> delaunay;
    vtkSmartPointer<vtkExtractEdges> extractEdges;
    vtkSmartPointer<vtkPolyDataMapper> wireframeMapper;
    vtkSmartPointer<vtkActor> wireframeActor;


    vtkSmartPointer<vtkPolyDataMapper> surfaceMapper;
    vtkSmartPointer<vtkActor> surfaceActor;
    //vtkSmartPointer<vtkPolyDataNormals> normalGenerator;
    double zmin;
    double zmax;

    double m_size = 25;
    // 坐标轴边界
    double bounds[6];
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
    void on_rbUnit0_toggled();
    void on_rbUnit1_toggled();
    void on_spinNbOfPoints_valueChanged(int);
private:
    Ui::ISOLuxDialog*ui;

    double m_m2feet;
    IESIsoWidget* m_isoWidget;
    ISOLuxPlot* m_3dplot;
};

#endif // ISOLUXDIALOG_H
