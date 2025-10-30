#include "threeDdialog.h"
#include "ui_threeddialog.h"
#include "IESglWidget.h"
#include <cmath>


// ������Ҫ��VTKͷ�ļ�
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
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkLine.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkArrowSource.h>
#include <vtkTransformPolyDataFilter.h>
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
    

    Qt::WindowFlags flags = Qt::Dialog;
    // �����󻯺���С����ť
    flags |= Qt::WindowMinMaxButtonsHint;
    // ��ӹرհ�ť
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    bool value = connect(ui->chkOXY, SIGNAL(stateChanged(int)), m_iesWidget, SLOT(on_chkOXY_stateChanged(int)));
    connect(ui->chkOXZ, SIGNAL(stateChanged(int)), m_iesWidget, SLOT(on_chkOXZ_stateChanged(int)));
    connect(ui->chkOYZ, SIGNAL(stateChanged(int)), m_iesWidget, SLOT(on_chkOYZ_stateChanged(int)));
    connect(ui->chkAxis, SIGNAL(stateChanged(int)), m_iesWidget, SLOT(on_chkAxis_stateChanged(int)));

    connect(ui->rbViewMesh, SIGNAL(toggled(bool)), this, SLOT(updateIESDataMesh(bool)));
    connect(ui->rbViewShading, SIGNAL(toggled(bool)), this, SLOT(updateIESDataShading(bool)));
    connect(ui->rbColor, SIGNAL(toggled(bool)), this, SLOT(updateIESDataColor(bool)));
    connect(ui->rbShape, SIGNAL(toggled(bool)), this, SLOT(updateIESDataShape(bool)));
}

ThreeDDialog::~ThreeDDialog()
{
    delete ui;
}
void ThreeDDialog::updateIESDataColor(bool value)
{
    m_iesWidget->updateIESDataColor(value);
    m_iesWidget->on_chkOYZ_stateChanged(ui->chkOYZ->isChecked());
    m_iesWidget->on_chkOXZ_stateChanged(ui->chkOXZ->isChecked());
    m_iesWidget->on_chkOXY_stateChanged(ui->chkOXY->isChecked());
    m_iesWidget->on_chkAxis_stateChanged(ui->chkAxis->isChecked());
}
void ThreeDDialog::updateIESDataShape(bool value)
{
    m_iesWidget->updateIESDataShape(value);
    m_iesWidget->on_chkOYZ_stateChanged(ui->chkOYZ->isChecked());
    m_iesWidget->on_chkOXZ_stateChanged(ui->chkOXZ->isChecked());
    m_iesWidget->on_chkOXY_stateChanged(ui->chkOXY->isChecked());
    m_iesWidget->on_chkAxis_stateChanged(ui->chkAxis->isChecked());
}
void ThreeDDialog::updateIESDataMesh(bool value)
{
    m_iesWidget->updateIESDataMesh(value);
    m_iesWidget->on_chkOYZ_stateChanged(ui->chkOYZ->isChecked());
    m_iesWidget->on_chkOXZ_stateChanged(ui->chkOXZ->isChecked());
    m_iesWidget->on_chkOXY_stateChanged(ui->chkOXY->isChecked());
    m_iesWidget->on_chkAxis_stateChanged(ui->chkAxis->isChecked());
}
void ThreeDDialog::updateIESDataShading(bool value)
{
    m_iesWidget->updateIESDataShading(value);
    m_iesWidget->on_chkOYZ_stateChanged(ui->chkOYZ->isChecked());
    m_iesWidget->on_chkOXZ_stateChanged(ui->chkOXZ->isChecked());
    m_iesWidget->on_chkOXY_stateChanged(ui->chkOXY->isChecked());
    m_iesWidget->on_chkAxis_stateChanged(ui->chkAxis->isChecked());
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
    m_iesWidget->on_chkOYZ_stateChanged(ui->chkOYZ->isChecked());
    m_iesWidget->on_chkOXZ_stateChanged(ui->chkOXZ->isChecked());
    m_iesWidget->on_chkOXY_stateChanged(ui->chkOXY->isChecked());
    m_iesWidget->on_chkAxis_stateChanged(ui->chkAxis->isChecked());
}


IESPointCloudWidget::IESPointCloudWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
{
    // ��ʼ��VTK���
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
    // ����ǿ��ֵ������
    m_intensities->SetName("Intensity");

    // ��װPolyData
    m_polyData->SetPoints(m_points);
    //m_polyData->GetPointData()->SetScalars(m_intensities);// ->SetScalars(m_intensities); // ����ǿ��ֵ������ɫӳ��


    structuredGrid->SetPoints(m_points);
    structuredGrid->GetPointData()->SetScalars(m_intensities);

    // ����GlyphFilter
    m_glyphFilter->SetInputData(m_polyData);
    surfaceFilter->SetInputData(structuredGrid);

    //////////////
    //addCoordinateAxes();

    m_mapper->SetScalarModeToUsePointData(); // ʹ�õ������еı���
    m_mapper->SetLookupTable(m_colorTransferFunction); // ������ɫ���ұ�

    // ����Actor
    m_actor->SetMapper(m_mapper);

    // ���Actor��Renderer
    m_renderer->AddActor(m_actor);

    // ������Ⱦ������ɫ:cite[6]
    m_renderer->SetBackground(0.2, 0.2, 0.2); // ���ɫ����

    // ����ɫ����Ա
    m_scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    m_scalarBarActor->SetTitle("Intensity");  // ���ñ���
    m_scalarBarActor->SetNumberOfLabels(5);   // ���ñ�ǩ����
    m_scalarBarActor->SetMaximumNumberOfColors(256); // ������ɫ����
    m_renderer->AddActor2D(m_scalarBarActor);
    // ���ñ����ı�����
    vtkTextProperty* titleProperty = m_scalarBarActor->GetTitleTextProperty();
    titleProperty->SetFontSize(12);
    titleProperty->SetColor(1.0, 1.0, 1.0);  // ��ɫ
    titleProperty->BoldOn();

    // ���ñ�ǩ�ı�����
    vtkTextProperty* labelProperty = m_scalarBarActor->GetLabelTextProperty();
    labelProperty->SetFontSize(10);

    labelProperty->SetColor(1.0, 1.0, 1.0);  // ��ɫ
    // ��ȡ��Ⱦ���ڲ������Ⱦ��:cite[1]:cite[5]
    this->renderWindow()->AddRenderer(m_renderer);    
}

