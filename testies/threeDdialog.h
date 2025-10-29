#ifndef THREEDDIALOG_H
#define THREEDDIALOG_H

#include <QDialog>
#include <QWindow>


namespace Ui {
class ThreeDDialog;
}


#include <QVTKOpenGLNativeWidget.h> // ����VTK��Ⱦ��Qt���
#include <vtkSmartPointer.h>        // VTK����ָ��

// ǰ������VTK�࣬�������ͷ�ļ�
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
class vtkUnsignedCharArray;
class vtkCellArray;
class vtkTransformFilter;
class vtkTransform;
class vtkVectorText;
class vtkFollower;

enum eFillStyle
{
    eShape=0,
    eColor
};
class IESPointCloudWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit IESPointCloudWidget(QWidget* parent = nullptr);
    ~IESPointCloudWidget();

    void clearPoints();
    void FillShapeData();
    void FillColorData();
    void addCoordinateAxes();

    std::string formatCandelaValue(double candelaValue);
    void addRadialScaleMarkers(vtkSmartPointer<vtkPoints> points,
        vtkSmartPointer<vtkCellArray> lines,
        double maxRadius,
        int radialDivisions,
        int numCircles);
    void createXYPlaneGrid(double r, double radialDivisions);
    void createYZPlaneGrid(double r,double radialDivisions);
    void createXZPlaneGrid(double r, double radialDivisions);
    void updateXYCandelaLabels(double maxRadius, int numCircles);
    void updateYZCandelaLabels(double maxRadius, int numCircles);
    void updateXZCandelaLabels(double maxRadius, int numCircles);



    void createXArrowSourceAxis(
        const std::string& axisName,
        double axisLength,
        double shaftRadius,
        double tipRadius,
        double colorR, double colorG, double colorB,
        double startX, double startY, double startZ,
        double dirX, double dirY, double dirZ);
    void createYArrowSourceAxis(
        const std::string& axisName,
        double axisLength,
        double shaftRadius,
        double tipRadius,
        double colorR, double colorG, double colorB,
        double startX, double startY, double startZ,
        double dirX, double dirY, double dirZ);
    void createZArrowSourceAxis(
        const std::string& axisName,
        double axisLength,
        double shaftRadius,
        double tipRadius,
        double colorR, double colorG, double colorB,
        double startX, double startY, double startZ,
        double dirX, double dirY, double dirZ);
    void addXArrowAxisLabel(const std::string& label, double x, double y, double z,
        double colorR, double colorG, double colorB);
    void addYArrowAxisLabel(const std::string& label, double x, double y, double z,
        double colorR, double colorG, double colorB);
    void addZArrowAxisLabel(const std::string& label, double x, double y, double z,
        double colorR, double colorG, double colorB);
    void createAxisArrow(double axisLength = 5.0,
        double shaftRadius = 0.01,
        double tipLength = 0.3,
        double tipRadius = 0.1,
        bool showLabels = true);
public Q_SLOTS:
    // ��ʼ��IES���ݲ�������ʾ
    void updateIESDataMesh(bool);
    void updateIESDataShading(bool);
    void updateIESDataColor(bool);
    void updateIESDataShape(bool);
    void on_chkOXY_stateChanged(int);
    void on_chkOXZ_stateChanged(int);
    void on_chkOYZ_stateChanged(int);
    void on_chkAxis_stateChanged(int);
private:

    // VTK�������ָ��
    vtkSmartPointer<vtkRenderer> m_renderer;          // ������Ⱦ��
    vtkSmartPointer<vtkPoints> m_points;              // ��������
    vtkSmartPointer<vtkFloatArray> m_intensities;     // ǿ��ֵ
    vtkSmartPointer<vtkPolyData> m_polyData;          // ��������
    vtkSmartPointer<vtkVertexGlyphFilter> m_glyphFilter; // ������ʾΪ����
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;      // ����ӳ����
    vtkSmartPointer<vtkActor> m_actor;                // �����е���Ա
    vtkSmartPointer<vtkColorTransferFunction> m_colorTransferFunction; // ��ɫӳ�亯��
    vtkSmartPointer<vtkScalarBarActor> m_scalarBarActor;  // ɫ����Ա
    vtkSmartPointer<vtkStructuredGrid> structuredGrid;

    vtkSmartPointer<vtkPoints> axisPoints;
    vtkSmartPointer<vtkCellArray> axisLines;
    vtkSmartPointer<vtkUnsignedCharArray> axisColors;
    vtkSmartPointer<vtkPolyData> axisData;
    vtkSmartPointer<vtkPolyDataMapper> axisMapper;
    vtkSmartPointer<vtkActor> axisActor;



    vtkSmartPointer<vtkActor> m_xyplane_actor;
    vtkSmartPointer<vtkActor> m_xzplane_actor;
    vtkSmartPointer<vtkActor> m_yzplane_actor;


    vtkSmartPointer<vtkPolyData> xz_gridData;
    vtkSmartPointer<vtkTransformFilter> xz_transformFilter;
    vtkSmartPointer<vtkTransform> xz_transform;
    vtkSmartPointer<vtkPolyDataMapper> xz_mapper;
    vtkSmartPointer<vtkPoints> xz_points;
    vtkSmartPointer<vtkCellArray> xz_lines;

    vtkSmartPointer<vtkPolyData> xy_gridData;
    vtkSmartPointer<vtkTransformFilter> xy_transformFilter;
    vtkSmartPointer<vtkTransform> xy_transform;
    vtkSmartPointer<vtkPolyDataMapper> xy_mapper;
    vtkSmartPointer<vtkPoints> xy_points;
    vtkSmartPointer<vtkCellArray> xy_lines;

    vtkSmartPointer<vtkPolyData> yz_gridData;
    vtkSmartPointer<vtkTransformFilter> yz_transformFilter;
    vtkSmartPointer<vtkTransform> yz_transform;
    vtkSmartPointer<vtkPolyDataMapper> yz_mapper;
    vtkSmartPointer<vtkPoints> yz_points;
    vtkSmartPointer<vtkCellArray> yz_lines;

    eFillStyle m_fillStyle;
    std::vector<vtkSmartPointer<vtkFollower>> m_xy_candelaLabels;
    std::vector<vtkSmartPointer<vtkFollower>> m_xz_candelaLabels;
    std::vector<vtkSmartPointer<vtkFollower>> m_yz_candelaLabels;


    vtkSmartPointer<vtkActor> m_xArrow_actor;
    vtkSmartPointer<vtkActor> m_yArrow_actor;
    vtkSmartPointer<vtkActor> m_zArrow_actor;
    vtkSmartPointer<vtkFollower> m_xArrow_textActor;
    vtkSmartPointer<vtkFollower> m_yArrow_textActor;
    vtkSmartPointer<vtkFollower> m_zArrow_textActor;
};


class ThreeDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThreeDDialog(QWidget *parent = nullptr);
    ~ThreeDDialog();
    void updateIES();

public Q_SLOTS:
    void updateIESDataColor(bool);
    void updateIESDataShape(bool);
    void updateIESDataMesh(bool);
    void updateIESDataShading(bool);
private:
    Ui::ThreeDDialog*ui;
    IESPointCloudWidget* m_iesWidget;
};

#endif // ISOLUXDIALOG_H
