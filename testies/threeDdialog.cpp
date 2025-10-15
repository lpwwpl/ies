#include "threeDdialog.h"
#include "ui_threeddialog.h"
#include "IESglWidget.h"
#include <cmath>


// 包含必要的VTK头文件
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkMapper2D.h>
#include <vtkStructuredGrid.h>
#include <vtkDataSetSurfaceFilter.h>

//#include "dislin.h"
ThreeDDialog::ThreeDDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ThreeDDialog)
{
    ui->setupUi(this);

    m_iesWidget = new IESPointCloudWidget();
    //m_glWidget = new PointCloudViewer();
    QHBoxLayout* hLayout = new QHBoxLayout();
    ui->widgetThreeD->setLayout(hLayout);
    hLayout->addWidget(m_iesWidget);

    ui->rbViewShading->setChecked(true);
    ui->rbColor->setChecked(true);
    
    connect(ui->rbViewMesh, SIGNAL(toggled(bool)), m_iesWidget, SLOT(updateIESDataMesh(bool)));
    connect(ui->rbViewShading, SIGNAL(toggled(bool)), m_iesWidget, SLOT(updateIESDataShading(bool)));
    connect(ui->rbColor, SIGNAL(toggled(bool)), m_iesWidget, SLOT(updateIESDataColor(bool)));
    connect(ui->rbShape, SIGNAL(toggled(bool)), m_iesWidget, SLOT(updateIESDataShape(bool)));
}

ThreeDDialog::~ThreeDDialog()
{
    delete ui;
}
void ThreeDDialog::updateIES()
{
    //m_iesWidget->clearPoints();
    if (IESLoader::instance().light.candela.size() < 1)
        return;

    if (ui->rbColor->isChecked())
    {
        m_iesWidget->updateIESDataColor(true);
    }
    else if (ui->rbShape->isChecked())
    {
        m_iesWidget->updateIESDataShape(true);
    }
    
    if (ui->rbViewMesh->isChecked())
    {
        m_iesWidget->updateIESDataMesh(true);
    }
    else if (ui->rbViewShading->isChecked())
    {
        m_iesWidget->updateIESDataShading(true);
    } 
}


IESPointCloudWidget::IESPointCloudWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
{
    // 初始化VTK组件
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_points = vtkSmartPointer<vtkPoints>::New();
    m_intensities = vtkSmartPointer<vtkFloatArray>::New();
    m_polyData = vtkSmartPointer<vtkPolyData>::New();
    m_glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    // 设置强度值数组名
    m_intensities->SetName("Intensity");

    // 组装PolyData
    m_polyData->SetPoints(m_points);
    //m_polyData->GetPointData()->SetScalars(m_intensities);// ->SetScalars(m_intensities); // 设置强度值用于颜色映射


    structuredGrid->SetPoints(m_points);
    structuredGrid->GetPointData()->SetScalars(m_intensities);

    // 设置GlyphFilter
    m_glyphFilter->SetInputData(m_polyData);
    surfaceFilter->SetInputData(structuredGrid);


    m_mapper->SetScalarModeToUsePointData(); // 使用点数据中的标量
    m_mapper->SetLookupTable(m_colorTransferFunction); // 设置颜色查找表

    // 设置Actor
    m_actor->SetMapper(m_mapper);

    // 添加Actor到Renderer
    m_renderer->AddActor(m_actor);

    // 设置渲染器背景色:cite[6]
    m_renderer->SetBackground(0.2, 0.2, 0.2); // 深灰色背景

    // 创建色标演员
    m_scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    m_scalarBarActor->SetTitle("Intensity");  // 设置标题
    m_scalarBarActor->SetNumberOfLabels(5);   // 设置标签数量
    m_scalarBarActor->SetMaximumNumberOfColors(256); // 设置颜色数量
    m_renderer->AddActor2D(m_scalarBarActor);
    // 设置标题文本属性
    vtkTextProperty* titleProperty = m_scalarBarActor->GetTitleTextProperty();
    titleProperty->SetFontSize(12);
    titleProperty->SetColor(1.0, 1.0, 1.0);  // 白色
    titleProperty->BoldOn();

    // 设置标签文本属性
    vtkTextProperty* labelProperty = m_scalarBarActor->GetLabelTextProperty();
    labelProperty->SetFontSize(10);

    labelProperty->SetColor(1.0, 1.0, 1.0);  // 白色
    // 获取渲染窗口并添加渲染器:cite[1]:cite[5]
    this->renderWindow()->AddRenderer(m_renderer);

    
}