IESPointCloudWidget::~IESPointCloudWidget()
{
    // ������Դ
    if (this->renderWindow()) {
        this->renderWindow()->RemoveRenderer(m_renderer);
    }
}
void IESPointCloudWidget::updateIESDataShape(bool value)
{
    if (IESLoader::instance().light.candela.size() < 1)return;
    if (!value)return;
    m_fillStyle = eShape;
    FillShapeData();
    // ����Mapper
    m_mapper->SetInputConnection(m_glyphFilter->GetOutputPort());

    // ֪ͨ��������Ѹ���
    m_polyData->Modified();
    //structuredGrid->Modified();
    m_points->Modified();
    m_intensities->Modified();
    m_mapper->Modified();
    // �����������ʾ���е�:cite[8]
    m_renderer->ResetCamera();

    //m_renderer->Render();
    // ��Ⱦ����:cite[8]
    this->renderWindow()->Render();
}
void IESPointCloudWidget::updateIESDataColor(bool value)
{    
    if (IESLoader::instance().light.candela.size() < 1)return;
    if (!value)return;
    m_fillStyle = eColor;

    FillColorData();
    m_mapper->SetInputConnection(surfaceFilter->GetOutputPort());

    // ֪ͨ��������Ѹ���
    //m_polyData->Modified();
    structuredGrid->Modified();
    m_points->Modified();
    m_intensities->Modified();
    m_mapper->Modified();
    // �����������ʾ���е�:cite[8]
    m_renderer->ResetCamera();

    //m_renderer->Render();
    // ��Ⱦ����:cite[8]
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


    //////////////////////////////
}

// ��Ӿ���̶��ߣ��ھ���������Ӷ̶̿ȱ�ǣ�
void IESPointCloudWidget::addRadialScaleMarkers(vtkSmartPointer<vtkPoints> points,
    vtkSmartPointer<vtkCellArray> lines,
    double maxRadius,
    int radialDivisions,
    int numCircles)
{
    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;

    // ��ÿ������������ӿ̶ȱ��
    for (int i = 0; i < radialDivisions; i++) {
        double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;

        // ���㾶���ߵķ�������
        double dirX = cos(angle);
        double dirY = sin(angle);

        // ���С�̶ȣ���ͬ��Բ֮�䣩
        int minorTicks = 4; // ÿ�������С�̶�����
        for (int circle = 1; circle <= numCircles; circle++) {
            double outerRadius = maxRadius * circle / numCircles;
            double innerRadius = maxRadius * (circle - 1) / numCircles;
            double segmentLength = outerRadius - innerRadius;

            for (int tick = 1; tick < minorTicks; tick++) {
                double radius = innerRadius + segmentLength * tick / minorTicks;
                double tickLength = maxRadius * 0.03; // �̶��߳���

                // ����̶��ߵ������յ㣨��ֱ�ھ����ߣ�
                double mainX = centerX + radius * dirX;
                double mainY = centerY + radius * dirY;

                // ��ֱ��������
                double perpX = -dirY;
                double perpY = dirX;

                // �̶��ߵ������˵�
                double startX = mainX - perpX * tickLength * 0.5;
                double startY = mainY - perpY * tickLength * 0.5;
                double endX = mainX + perpX * tickLength * 0.5;
                double endY = mainY + perpY * tickLength * 0.5;

                // ��ӿ̶���
                vtkIdType startPoint = points->InsertNextPoint(startX, startY, centerZ);
                vtkIdType endPoint = points->InsertNextPoint(endX, endY, centerZ);

                vtkSmartPointer<vtkLine> tickLine = vtkSmartPointer<vtkLine>::New();
                tickLine->GetPointIds()->SetId(0, startPoint);
                tickLine->GetPointIds()->SetId(1, endPoint);
                lines->InsertNextCell(tickLine);
            }
        }
    }
}



