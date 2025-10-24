#include "isoluxdialog.h"
#include "ui_isoluxdialog.h"
#include "IESIsoWidget.h"

#include "IESLoader.h"
#include <QOpenGLWidget>
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
#include <vtkLookupTable.h>
#include <vtkCubeAxesActor.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkProperty.h>
#include <vtkDelaunay2D.h>
#include <vtkExtractEdges.h>

#include <vtkSurfaceReconstructionFilter.h>
#include <vtkGaussianSplatter.h>
#include <vtkElevationFilter.h>
//#include <vtkVertexGlyphFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkExtractSurface.h>
#include <vtkLine.h>
ISOLuxPlot::ISOLuxPlot()/*:vtx(nullptr),itx(nullptr)*/
{ 
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_points = vtkSmartPointer<vtkPoints>::New();
    m_intensities = vtkSmartPointer<vtkFloatArray>::New();
    m_polyData = vtkSmartPointer<vtkPolyData>::New();
    m_glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    // 初始化边界
    for (int i = 0; i < 6; ++i) {
        bounds[i] = 0.0;
    }

    createColorLookupTable();
    createCoordinateGrid();
    createScalarBar(m_lut);

    // 创建映射器
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New(); 
    m_mapper->SetScalarRange(m_intensities->GetRange());
    m_mapper->SetLookupTable(createColorLookupTable());
    m_mapper->SetScalarModeToUsePointData();
    // 创建演员
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->GetProperty()->SetColor(0, 0, 0); // 天蓝色线框
    m_actor->SetMapper(m_mapper);
    m_renderer->AddActor(m_actor);

    gridSpacing = 0.1;
    calculationWidth = 20;
    m_size = 25;

    contourFilter = vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInputData(m_polyData);
    contourFilter->GenerateValues(m_levelSize, zmin, zmax);
    contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contourFilter->GetOutputPort());
    contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetColor(0, 0, 0); // 黑色轮廓线
    contourActor->GetProperty()->SetLineWidth(5);
    m_renderer->AddActor(contourActor);

    delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
    delaunay->SetInputData(m_polyData);
    delaunay->SetTolerance(0.001);
    delaunay->SetAlpha(1);
    delaunay->Update();
    //extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
    //extractEdges->SetInputConnection(delaunay->GetOutputPort());
    //extractEdges->Update();
    //wireframeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //wireframeMapper->SetInputConnection(extractEdges->GetOutputPort());
    //wireframeMapper->SetScalarRange(zmin, zmax);
    //wireframeMapper->SetLookupTable(m_lut);
    //wireframeMapper->ScalarVisibilityOn();
    //wireframeActor = vtkSmartPointer<vtkActor>::New();
    //wireframeActor->SetMapper(wireframeMapper);
    //wireframeActor->GetProperty()->SetLineWidth(1);
    //wireframeActor->GetProperty()->SetColor(0,0,0); // 蓝色线网

    surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    surfaceMapper->SetInputConnection(delaunay->GetOutputPort());
    surfaceMapper->SetScalarRange(zmin, zmax);
    surfaceMapper->SetLookupTable(m_lut);
    surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(surfaceMapper);
    surfaceActor->GetProperty()->SetOpacity(1); // 非常透明的表面

    //m_renderer->AddActor(wireframeActor);
    m_renderer->AddActor(surfaceActor); // 可选：添加透明表面


    m_mapper->SetInputData(m_polyData);


    m_scalarBarActor->SetTitle("Intensity");  // 设置标题
    m_scalarBarActor->SetMaximumNumberOfColors(256); // 设置颜色数量
    m_renderer->AddActor2D(m_scalarBarActor);
    this->renderWindow()->AddRenderer(m_renderer);
}