IESPointCloudWidget::~IESPointCloudWidget()
{
    // 清理资源
    if (this->renderWindow()) {
        this->renderWindow()->RemoveRenderer(m_renderer);
    }
}
void IESPointCloudWidget::updateIESDataShape(bool value)
{
    if (IESLoader::instance().light.candela.size() < 1)return;
    if (!value)return;
    FillShapeData();
    // 设置Mapper
    m_mapper->SetInputConnection(m_glyphFilter->GetOutputPort());

    // 通知组件数据已更新
    m_polyData->Modified();
    //structuredGrid->Modified();
    m_points->Modified();
    m_intensities->Modified();
    m_mapper->Modified();
    // 重置相机以显示所有点:cite[8]
    m_renderer->ResetCamera();

    //m_renderer->Render();
    // 渲染窗口:cite[8]
    this->renderWindow()->Render();
}
void IESPointCloudWidget::updateIESDataColor(bool value)
{    
    if (IESLoader::instance().light.candela.size() < 1)return;
    if (!value)return;
    FillColorData();
    m_mapper->SetInputConnection(surfaceFilter->GetOutputPort());

    // 通知组件数据已更新
    //m_polyData->Modified();
    structuredGrid->Modified();
    m_points->Modified();
    m_intensities->Modified();
    m_mapper->Modified();
    // 重置相机以显示所有点:cite[8]
    m_renderer->ResetCamera();

    //m_renderer->Render();
    // 渲染窗口:cite[8]
    this->renderWindow()->Render();

}
void IESPointCloudWidget::clearPoints()
{
    m_points->Reset();
    m_intensities->Reset();

}
void IESPointCloudWidget::updateIESDataShading(bool value)
{
    if (!value)return;
}

void IESPointCloudWidget::FillColorData()
{
    clearPoints();
    m_points->SetNumberOfPoints(IESLoader::instance().coords.size());
    m_intensities->SetNumberOfValues(IESLoader::instance().coords.size());

    for (long i = 0; i < IESLoader::instance().coords.size(); i++)
    {
        double t = IESLoader::instance().tflat[i];
        double p = IESLoader::instance().pflat[i];
        //double r = IESLoader::instance().intensity[i];
        QVector3D v = IESLoader::instance().polar_to_cartesian(t, p, 1);
        double intensity = IESLoader::instance().intensity[i];
        m_points->SetPoint(i, v.x(), v.y(), v.z());
        m_intensities->SetValue(i, intensity);
    }
    structuredGrid->SetDimensions(361, 181, 1);
    // 更新颜色映射范围
    double range[2];
    m_intensities->GetRange(range);
    m_mapper->SetScalarRange(range);

    // 设置颜色映射函数 (蓝-绿-红)
    m_colorTransferFunction->RemoveAllPoints();
    m_colorTransferFunction->AddRGBPoint(range[0], 0.0, 0.0, 1.0); // 蓝色代表低强度
    m_colorTransferFunction->AddRGBPoint((range[0] + range[1]) / 2, 0.0, 1.0, 0.0); // 绿色代表中强度
    m_colorTransferFunction->AddRGBPoint(range[1], 1.0, 0.0, 0.0); // 红色代表高强度


    m_scalarBarActor->SetLookupTable(m_colorTransferFunction);
    m_scalarBarActor->SetTitle("Intensity");

    // 设置色标位置和大小 (右侧垂直放置)
    m_scalarBarActor->SetPosition(0.85, 0.1);    // 位置 (相对坐标)
    m_scalarBarActor->SetWidth(0.1);             // 宽度
    m_scalarBarActor->SetHeight(0.8);            // 高度
        // 设置色标方向为垂直

    m_scalarBarActor->SetOrientationToVertical();
}
void IESPointCloudWidget::FillShapeData()
{
    clearPoints();
    m_points->SetNumberOfPoints(IESLoader::instance().coords.size());
    //m_intensities->SetNumberOfValues(IESLoader::instance().coords.size());

    for (long i = 0; i < IESLoader::instance().coords.size(); i++)
    {
        double t = IESLoader::instance().tflat[i];
        double p = IESLoader::instance().pflat[i];
        double r = IESLoader::instance().intensity[i];
        QVector3D v = IESLoader::instance().polar_to_cartesian(t, p, r);
        double intensity = IESLoader::instance().intensity[i];
        m_points->SetPoint(i, v.x(), v.y(), v.z());
        //m_intensities->SetValue(i, v.z());
    }
 
    // 更新颜色映射范围
    double range[2];
    //m_intensities->GetRange(range);
    //m_mapper->SetScalarRange(range);

    // 设置颜色映射函数 (蓝-绿-红)
    //m_colorTransferFunction->RemoveAllPoints();
    //m_colorTransferFunction->AddRGBPoint(range[0], 0.0, 0.0, 1.0); // 蓝色代表低强度
    //m_colorTransferFunction->AddRGBPoint((range[0] + range[1]) / 2, 0.0, 1.0, 0.0); // 绿色代表中强度
    //m_colorTransferFunction->AddRGBPoint(range[1], 1.0, 0.0, 0.0); // 红色代表高强度


    //m_scalarBarActor->SetLookupTable(m_colorTransferFunction);
    //m_scalarBarActor->SetTitle("Intensity");

    //// 设置色标位置和大小 (右侧垂直放置)
    //m_scalarBarActor->SetPosition(0.85, 0.1);    // 位置 (相对坐标)
    //m_scalarBarActor->SetWidth(0.1);             // 宽度
    //m_scalarBarActor->SetHeight(0.8);            // 高度
    //    // 设置色标方向为垂直

    //m_scalarBarActor->SetOrientationToVertical();
}

void IESPointCloudWidget::updateIESDataMesh(bool value)
{   
    if (!value)return;
}