void IESPointCloudWidget::createYZPlaneGrid(double maxRadius, double radialDivisions)
{
    if (m_yzplane_actor.Get())
        m_renderer->RemoveActor(m_yzplane_actor);
    yz_gridData = vtkSmartPointer<vtkPolyData>::New(); // createPolarGrid(0, 0, 0, 2.0, 8);
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;

    // ��Ӽ��������ĵ�
    points->InsertNextPoint(centerX, centerY, centerZ);
    // ����������
    for (int i = 0; i < radialDivisions; i++) {
        double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
        double x = centerX + maxRadius * cos(angle);
        double y = centerY + maxRadius * sin(angle);
        double z = centerZ;

        points->InsertNextPoint(x, y, z);

        // ���������ĵ�Բ�ܵ���
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, 0); // ���ĵ�
        line->GetPointIds()->SetId(1, i + 1); // Բ�ܵ�
        lines->InsertNextCell(line);
    }

    // ����ͬ��Բ
    int numCircles = 5;
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;
        int startIndex = points->GetNumberOfPoints();

        // ����Բ�ܵ�
        for (int i = 0; i <= radialDivisions; i++) {
            double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
            double x = centerX + radius * cos(angle);
            double y = centerY + radius * sin(angle);
            double z = centerZ;

            points->InsertNextPoint(x, y, z);
        }

        // ����Բ����
        for (int i = 0; i < radialDivisions; i++) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, startIndex + i);
            line->GetPointIds()->SetId(1, startIndex + i + 1);
            lines->InsertNextCell(line);
        }
    }
  

    yz_gridData->SetPoints(points);
    yz_gridData->SetLines(lines);

    // ��ת��YZƽ��
    yz_transform = vtkSmartPointer<vtkTransform>::New();
    yz_transform->RotateX(90); // ��X����ת90��

    yz_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    yz_transformFilter->SetInputData(yz_gridData);
    yz_transformFilter->SetTransform(yz_transform);
    yz_transformFilter->Update();

    yz_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    yz_mapper->SetInputConnection(yz_transformFilter->GetOutputPort());

    m_yzplane_actor = vtkSmartPointer<vtkActor>::New();
    m_yzplane_actor->SetMapper(yz_mapper);
    m_yzplane_actor->GetProperty()->SetColor(0.0, 0.0, 1.0); // ��ɫ
    m_yzplane_actor->GetProperty()->SetLineWidth(2);

    m_renderer->AddActor(m_yzplane_actor);

    updateYZCandelaLabels(maxRadius, numCircles);
}
std::string IESPointCloudWidget::formatCandelaValue(double candelaValue)
{
    return std::to_string(static_cast<int>(candelaValue)) + " cd";
}


// ��ӹ�ǿֵ��ǩ��xxx cd��
void IESPointCloudWidget::updateXYCandelaLabels(double maxRadius, int numCircles)
{
    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;

    // ���֮ǰ�ı�ǩ
    for (auto& label : m_xy_candelaLabels) {
        if (label.Get()) {
            m_renderer->RemoveActor(label);
        }
    }
    m_xy_candelaLabels.clear();
    if (m_fillStyle == eColor)
        return;

    // Ϊÿ����̶��ߣ�ͬ��Բ����ӹ�ǿֵ��ǩ
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;

        // ��ȡ��Ӧ�Ĺ�ǿֵ
        double candelaValue = 0.0;
        //if (circle - 1 < candelaValues.size()) {
        //    candelaValue = candelaValues[circle - 1];
        //}
        candelaValue = radius;// candelaValues[circle - 1];
        // �����ı�Դ
        vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();

        // ��ʽ����ǿֵ�ı�
        std::string labelText = formatCandelaValue(candelaValue);
        textSource->SetText(labelText.c_str());

        // �����ı�ӳ����
        vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        textMapper->SetInputConnection(textSource->GetOutputPort());

        // �����ı���Ա
        vtkSmartPointer<vtkFollower> textActor = vtkSmartPointer<vtkFollower>::New();
        textActor->SetMapper(textMapper);

        // ����ǩ������0�ȷ���ľ�������
        double labelOffset = 0;// maxRadius * 0.1; // ��ǩ�������ߵľ���
        //double labelX = centerX + radius * cos(0) + labelOffset;
        //double labelY = centerY + radius * sin(0);
        //double labelZ = centerZ;
        double labelX = centerX + radius * sin(0);
        double labelY = centerY;
        double labelZ = centerZ + radius * cos(0) + labelOffset;

        textActor->SetPosition(labelX, labelY, labelZ);
        textActor->SetScale(maxRadius * 0.03); // �ı���С
        textActor->GetProperty()->SetColor(1.0,0.0, 0.0); // ��ɫ�ı���ͻ����ʾ

        // Ӧ����������ͬ�ı任
        vtkSmartPointer<vtkTransform> textTransform = vtkSmartPointer<vtkTransform>::New();
        textTransform->RotateY(90);
        textActor->SetUserTransform(textTransform);

      
        m_xy_candelaLabels.push_back(textActor);
    }
    for (auto& label : m_xy_candelaLabels) {
        if (label.Get()) {
            m_renderer->AddActor(label);
        }
    }
}

void IESPointCloudWidget::createAxisArrow(double axisLength,
    double shaftRadius,
    double tipLength,
    double tipRadius,
    bool showLabels)
{
    // X�ᣨ��ɫ��
    createXArrowSourceAxis("X", axisLength, shaftRadius, tipRadius,
        1.0, 0.0, 0.0, 0, 0, 0, 1, 0, 0);

    // Y�ᣨ��ɫ��
    createYArrowSourceAxis("Y", axisLength, shaftRadius, tipRadius,
        0.0, 1.0, 0.0, 0, 0, 0, 0, 1, 0);

    // Z�ᣨ��ɫ��
    createZArrowSourceAxis("Z", axisLength, shaftRadius, tipRadius,
        0.0, 0.0, 1.0, 0, 0, 0, 0, 0, 1);
}