//void ISOLuxPlot::preGlWidgetChange() {
//    //release image texture
//}
//void ISOLuxPlot::onGlWidgetChanged() {
//    //create image texture
//
//}
//void ISOLuxPlot::generateMeshIndices() {
//    // 简单的网格生成算法 (仅用于演示)
//    // 实际应用中应该使用更复杂的算法如Poisson重建或Delaunay三角化
//    const int theta = 1810;
//    const int phi = 361;
//    const int numPoints = theta * phi;
//
//    // 创建规则网格
//    QVector<QVector3D> gridVertices;
//    QVector<float> gridValues;
//
//   
//}
//void ISOLuxPlot::OnImguiFrameShow()
//{

    //ImGui::Begin("HELLO IMGUI", 0, 0);
    //ImPlot3DSurfaceFlags flags = ImPlot3DSurfaceFlags_NoMarkers;


    //long N = IESLoader::instance().coords.size();
    //float* xs = new float[N];
    //float* ys = new float[N];
    //float* zs = new float[N];
    //static float t = 0.0f;
    ////t += ImGui::GetIO().DeltaTime;
    //double range_min = INFINITY;
    //double range_max = -INFINITY;
    //if(!vtx)
    //    vtx = new ImPlot3DPoint();
    //if (!itx)
    //    itx = new unsigned int[N];
    //for (long i = 0; i < IESLoader::instance().coords.size(); i++) {

    //    QVector3D p = IESLoader::instance().coords[i];
    //    xs[i] = p.x();
    //    ys[i] = p.y();
    //    zs[i] = p.z();
    //    vtx->x = p.x();
    //    vtx->y = p.y();
    //    vtx->z = p.z();
    //    itx[i] = p.length();
    //    if (zs[i] < range_min)range_min = zs[i];
    //    if (zs[i] > range_max)range_max = zs[i];
    //}

    //if (ImPlot3D::BeginPlot("Surface Plots", ImVec2(-1, 400), ImPlot3DFlags_NoClip)) {
    //    ImPlot3D::SetupAxesLimits(-1, 1, -1, 1, -1.5, 1.5);

    //    // Set fill style
    //    ImPlot3D::PushStyleVar(ImPlot3DStyleVar_FillAlpha, 0.8f);
    //    // Set line style
    //    ImPlot3D::SetNextLineStyle(ImPlot3D::GetColormapColor(1));

    //    // Set marker style
    //    ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Square, IMPLOT3D_AUTO, ImPlot3D::GetColormapColor(2));

    //    // Plot the surface
    //    //if (custom_range)
    //    ImPlot3D::PlotMesh("Wave Surface", vtx,itx,N,N);
    //        //ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, 361, 181, (double)range_min, (double)range_max, flags);
    //    //else
    //    //    ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, N, N, 0.0, 0.0, flags);

    //    // End the plot
    //    ImPlot3D::PopStyleVar();
    //    ImPlot3D::EndPlot();
    //}
    //delete []vtx;
    //delete[]itx;
    //itx = nullptr;
    //vtx = nullptr;
    //delete []xs;
    //delete []ys;
    //delete []zs;
    //ImGui::End();
//}
ISOLuxDialog::ISOLuxDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ISOLuxDialog)
{
    ui->setupUi(this);

    m_isoWidget = new IESIsoWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    ui->iso2d->setLayout(hLayout);
    hLayout->addWidget(m_isoWidget);

    m_3dplot = new ISOLuxPlot();
    QHBoxLayout* hLayout3d = new QHBoxLayout();
    ui->iso3d->setLayout(hLayout3d);
    hLayout3d->addWidget(m_3dplot);

    ui->spinLevels->setValue(5);
    ui->spinDistance->setValue(5);
    ui->spinHalfMapWidth->setValue(10);
    ui->rbUnit0->setChecked(true);
    ui->spinNbOfPoints->setValue(61);
    ui->rb2D->setChecked(true);


    ui->spinLevels->setFocusPolicy(Qt::NoFocus);
}

void ISOLuxDialog::on_spinHalfMapWidth_valueChanged(int)
{
    updateIES();
}
void ISOLuxDialog::on_spinLevels_valueChanged(int)
{
    updateIES();
}
void ISOLuxDialog::on_spinDistance_valueChanged(int)
{
    updateIES();
}

