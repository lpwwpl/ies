#include "isoluxdialog.h"
#include "ui_isoluxdialog.h"
#include "IESIsoWidget.h"

//#include "./implot3d/implot3d.h"
#include "IESLoader.h"
#include <QOpenGLWidget>
ISOLuxPlot::ISOLuxPlot()/*:vtx(nullptr),itx(nullptr)*/
{ 

}

void ISOLuxPlot::updateData()
{
   

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
    }
        break;
    case ePlaneZ:
    {
        if (is2D)
        {

            m_isoWidget->m_levelSize = ui->spinLevels->value();
            m_isoWidget->updateIESXY(distance, halfMap);
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
    m_3dplot->updateData();
}
ISOLuxDialog::~ISOLuxDialog()
{
    delete ui;
}

void ISOLuxDialog::updateIES()
{
    on_cmbPlane_currentIndexChanged(ui->cmbPlane->currentIndex());
}