void IESPointCloudWidget::createXArrowSourceAxis(
    const std::string& axisName,
    double axisLength,
    double shaftRadius,
    double tipRadius,
    double colorR, double colorG, double colorB,
    double startX, double startY, double startZ,
    double dirX, double dirY, double dirZ
) {
    if (m_xArrow_actor.Get())
    {
        m_renderer->RemoveActor(m_xArrow_actor);
    }
    if (m_xArrow_textActor.Get())
    {
        m_renderer->RemoveActor(m_xArrow_textActor);
    }
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    arrowSource->SetTipLength(0.25);  // ��ͷ���ȱ���
    arrowSource->SetTipRadius(tipRadius);
    arrowSource->SetShaftRadius(shaftRadius);
    arrowSource->SetTipResolution(20);
    arrowSource->SetShaftResolution(20);
    arrowSource->Update();

    // ����任
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    // �ƶ������
    transform->Translate(startX, startY, startZ);

    // ������ת��Ĭ�ϼ�ͷ��X�᷽��
    double defaultDir[3] = { 1.0, 0.0, 0.0 };
    double targetDir[3] = { dirX, dirY, dirZ };
    vtkMath::Normalize(targetDir);

    double rotationAngle = vtkMath::DegreesFromRadians(
        acos(vtkMath::Dot(defaultDir, targetDir))
    );

    double rotationAxis[3];
    vtkMath::Cross(defaultDir, targetDir, rotationAxis);

    if (vtkMath::Norm(rotationAxis) > 0.001) {
        vtkMath::Normalize(rotationAxis);
        transform->RotateWXYZ(rotationAngle, rotationAxis);
    }

    // ���ż�ͷ��ָ������
    double arrowLength = vtkMath::Norm(targetDir) * axisLength;
    transform->Scale(arrowLength, arrowLength, arrowLength);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(arrowSource->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    // ����ӳ��������Ա
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());

    m_xArrow_actor = vtkSmartPointer<vtkActor>::New();
    m_xArrow_actor->SetMapper(mapper);
    m_xArrow_actor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_xArrow_actor);

    // ��ӱ�ǩ
    addXArrowAxisLabel(axisName, startX + dirX * axisLength * 1.15,
        startY + dirY * axisLength * 1.15,
        startZ + dirZ * axisLength * 1.15,
        colorR, colorG, colorB);
}