void ISOLuxDialog::on_chkGrid_stateChanged(int value)
{
    m_isoWidget->m_bUseGrid = value;
    m_3dplot->m_bUseGrid = value;
    updateIES();
}
void ISOLuxDialog::on_cmbPlane_currentIndexChanged(int plane)
{
    bool is2D = ui->rb2D->isChecked();
    double distance = ui->spinDistance->value();
    double halfMap = ui->spinHalfMapWidth->value();
    switch (plane)
    {
    case ePlaneX:
    {
        if (is2D)
        {
            m_isoWidget->m_levelSize = ui->spinLevels->value();
            m_isoWidget->updateIESYZ(distance,halfMap);
        }
        else
        {
            m_3dplot->m_levelSize = ui->spinLevels->value();
            m_3dplot->updateIESYZ(distance,halfMap);
        }
        //if()
    }
        break;
    case ePlaneY:
    {
        if (is2D)
        {
            m_isoWidget->m_levelSize = ui->spinLevels->value();
            m_isoWidget->updateIESXZ(distance, halfMap);
        }
        else
        {
            m_3dplot->m_levelSize = ui->spinLevels->value();
            m_3dplot->updateIESXZ(distance, halfMap);
        }
    }
        break;
    case ePlaneZ:
    {
        if (is2D)
        {

            m_isoWidget->m_levelSize = ui->spinLevels->value();
            m_isoWidget->updateIESXY(distance, halfMap);
        }
        else
        {
            m_3dplot->m_levelSize = ui->spinLevels->value();
            m_3dplot->updateIESXY(distance, halfMap);
        }
    }
        break;
    default:
        break;
    }
    return;
}
void ISOLuxDialog::on_rb2D_toggled()
{ 
    ui->isoLuxwidget->setCurrentWidget(ui->iso2d);
}
void ISOLuxDialog::on_rb3D_toggled()
{
    ui->isoLuxwidget->setCurrentWidget(ui->iso3d);
    updateIES();
    m_3dplot->setViewToIsometric45();
}
ISOLuxDialog::~ISOLuxDialog()
{
    delete ui;
}

void ISOLuxDialog::updateIES()
{
    on_cmbPlane_currentIndexChanged(ui->cmbPlane->currentIndex());
}

