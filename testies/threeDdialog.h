#ifndef THREEDDIALOG_H
#define THREEDDIALOG_H

#include <QDialog>
#include <QWindow>


namespace Ui {
class ThreeDDialog;
}


#include <QVTKOpenGLNativeWidget.h> // 用于VTK渲染的Qt组件
#include <vtkSmartPointer.h>        // VTK智能指针

// 前向声明VTK类，避免包含头文件
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
class IESPointCloudWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit IESPointCloudWidget(QWidget* parent = nullptr);
    ~IESPointCloudWidget();

    void clearPoints();
    void FillShapeData();
    void FillColorData();
public Q_SLOTS:
    // 初始化IES数据并更新显示
    void updateIESDataMesh(bool);
    void updateIESDataShading(bool);
    void updateIESDataColor(bool);
    void updateIESDataShape(bool);
private:

    // VTK组件智能指针
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
};


class ThreeDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThreeDDialog(QWidget *parent = nullptr);
    ~ThreeDDialog();
    void updateIES();
private:
    Ui::ThreeDDialog*ui;
    IESPointCloudWidget* m_iesWidget;
};

#endif // ISOLUXDIALOG_H