void IESPointCloudWidget::createYArrowSourceAxis(
    const std::string& axisName,
    double axisLength,
    double shaftRadius,
    double tipRadius,
    double colorR, double colorG, double colorB,
    double startX, double startY, double startZ,
    double dirX, double dirY, double dirZ
) {
    if (m_yArrow_actor.Get())
    {
        m_renderer->RemoveActor(m_yArrow_actor);
    }
    if (m_yArrow_textActor.Get())
    {
        m_renderer->RemoveActor(m_yArrow_textActor);
    }
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    arrowSource->SetTipLength(0.25);  // ��ͷ���ȱ���
    arrowSource->SetTipRadius(tipRadius);
    arrowSource->SetShaftRadius(shaftRadius);
    arrowSource->SetTipResolution(20);
    arrowSource->SetShaftResolution(20);
    arrowSource->Update();

    // ����任
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    // �ƶ������
    transform->Translate(startX, startY, startZ);

    // ������ת��Ĭ�ϼ�ͷ��X�᷽��
    double defaultDir[3] = { 1.0, 0.0, 0.0 };
    double targetDir[3] = { dirX, dirY, dirZ };
    vtkMath::Normalize(targetDir);

    double rotationAngle = vtkMath::DegreesFromRadians(
        acos(vtkMath::Dot(defaultDir, targetDir))
    );

    double rotationAxis[3];
    vtkMath::Cross(defaultDir, targetDir, rotationAxis);

    if (vtkMath::Norm(rotationAxis) > 0.001) {
        vtkMath::Normalize(rotationAxis);
        transform->RotateWXYZ(rotationAngle, rotationAxis);
    }

    // ���ż�ͷ��ָ������
    double arrowLength = vtkMath::Norm(targetDir) * axisLength;
    transform->Scale(arrowLength, arrowLength, arrowLength);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(arrowSource->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    // ����ӳ��������Ա
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());

    m_yArrow_actor = vtkSmartPointer<vtkActor>::New();
    m_yArrow_actor->SetMapper(mapper);
    m_yArrow_actor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_yArrow_actor);

    // ��ӱ�ǩ
    addYArrowAxisLabel(axisName, startX + dirX * axisLength * 1.15,
        startY + dirY * axisLength * 1.15,
        startZ + dirZ * axisLength * 1.15,
        colorR, colorG, colorB);
}
void IESPointCloudWidget::createZArrowSourceAxis(
    const std::string& axisName,
    double axisLength,
    double shaftRadius,
    double tipRadius,
    double colorR, double colorG, double colorB,
    double startX, double startY, double startZ,
    double dirX, double dirY, double dirZ
) {
    if (m_zArrow_actor.Get())
    {
        m_renderer->RemoveActor(m_zArrow_actor);
    }
    if (m_zArrow_textActor.Get())
    {
        m_renderer->RemoveActor(m_zArrow_textActor);
    }
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    arrowSource->SetTipLength(0.25);  // ��ͷ���ȱ���
    arrowSource->SetTipRadius(tipRadius);
    arrowSource->SetShaftRadius(shaftRadius);
    arrowSource->SetTipResolution(20);
    arrowSource->SetShaftResolution(20);
    arrowSource->Update();

    // ����任
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    // �ƶ������
    transform->Translate(startX, startY, startZ);

    // ������ת��Ĭ�ϼ�ͷ��X�᷽��
    double defaultDir[3] = { 1.0, 0.0, 0.0 };
    double targetDir[3] = { dirX, dirY, dirZ };
    vtkMath::Normalize(targetDir);

    double rotationAngle = vtkMath::DegreesFromRadians(
        acos(vtkMath::Dot(defaultDir, targetDir))
    );

    double rotationAxis[3];
    vtkMath::Cross(defaultDir, targetDir, rotationAxis);

    if (vtkMath::Norm(rotationAxis) > 0.001) {
        vtkMath::Normalize(rotationAxis);
        transform->RotateWXYZ(rotationAngle, rotationAxis);
    }

    // ���ż�ͷ��ָ������
    double arrowLength = vtkMath::Norm(targetDir) * axisLength;
    transform->Scale(arrowLength, arrowLength, arrowLength);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(arrowSource->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    // ����ӳ��������Ա
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());

    m_zArrow_actor = vtkSmartPointer<vtkActor>::New();
    m_zArrow_actor->SetMapper(mapper);
    m_zArrow_actor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_zArrow_actor);

    // ��ӱ�ǩ
    addZArrowAxisLabel(axisName, startX + dirX * axisLength * 1.15,
        startY + dirY * axisLength * 1.15,
        startZ + dirZ * axisLength * 1.15,
        colorR, colorG, colorB);
}
void IESPointCloudWidget::addXArrowAxisLabel(const std::string& label, double x, double y, double z,
    double colorR, double colorG, double colorB) {
    vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
    textSource->SetText(label.c_str());

    vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    m_xArrow_textActor = vtkSmartPointer<vtkFollower>::New();
    m_xArrow_textActor->SetMapper(textMapper);
    m_xArrow_textActor->SetPosition(x, y, z);
    m_xArrow_textActor->SetScale(0.15);
    m_xArrow_textActor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_xArrow_textActor);
}
void IESPointCloudWidget::addYArrowAxisLabel(const std::string& label, double x, double y, double z,
    double colorR, double colorG, double colorB) {
    vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
    textSource->SetText(label.c_str());

    vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    m_yArrow_textActor = vtkSmartPointer<vtkFollower>::New();
    m_yArrow_textActor->SetMapper(textMapper);
    m_yArrow_textActor->SetPosition(x, y, z);
    m_yArrow_textActor->SetScale(0.15);
    m_yArrow_textActor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_yArrow_textActor);
}
void IESPointCloudWidget::addZArrowAxisLabel(const std::string& label, double x, double y, double z,
    double colorR, double colorG, double colorB) {
    vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
    textSource->SetText(label.c_str());

    vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    m_zArrow_textActor = vtkSmartPointer<vtkFollower>::New();
    m_zArrow_textActor->SetMapper(textMapper);
    m_zArrow_textActor->SetPosition(x, y, z);
    m_zArrow_textActor->SetScale(0.15);
    m_zArrow_textActor->GetProperty()->SetColor(colorR, colorG, colorB);

    m_renderer->AddActor(m_zArrow_textActor);
}
void IESPointCloudWidget::updateXZCandelaLabels(double maxRadius, int numCircles)
{
    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;

    // ���֮ǰ�ı�ǩ
    for (auto& label : m_xz_candelaLabels) {
        if (label.Get()) {
            m_renderer->RemoveActor(label);
        }
    }
    m_xz_candelaLabels.clear();
    if (m_fillStyle == eColor)
        return;

    // Ϊÿ����̶��ߣ�ͬ��Բ����ӹ�ǿֵ��ǩ
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;

        // ��ȡ��Ӧ�Ĺ�ǿֵ
        double candelaValue = 0.0;

        candelaValue = radius;
        // �����ı�Դ
        vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();

        // ��ʽ����ǿֵ�ı�
        std::string labelText = formatCandelaValue(candelaValue);
        textSource->SetText(labelText.c_str());

        // �����ı�ӳ����
        vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        textMapper->SetInputConnection(textSource->GetOutputPort());

        // �����ı���Ա
        vtkSmartPointer<vtkFollower> textActor = vtkSmartPointer<vtkFollower>::New();
        textActor->SetMapper(textMapper);

        // ����ǩ������0�ȷ���ľ�������
        double labelOffset = 0;// maxRadius * 0.1; // ��ǩ�������ߵľ���
        double labelX = centerX + radius * sin(0);
        double labelY = centerY;
        double labelZ = centerZ + radius * cos(0) + labelOffset;

        textActor->SetPosition(labelX, labelY, labelZ);
        textActor->SetScale(maxRadius * 0.03); // �ı���С
        textActor->GetProperty()->SetColor(0, 1.0, 0.0); // ��ɫ�ı���ͻ����ʾ

        // Ӧ����������ͬ�ı任
        vtkSmartPointer<vtkTransform> textTransform = vtkSmartPointer<vtkTransform>::New();
        textTransform->RotateX(90);
        textActor->SetUserTransform(textTransform);


        m_xz_candelaLabels.push_back(textActor);
    }
    for (auto& label : m_xz_candelaLabels) {
        if (label.Get()) {
            m_renderer->AddActor(label);
        }
    }
}
void IESPointCloudWidget::updateYZCandelaLabels(double maxRadius, int numCircles)
{
    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;

    // ���֮ǰ�ı�ǩ
    for (auto& label : m_yz_candelaLabels) {
        if (label.Get()) {
            m_renderer->RemoveActor(label);
        }
    }
    m_yz_candelaLabels.clear();

    if (m_fillStyle == eColor)
        return;

    // Ϊÿ����̶��ߣ�ͬ��Բ����ӹ�ǿֵ��ǩ
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;

        // ��ȡ��Ӧ�Ĺ�ǿֵ
        double candelaValue = 0.0;
        //if (circle - 1 < candelaValues.size()) {
        //    candelaValue = candelaValues[circle - 1];
        //}
        candelaValue = radius;
        // �����ı�Դ
        vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();

        // ��ʽ����ǿֵ�ı�
        std::string labelText = formatCandelaValue(candelaValue);
        textSource->SetText(labelText.c_str());

        // �����ı�ӳ����
        vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        textMapper->SetInputConnection(textSource->GetOutputPort());

        // �����ı���Ա
        vtkSmartPointer<vtkFollower> textActor = vtkSmartPointer<vtkFollower>::New();
        textActor->SetMapper(textMapper);

        // ����ǩ������0�ȷ���ľ�������
        double labelOffset = 0;// maxRadius * 0.1; // ��ǩ�������ߵľ���
        double labelX = centerX + radius * sin(0);
        double labelY = centerY;
        double labelZ = centerZ + radius * cos(0) + labelOffset;

        textActor->SetPosition(labelX, labelY, labelZ);
        textActor->SetScale(maxRadius * 0.03); // �ı���С
        textActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // ��ɫ�ı���ͻ����ʾ

        // Ӧ����������ͬ�ı任
        vtkSmartPointer<vtkTransform> textTransform = vtkSmartPointer<vtkTransform>::New();
        textTransform->RotateZ(90);
        textActor->SetUserTransform(textTransform);


        m_yz_candelaLabels.push_back(textActor);
    }

    for (auto& label : m_yz_candelaLabels) {
        if (label.Get()) {
            m_renderer->AddActor(label);
        }
    }
}