void ISOLuxPlot::updateIESXY(double distance, double halfmap)
{
    if (IESLoader::instance().light.candela.size() < 1)
        return;
    clearPoints();

    m_scalarBarActor->SetNumberOfLabels(m_levelSize);   // 设置标签数量
    m_intensities->SetName("Intensity");
    // 计算最大光强值用于归一化
    double maxIntensity = IESLoader::instance().light.max_candela;
    maxIlluminance = 0;
    minIlluminance = 1e9;
    calculationWidth = halfmap * 2;
    double halfWidth = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    fixtureX = 0;
    fixtureY = 0;
    fixtureZ = distance;

    double radio = calculationWidth / m_size;
    zmax = 0;
    for (int i = 0; i < gridSize; ++i) {
        std::vector<double> oneLine;
        double x = -halfWidth + i * gridSpacing;
        for (int j = 0; j < gridSize; ++j) {
            double y = -halfWidth + j * gridSpacing;
            float intensity = calculateIlluminanceAtPoint(x, y, 0);
            if (intensity > zmax)zmax = intensity;
            m_intensities->InsertNextValue(intensity);
        }
    }
    double z_radio = zmax / m_size;
    for (int i = 0; i < gridSize; ++i) {
        std::vector<double> oneLine;
        double x = -halfWidth + i * gridSpacing;
        double x_scale = x / radio;
        for (int j = 0; j < gridSize; ++j) {
            double y = -halfWidth + j * gridSpacing;
            double y_scale = y / radio;
            float intensity = calculateIlluminanceAtPoint(x, y, 0);
            m_points->InsertNextPoint(x_scale, y_scale, intensity / z_radio);
        }
    }

    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    // 添加水平线
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize - 1; ++j) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = i * gridSize + (j + 1);

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }

    // 添加垂直线
    for (int j = 0; j < gridSize; ++j) {
        for (int i = 0; i < gridSize - 1; ++i) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = (i + 1) * gridSize + j;

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }

    // 创建多边形数据
    m_polyData->SetPoints(m_points);
    //m_polyData->SetPolys(cells);
    m_polyData->GetPointData()->SetScalars(m_intensities);
    m_polyData->SetLines(lines);

    // 使用顶点滤波器显示点
    //m_glyphFilter->SetInputData(m_polyData);
    //m_glyphFilter->Update();

    if (cubeAxesActor && m_polyData)
    {
        m_polyData->GetBounds(bounds);
        cubeAxesActor->SetBounds(bounds);
        cubeAxesActor->SetZAxisRange(zmin, zmax);
        cubeAxesActor->SetXAxisRange(-halfmap, halfmap);
        cubeAxesActor->SetYAxisRange(-halfmap, halfmap);
        cubeAxesActor->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);
        vtkCamera* camera = m_renderer->GetActiveCamera();
        m_lut->SetRange(zmin, zmax);
        m_scalarBarActor->SetLookupTable(m_lut);
        cubeAxesActor->SetCamera(camera);
    }
    ///////////////////////////
    surfaceMapper->SetScalarRange(zmin, zmax);
    surfaceMapper->SetLookupTable(m_lut);
    surfaceMapper->Update();

    updateCubeAxesBounds();
    contourFilter->GenerateValues(m_levelSize, zmin, zmax);
    contourFilter->Update();

    this->renderWindow()->Render();
}
void ISOLuxPlot::updateIESYZ(double distance, double halfmap)
{
    if (IESLoader::instance().light.candela.size() < 1)
        return;

    clearPoints();

    m_scalarBarActor->SetNumberOfLabels(m_levelSize);   // 设置标签数量

    m_intensities->SetName("Intensity");
    // 计算最大光强值用于归一化
    double maxIntensity = IESLoader::instance().light.max_candela;
    maxIlluminance = 0;
    minIlluminance = 1e9;
    calculationWidth = halfmap * 2;
    double halfWidth = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

    fixtureX = distance;
    fixtureY = 0;
    fixtureZ = 0;
    double radio =  calculationWidth/ m_size;
    zmax = 0;
    for (int i = 0; i < gridSize; ++i) {
        std::vector<double> oneLine;
        double y = -halfWidth + i * gridSpacing;
        for (int j = 0; j < gridSize; ++j) {
            double z = -halfWidth + j * gridSpacing;         
            float intensity = calculateIlluminanceAtPoint(0, y, z);
            if (intensity > zmax)zmax = intensity;
            m_intensities->InsertNextValue(intensity);
        }
    }
    double z_radio = zmax / m_size;
    for (int i = 0; i < gridSize; ++i) {
        std::vector<double> oneLine;
        double y = -halfWidth + i * gridSpacing;
        double y_scale = y / radio;
        for (int j = 0; j < gridSize; ++j) {
            double z = -halfWidth + j * gridSpacing;
            double z_scale = z / radio;
            float intensity = calculateIlluminanceAtPoint(0, y, z);
            m_points->InsertNextPoint(y_scale, z_scale,intensity / z_radio);
        }
    }

    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    // 添加水平线
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize - 1; ++j) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = i * gridSize + (j + 1);

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }

    // 添加垂直线
    for (int j = 0; j < gridSize; ++j) {
        for (int i = 0; i < gridSize - 1; ++i) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = (i + 1) * gridSize + j;

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }

    // 创建多边形数据
    m_polyData->SetPoints(m_points);
    //m_polyData->SetPolys(cells);
    m_polyData->GetPointData()->SetScalars(m_intensities);
    m_polyData->SetLines(lines);
    // 使用顶点滤波器显示点
    //m_glyphFilter->SetInputData(m_polyData);
    //m_glyphFilter->Update();

    // 更新立方体坐标轴边界
    updateCubeAxesBounds();
    if (cubeAxesActor && m_polyData)
    {
        m_polyData->GetBounds(bounds);
        cubeAxesActor->SetBounds(bounds);
        cubeAxesActor->SetXAxisRange(-halfmap, halfmap);
        cubeAxesActor->SetYAxisRange(-halfmap, halfmap);
        cubeAxesActor->SetZAxisRange(zmin, zmax);
        //m_lut->SetRange(zmin, zmax);
        m_lut->SetRange(zmin, zmax);
        m_lut->SetTableRange(zmin, zmax);
        m_lut->Build();
        m_scalarBarActor->SetLookupTable(m_lut);

        vtkCamera* camera = m_renderer->GetActiveCamera();
        cubeAxesActor->SetCamera(camera);
    }

    surfaceMapper->SetScalarRange(zmin, zmax);
    surfaceMapper->SetLookupTable(m_lut);
    surfaceMapper->Update();

    contourFilter->GenerateValues(m_levelSize, zmin, zmax);
    contourFilter->Update();
    this->renderWindow()->Render();
}
// 创建颜色查找表
vtkSmartPointer<vtkLookupTable> ISOLuxPlot::createColorLookupTable() 
{
    m_lut = vtkSmartPointer<vtkLookupTable>::New();
    //m_lut->SetHueRange(0.0, 0.7); // 从红色到蓝色
    //m_lut->SetSaturationRange(1.0, 1.0);
    //m_lut->SetValueRange(1.0, 1.0);
    //m_lut->SetAlphaRange(1.0, 1.0);
    for (int i = 0; i < 256; i++) {
        double t = i / 255.0;
        double r, g, b;

        if (t < 0.25) {
            // 蓝色到青色
            double local_t = t / 0.25;
            r = 0.0;
            g = 0.5 * local_t;
            b = 1.0 - 0.5 * local_t;
        }
        else if (t < 0.5) {
            // 青色到绿色
            double local_t = (t - 0.25) / 0.25;
            r = 0.0;
            g = 0.5 + 0.5 * local_t;
            b = 0.5 - 0.5 * local_t;
        }
        else if (t < 0.75) {
            // 绿色到黄色
            double local_t = (t - 0.5) / 0.25;
            r = 1.0 * local_t;
            g = 1.0;
            b = 0.0;
        }
        else {
            // 黄色到橙色
            double local_t = (t - 0.75) / 0.25;
            r = 1.0;
            g = 1.0 - 0.5 * local_t;
            b = 0.0;
        }

        m_lut->SetTableValue(i, r, g, b, 1.0);
    }
    m_lut->SetNumberOfTableValues(256);
    m_lut->Build();
    return m_lut;
}
void ISOLuxPlot::clearPoints()
{
    m_points->Reset();
    m_intensities->Reset();
}
// 创建标量条
vtkSmartPointer<vtkScalarBarActor> ISOLuxPlot::createScalarBar(vtkLookupTable* lut) {
    m_scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    m_scalarBarActor->SetLookupTable(lut);
    m_scalarBarActor->SetTitle("Light Intensity");
    m_scalarBarActor->SetNumberOfLabels(5);
    m_scalarBarActor->GetTitleTextProperty()->SetColor(1, 1, 1);
    m_scalarBarActor->GetLabelTextProperty()->SetColor(1, 1, 1);
    m_scalarBarActor->SetWidth(0.1);
    m_scalarBarActor->SetHeight(0.7);
    m_scalarBarActor->SetPosition(0.85, 0.15);
    return m_scalarBarActor;
}
// 创建坐标轴网格
vtkSmartPointer<vtkActor> ISOLuxPlot::createCoordinateGrid() 
{
    cubeAxesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
    //cubeAxesActor->SetCamera(m_renderer->GetActiveCamera());
    // 设置坐标轴标题
    //cubeAxesActor->SetXTitle("X Axis (m)");
    //cubeAxesActor->SetYTitle("Y Axis (m)");
    //cubeAxesActor->SetZTitle("Z Axis (m)");

    // 设置坐标轴标签格式
    //cubeAxesActor->SetXLabelFormat("%6.1f");
    //cubeAxesActor->SetYLabelFormat("%6.1f");
    //cubeAxesActor->SetZLabelFormat("%6.1f");
    //cubeAxesActor->GetXAxesLinesProperty()->SetColor(1, 1, 1); // 白色轴线
    //cubeAxesActor->GetYAxesLinesProperty()->SetColor(1, 1, 1);
    //cubeAxesActor->GetZAxesLinesProperty()->SetColor(1, 1, 1);

    //cubeAxesActor->GetTitleTextProperty(0)->SetColor(1, 1, 1);
    //cubeAxesActor->GetTitleTextProperty(1)->SetColor(1, 1, 1);
    //cubeAxesActor->GetTitleTextProperty(2)->SetColor(1, 1, 1);

    //cubeAxesActor->GetLabelTextProperty(0)->SetColor(1, 1, 1);
    //cubeAxesActor->GetLabelTextProperty(1)->SetColor(1, 1, 1);
    //cubeAxesActor->GetLabelTextProperty(2)->SetColor(1, 1, 1);
    //cubeAxesActor->SetFlyModeToOuterEdges(); // 坐标轴始终在边界上

    //cubeAxesActor->SetScreenSize(6);
    cubeAxesActor->DrawXGridlinesOn();
    cubeAxesActor->DrawYGridlinesOn();
    cubeAxesActor->DrawZGridlinesOn();

    cubeAxesActor->XAxisMinorTickVisibilityOff();
    cubeAxesActor->YAxisMinorTickVisibilityOff();
    cubeAxesActor->ZAxisMinorTickVisibilityOn();
    //cubeAxesActor->YAxes[i]->SetMinorTicksVisible(1);

    cubeAxesActor->SetFlyModeToFurthestTriad();
    cubeAxesActor->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);

    // 创建方向标记（可选，用于显示方向）
    ////axes = vtkSmartPointer<vtkAxesActor>::New();
    ////axes->SetTotalLength(1.0, 1.0, 1.0);
    ////axes->SetShaftTypeToCylinder();
    ////axes->SetCylinderRadius(0.01);
    ////interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    ////axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    ////axesWidget->SetOrientationMarker(axes);
    ////axesWidget->SetInteractor(interactor);
    ////axesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
    ////axesWidget->SetEnabled(1);
    ////axesWidget->InteractiveOn(); // 允许交互

    //m_renderer->AddActor(cubeAxesActor);
    //m_renderer->ResetCamera();
    //m_renderer->ResetCameraClippingRange();
    m_renderer->AddActor(cubeAxesActor);

    return cubeAxesActor;
}
void ISOLuxPlot::setAxisVisibility(bool visible) {
    if (cubeAxesActor) 
    {
        if (visible) 
        {
            cubeAxesActor->XAxisVisibilityOn();
            cubeAxesActor->YAxisVisibilityOn();
            cubeAxesActor->ZAxisVisibilityOn();
        }
        else 
        {
            cubeAxesActor->XAxisVisibilityOff();
            cubeAxesActor->YAxisVisibilityOff();
            cubeAxesActor->ZAxisVisibilityOff();
        }
        this->renderWindow()->Render();
    }
}
void ISOLuxPlot::updateCubeAxesBounds() {

    if (m_bUseGrid)
    {
        cubeAxesActor->DrawXGridlinesOn();
        cubeAxesActor->DrawYGridlinesOn();
        cubeAxesActor->DrawZGridlinesOn();
    }
    else
    {
        cubeAxesActor->DrawXGridlinesOff();
        cubeAxesActor->DrawYGridlinesOff();
        cubeAxesActor->DrawZGridlinesOff();
    }
}


