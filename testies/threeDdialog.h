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
    // ��ʼ��IES���ݲ�������ʾ
    void updateIESDataMesh(bool);
    void updateIESDataShading(bool);
    void updateIESDataColor(bool);
    void updateIESDataShape(bool);
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