void IESPointCloudWidget::createXZPlaneGrid(double maxRadius, double radialDivisions)
{
    if (m_xzplane_actor.Get())
        m_renderer->RemoveActor(m_xzplane_actor);


    xz_gridData = vtkSmartPointer<vtkPolyData>::New();
    xz_points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> xz_lines = vtkSmartPointer<vtkCellArray>::New();

    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;
    // ��Ӽ��������ĵ�
    xz_points->InsertNextPoint(centerX, centerY, centerZ);
    // ����������
    for (int i = 0; i < radialDivisions; i++) {
        double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
        double x = centerX + maxRadius * cos(angle);
        double y = centerY + maxRadius * sin(angle);
        double z = centerZ;
        xz_points->InsertNextPoint(x, y, z);
        // ���������ĵ�Բ�ܵ���
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, 0); // ���ĵ�
        line->GetPointIds()->SetId(1, i + 1); // Բ�ܵ�
        xz_lines->InsertNextCell(line);
    }

    // ����ͬ��Բ
    int numCircles = 5;
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;
        int startIndex = xz_points->GetNumberOfPoints();

        // ����Բ�ܵ�
        for (int i = 0; i <= radialDivisions; i++) {
            double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
            double x = centerX + radius * cos(angle);
            double y = centerY + radius * sin(angle);
            double z = centerZ;

            xz_points->InsertNextPoint(x, y, z);
        }

        // ����Բ����
        for (int i = 0; i < radialDivisions; i++) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, startIndex + i);
            line->GetPointIds()->SetId(1, startIndex + i + 1);
            xz_lines->InsertNextCell(line);
        }
    }
    //addRadialScaleMarkers(xz_points, xz_lines, maxRadius, radialDivisions, numCircles);

    xz_gridData->SetPoints(xz_points);
    xz_gridData->SetLines(xz_lines);

    // ��ת��XZƽ��
    xz_transform = vtkSmartPointer<vtkTransform>::New();
    xz_transform->RotateY(90); // ��Y����ת90��

    xz_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    xz_transformFilter->SetInputData(xz_gridData);
    xz_transformFilter->SetTransform(xz_transform);
    xz_transformFilter->Update();

    xz_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    xz_mapper->SetInputConnection(xz_transformFilter->GetOutputPort());

    m_xzplane_actor = vtkSmartPointer<vtkActor>::New();
    m_xzplane_actor->SetMapper(xz_mapper);
    m_xzplane_actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // ��ɫ
    m_xzplane_actor->GetProperty()->SetLineWidth(2);    
    m_renderer->AddActor(m_xzplane_actor);

    updateXZCandelaLabels(maxRadius, numCircles);
}
void IESPointCloudWidget::createXYPlaneGrid(double maxRadius, double radialDivisions)
{
    if (m_xyplane_actor.Get())
        m_renderer->RemoveActor(m_xyplane_actor);

    xy_gridData = vtkSmartPointer<vtkPolyData>::New();
    xy_points = vtkSmartPointer<vtkPoints>::New();
    xy_lines = vtkSmartPointer<vtkCellArray>::New();

    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;
    // ��Ӽ��������ĵ�
    xy_points->InsertNextPoint(centerX, centerY, centerZ);
    // ����������
    for (int i = 0; i < radialDivisions; i++) {
        double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
        double x = centerX + maxRadius * cos(angle);
        double y = centerY + maxRadius * sin(angle);
        double z = centerZ;

        xy_points->InsertNextPoint(x, y, z);

        // ���������ĵ�Բ�ܵ���
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, 0); // ���ĵ�
        line->GetPointIds()->SetId(1, i + 1); // Բ�ܵ�
        xy_lines->InsertNextCell(line);
    }

    // ����ͬ��Բ
    int numCircles = 5;
    for (int circle = 1; circle <= numCircles; circle++) {
        double radius = maxRadius * circle / numCircles;
        int startIndex = xy_points->GetNumberOfPoints();

        // ����Բ�ܵ�
        for (int i = 0; i <= radialDivisions; i++) {
            double angle = 2.0 * vtkMath::Pi() * i / radialDivisions;
            double x = centerX + radius * cos(angle);
            double y = centerY + radius * sin(angle);
            double z = centerZ;

            xy_points->InsertNextPoint(x, y, z);
        }
       
        // ����Բ����
        for (int i = 0; i < radialDivisions; i++) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, startIndex + i);
            line->GetPointIds()->SetId(1, startIndex + i + 1);
            xy_lines->InsertNextCell(line);
        }
    }

    //addRadialScaleMarkers(xy_points, xy_lines, maxRadius, radialDivisions, numCircles);

    xy_gridData->SetPoints(xy_points);
    xy_gridData->SetLines(xy_lines);

    xy_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    xy_mapper->SetInputData(xy_gridData);

    m_xyplane_actor = vtkSmartPointer<vtkActor>::New();
    m_xyplane_actor->SetMapper(xy_mapper);
    m_xyplane_actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // ��ɫ
    m_xyplane_actor->GetProperty()->SetLineWidth(2);

    m_renderer->AddActor(m_xyplane_actor);

    updateXYCandelaLabels(maxRadius, numCircles);
}
void IESPointCloudWidget::addCoordinateAxes() 
{
    // �����򵥵�������
    axisPoints = vtkSmartPointer<vtkPoints>::New();
    axisLines = vtkSmartPointer<vtkCellArray>::New();
    axisColors = vtkSmartPointer<vtkUnsignedCharArray>::New();

    axisColors->SetNumberOfComponents(3);

    // X�� (��ɫ)
    axisPoints->InsertNextPoint(0, 0, 0);
    axisPoints->InsertNextPoint(2, 0, 0);
    vtkIdType xAxis[2] = { 0, 1 };
    axisLines->InsertNextCell(2, xAxis);
    axisColors->InsertNextTuple3(255, 0, 0);

    // Y�� (��ɫ)
    axisPoints->InsertNextPoint(0, 0, 0);
    axisPoints->InsertNextPoint(0, 2, 0);
    vtkIdType yAxis[2] = { 2, 3 };
    axisLines->InsertNextCell(2, yAxis);
    axisColors->InsertNextTuple3(0, 255, 0);

    // Z�� (��ɫ)
    axisPoints->InsertNextPoint(0, 0, 0);
    axisPoints->InsertNextPoint(0, 0, 2);
    vtkIdType zAxis[2] = { 4, 5 };
    axisLines->InsertNextCell(2, zAxis);
    axisColors->InsertNextTuple3(0, 0, 255);


    axisData = vtkSmartPointer<vtkPolyData>::New();
    axisData->SetPoints(axisPoints);
    axisData->SetLines(axisLines);
    axisData->GetCellData()->SetScalars(axisColors);

    axisMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    axisMapper->SetInputData(axisData);

    axisActor = vtkSmartPointer<vtkActor>::New();
    axisActor->SetMapper(axisMapper);
    axisActor->GetProperty()->SetLineWidth(2);
    m_renderer->AddActor(axisActor);
}