void ISOLuxPlot::setViewToIsometric45() {
    // 45°等轴测视图
    vtkCamera* camera = m_renderer->GetActiveCamera();

    // 计算45°等轴测位置
    double distance = 100; // 相机距离
    double angle = 45.0 * vtkMath::Pi() / 180.0; // 45度转换为弧度

    // 45°等轴测位置 (1,1,1)方向
    double x = distance * sin(angle) * cos(angle);
    double y = distance * sin(angle) * sin(angle);
    double z = distance * cos(angle);

    camera->SetPosition(x, y, z);
    camera->SetFocalPoint(0, 0, 10);
    camera->SetViewUp(0, 0, 1);  // Z轴向上

    // 调整CubeAxesActor
    if (cubeAxesActor) {
        cubeAxesActor->SetCamera(camera);
    }

    //currentViewMode = ISOMETRIC_VIEW;
    this->renderWindow()->Render();
}

void ISOLuxPlot::setGridVisibility(bool visible) 
{
    if (cubeAxesActor) 
    {
        if (visible) {
            cubeAxesActor->DrawXGridlinesOn();
            cubeAxesActor->DrawYGridlinesOn();
            cubeAxesActor->DrawZGridlinesOn();
        }
        else {
            cubeAxesActor->DrawXGridlinesOff();
            cubeAxesActor->DrawYGridlinesOff();
            cubeAxesActor->DrawZGridlinesOff();
        }
        this->renderWindow()->Render();
    }
}

void ISOLuxPlot::updateIESXZ(double distance, double halfmap)
{
    if (IESLoader::instance().light.candela.size() < 1)
        return;
    clearPoints();

    m_scalarBarActor->SetNumberOfLabels(m_levelSize);   // 设置标签数量
    m_intensities->SetName("Intensity");
    // 计算最大光强值用于归一化
    double maxIntensity = IESLoader::instance().light.max_candela;
    maxIlluminance = 0;
    minIlluminance = 1e9;
    calculationWidth = halfmap * 2;
    double halfWidth = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    fixtureX = 0;
    fixtureY = distance;
    fixtureZ = 0;

    double radio = calculationWidth/ m_size;
    zmax = 0;

    for (int i = 0; i < gridSize; ++i) {
        const double x = -halfWidth + i * gridSpacing;
        for (int j = 0; j < gridSize; ++j) {
            const double z = -halfWidth + j * gridSpacing;
            const float intensity = calculateIlluminanceAtPoint(x, 0, z);

            if (intensity > zmax)zmax = intensity;
            m_intensities->InsertNextValue(intensity);
        }
    }

    double z_radio = zmax / m_size;

    for (int i = 0; i < gridSize; ++i) {
        const double x = -halfWidth + i * gridSpacing;
        const double x_scale = x / radio;
        //#pragma omp for
        for (int j = 0; j < gridSize; ++j) {
            const double z = -halfWidth + j * gridSpacing;
            const double z_scale = z / radio;
            const float intensity = calculateIlluminanceAtPoint(x, 0, z);

            m_points->InsertNextPoint(x_scale, z_scale, intensity / z_radio);
        }
    }

    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    // 添加水平线
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize - 1; ++j) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = i * gridSize + (j + 1);

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }

    // 添加垂直线
    for (int j = 0; j < gridSize; ++j) {
        for (int i = 0; i < gridSize - 1; ++i) {
            vtkIdType pt1 = i * gridSize + j;
            vtkIdType pt2 = (i + 1) * gridSize + j;

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pt1);
            line->GetPointIds()->SetId(1, pt2);
            lines->InsertNextCell(line);
        }
    }
    // 创建网格三角形
    //for (int i = 0; i < gridSize - 1; ++i) {
    //    for (int j = 0; j < gridSize - 1; ++j) {
    //        vtkIdType quad[4];
    //        quad[0] = i * gridSize + j;
    //        quad[1] = i * gridSize + (j + 1);
    //        quad[2] = (i + 1) * gridSize + (j + 1);
    //        quad[3] = (i + 1) * gridSize + j;

    //        // 第一个三角形
    //        vtkSmartPointer<vtkIdList> triangle1 = vtkSmartPointer<vtkIdList>::New();
    //        triangle1->InsertNextId(quad[0]);
    //        triangle1->InsertNextId(quad[1]);
    //        triangle1->InsertNextId(quad[2]);
    //        cells->InsertNextCell(triangle1);

    //        // 第二个三角形
    //        vtkSmartPointer<vtkIdList> triangle2 = vtkSmartPointer<vtkIdList>::New();
    //        triangle2->InsertNextId(quad[0]);
    //        triangle2->InsertNextId(quad[2]);
    //        triangle2->InsertNextId(quad[3]);
    //        cells->InsertNextCell(triangle2);
    //    }
    //}
    // 创建多边形数据
    m_polyData->SetPoints(m_points);
    //m_polyData->SetPolys(cells);
    m_polyData->GetPointData()->SetScalars(m_intensities);
    m_polyData->SetLines(lines);
    // 使用顶点滤波器显示点
    //m_glyphFilter->SetInputData(m_polyData);
    //m_glyphFilter->Update();

    if (cubeAxesActor && m_polyData)
    {
        m_polyData->GetBounds(bounds);
        cubeAxesActor->SetBounds(bounds);
        cubeAxesActor->SetZAxisRange(zmin, zmax);
        cubeAxesActor->SetXAxisRange(-halfmap, halfmap);
        cubeAxesActor->SetYAxisRange(-halfmap, halfmap);
        m_lut->SetRange(zmin, zmax);
        m_lut->SetTableRange(zmin, zmax);
        m_lut->Build();
        m_scalarBarActor->SetLookupTable(m_lut);
        vtkCamera* camera = m_renderer->GetActiveCamera();
        cubeAxesActor->SetCamera(camera);
    }

    surfaceMapper->SetScalarRange(zmin, zmax);
    surfaceMapper->SetLookupTable(m_lut);
    surfaceMapper->Update();

    updateCubeAxesBounds();
    contourFilter->GenerateValues(m_levelSize, zmin, zmax);
    contourFilter->Update();

    this->renderWindow()->Render();
}

double ISOLuxPlot::calculateIlluminanceAtPoint(double x, double y, double z)
{
    // 计算距离
    double dx = x - fixtureX;
    double dy = y - fixtureY;
    double dz = z - fixtureZ;
    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);

    if (totalDistance == 0) return 0;

    // 计算垂直角度 (从灯具向下为正)
    double verticalAngle = acos(-dz / totalDistance) * 180.0 / M_PI;  // 注意符号

    // 计算水平角度
    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;
    if (horizontalAngle < 0) horizontalAngle += 360;

    // 获取光强值
    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);

    // 计算照度 (距离平方反比定律 + 余弦定律)
    double cosIncidence = -dz / totalDistance;  // 入射角余弦
    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;

    return std::max(0.0, illuminance);
}