void IESPointCloudWidget::FillColorData()
{
    clearPoints();
    m_points->SetNumberOfPoints(IESLoader::instance().coords.size());
    m_intensities->SetNumberOfValues(IESLoader::instance().coords.size());

    double max_x = -INFINITY, max_y = -INFINITY, max_z = -INFINITY;
    for (long i = 0; i < IESLoader::instance().coords.size(); i++)
    {
        double t = IESLoader::instance().tflat[i];
        double p = IESLoader::instance().pflat[i];
        //double r = IESLoader::instance().intensity[i];
        QVector3D v = IESLoader::instance().polar_to_cartesian(t, p, 1);
        double intensity = IESLoader::instance().intensity[i];
        m_points->SetPoint(i, v.x(), v.y(), v.z());
        m_intensities->SetValue(i, intensity);
        if (v.x() > max_x)max_x = v.x();
        if (v.y() > max_y)max_y = v.y();
        if (v.z() > max_z)max_z = v.z();
    }

    createXZPlaneGrid(max_y + 0.5, 12);
    createYZPlaneGrid(max_x + 0.5, 12);
    createXYPlaneGrid(max_z + 0.5, 12);
    createAxisArrow(max_z + 0.5);

    structuredGrid->SetDimensions(361, 181, 1);
    // ������ɫӳ�䷶Χ
    double range[2];
    m_intensities->GetRange(range);
    m_mapper->SetScalarRange(range);

    // ������ɫӳ�亯�� (��-��-��)
    m_colorTransferFunction->RemoveAllPoints();
    m_colorTransferFunction->AddRGBPoint(range[0], 0.0, 0.0, 1.0); // ��ɫ�����ǿ��
    m_colorTransferFunction->AddRGBPoint((range[0] + range[1]) / 2, 0.0, 1.0, 0.0); // ��ɫ������ǿ��
    m_colorTransferFunction->AddRGBPoint(range[1], 1.0, 0.0, 0.0); // ��ɫ�����ǿ��


    m_scalarBarActor->SetLookupTable(m_colorTransferFunction);
    m_scalarBarActor->SetTitle("Intensity");

    // ����ɫ��λ�úʹ�С (�Ҳഹֱ����)
    m_scalarBarActor->SetPosition(0.85, 0.1);    // λ�� (�������)
    m_scalarBarActor->SetWidth(0.1);             // ���
    m_scalarBarActor->SetHeight(0.8);            // �߶�
        // ����ɫ�귽��Ϊ��ֱ

    m_scalarBarActor->SetOrientationToVertical();

    m_renderer->AddActor(m_scalarBarActor);
}
void IESPointCloudWidget::FillShapeData()
{
    clearPoints();
    m_points->SetNumberOfPoints(IESLoader::instance().coords.size());
    //m_intensities->SetNumberOfValues(IESLoader::instance().coords.size());
    double max_x=-INFINITY, max_y = -INFINITY, max_z = -INFINITY;
    for (long i = 0; i < IESLoader::instance().coords.size(); i++)
    {
        double t = IESLoader::instance().tflat[i];
        double p = IESLoader::instance().pflat[i];
        double r = IESLoader::instance().intensity[i];
        QVector3D v = IESLoader::instance().polar_to_cartesian(t, p, r);
        double intensity = IESLoader::instance().intensity[i];
        m_points->SetPoint(i, v.x(), v.y(), v.z());
        if (v.x() > max_x)max_x = v.x();
        if (v.y() > max_y)max_y = v.y();
        if (intensity > max_z)max_z = intensity;
        //m_intensities->SetValue(i, v.z());
    }
 
    // ������ɫӳ�䷶Χ
    double range[2];
    //m_intensities->GetRange(range);
    //m_mapper->SetScalarRange(range);

    createXZPlaneGrid(max_z, 12);
    createYZPlaneGrid(max_z, 12);
    createXYPlaneGrid(max_z, 12);
    createAxisArrow(max_z );
    // ������ɫӳ�亯�� (��-��-��)
    //m_colorTransferFunction->RemoveAllPoints();
    //m_colorTransferFunction->AddRGBPoint(range[0], 0.0, 0.0, 1.0); // ��ɫ�����ǿ��
    //m_colorTransferFunction->AddRGBPoint((range[0] + range[1]) / 2, 0.0, 1.0, 0.0); // ��ɫ������ǿ��
    //m_colorTransferFunction->AddRGBPoint(range[1], 1.0, 0.0, 0.0); // ��ɫ�����ǿ��


    //m_scalarBarActor->SetLookupTable(m_colorTransferFunction);
    //m_scalarBarActor->SetTitle("Intensity");

    //// ����ɫ��λ�úʹ�С (�Ҳഹֱ����)
    //m_scalarBarActor->SetPosition(0.85, 0.1);    // λ�� (�������)
    //m_scalarBarActor->SetWidth(0.1);             // ���
    //m_scalarBarActor->SetHeight(0.8);            // �߶�
    //    // ����ɫ�귽��Ϊ��ֱ
    
    //m_scalarBarActor->SetOrientationToVertical();
    m_renderer->RemoveActor(m_scalarBarActor);
}

void IESPointCloudWidget::updateIESDataMesh(bool value)
{
    if (!value)return;
}

void IESPointCloudWidget::on_chkOXY_stateChanged(int value)
{
    if (!IESLoader::instance().light.candela.size() > 0)
        return;
    if (value)
    {
        m_xyplane_actor->SetVisibility(true);
        for (auto& label : m_xy_candelaLabels) 
        {
            label->SetVisibility(true);
        }

        //m_renderer->AddActor(m_xyplane_actor);
    }
    else
    {
        m_xyplane_actor->SetVisibility(false);
        for (auto& label : m_xy_candelaLabels) 
        {
            label->SetVisibility(false);
        }
        //m_renderer->RemoveActor(m_xyplane_actor);
    }

    this->renderWindow()->Render();
}
void IESPointCloudWidget::on_chkOXZ_stateChanged(int value)
{
    if (!IESLoader::instance().light.candela.size() > 0)
        return;
    if (value)
    {
        m_xzplane_actor->SetVisibility(true);
        for (auto& label : m_xz_candelaLabels) 
        {
            label->SetVisibility(true);
        }
        //m_renderer->AddActor(m_xzplane_actor);
    }
    else
    {
        m_xzplane_actor->SetVisibility(false);
        for (auto& label : m_xz_candelaLabels) 
        {
            label->SetVisibility(false);
        }
        //m_renderer->RemoveActor(m_xzplane_actor);
    } 
    this->renderWindow()->Render();
}
void IESPointCloudWidget::on_chkOYZ_stateChanged(int value)
{
    if (!IESLoader::instance().light.candela.size() > 0)
        return;
    if (value)
    {
        m_yzplane_actor->SetVisibility(true);
        for (auto& label : m_yz_candelaLabels) 
        {
            label->SetVisibility(true);
        }
        //m_renderer->AddActor(m_yzplane_actor);
    }
    else
    {
        m_yzplane_actor->SetVisibility(false);
        for (auto& label : m_yz_candelaLabels) 
        {
            label->SetVisibility(false);
        }
        //m_renderer->RemoveActor(m_yzplane_actor);
    } 
    this->renderWindow()->Render();
}
void IESPointCloudWidget::on_chkAxis_stateChanged(int value)
{
    if (!IESLoader::instance().light.candela.size() > 0)
        return;
    if (value)
    {
        m_xArrow_actor->SetVisibility(true);
        m_yArrow_actor->SetVisibility(true);
        m_zArrow_actor->SetVisibility(true);
        m_xArrow_textActor->SetVisibility(true);
        m_yArrow_textActor->SetVisibility(true);
        m_zArrow_textActor->SetVisibility(true);
    }
    else
    {
        m_xArrow_actor->SetVisibility(false);
        m_yArrow_actor->SetVisibility(false);
        m_zArrow_actor->SetVisibility(false);
        m_xArrow_textActor->SetVisibility(false);
        m_yArrow_textActor->SetVisibility(false);
        m_zArrow_textActor->SetVisibility(false);
    }
    this->renderWindow()->Render();
}