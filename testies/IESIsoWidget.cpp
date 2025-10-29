#include "IESIsoWidget.h"
#include "contours.h"
#include <QDebug>

#include "IESLoader.h"

IESIsoWidget::IESIsoWidget()
{
    Init();
}

IESIsoWidget::~IESIsoWidget()
{

}

void IESIsoWidget::Init()
{
    calculationWidth = 20;
    fixtureX = 0;
    fixtureY = 0;
    fixtureZ = 0;
    gridSpacing = 0.1;
    m_levelSize = 5;
    //setupColorSchemes();
    m_colorScale= new QCPColorScale(this);
    

    addLayer("gridLayer", nullptr, QCustomPlot::limAbove); // ����ӵ�Ĭ��ͼ���Ϸ�
    layer("gridLayer")->setMode(QCPLayer::lmBuffered); // �ؼ�������Ϊ����ģʽ

    m_margin = new QCPMarginGroup(this);
    axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, m_margin);
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plotLayout()->addElement(0, 1, m_colorScale);
}

//void IESIsoWidget::setupColorSchemes()
//{
//    // ���������ɫ����
//    colorSchemes.clear();
//
//    // ����1: �ʺ�ɫ
//    ColorScheme rainbow;
//    rainbow.name = "�ʺ�ɫ";
//    rainbow.colors = {
//        QColor(148, 0, 211),   // ��ɫ
//        QColor(75, 0, 130),    // ����ɫ
//        QColor(0, 0, 255),     // ��ɫ
//        QColor(0, 255, 0),     // ��ɫ
//        QColor(255, 255, 0),   // ��ɫ
//        QColor(255, 127, 0),   // ��ɫ
//        QColor(255, 0, 0)      // ��ɫ
//    };
//    colorSchemes.push_back(rainbow);
//
//    // ����2: ����ͼ
//    ColorScheme heatmap;
//    heatmap.name = "����ͼ";
//    heatmap.colors = {
//        QColor(0, 0, 0),       // ��ɫ
//        QColor(128, 0, 0),     // ���ɫ
//        QColor(255, 0, 0),     // ��ɫ
//        QColor(255, 128, 0),   // ��ɫ
//        QColor(255, 255, 0),   // ��ɫ
//        QColor(255, 255, 128), // ǳ��ɫ
//        QColor(255, 255, 255)  // ��ɫ
//    };
//    colorSchemes.push_back(heatmap);
//
//    // ����3: ��ɫ��
//    ColorScheme cool;
//    cool.name = "��ɫ��";
//    cool.colors = {
//        QColor(255, 255, 255), // ��ɫ
//        QColor(200, 230, 255), // ǳ��ɫ
//        QColor(150, 200, 255), // ����ɫ
//        QColor(100, 150, 255), // ��ɫ
//        QColor(50, 100, 200),  // ����ɫ
//        QColor(0, 50, 150),    // ����ɫ
//        QColor(0, 0, 100)      // ������
//    };
//    colorSchemes.push_back(cool);
//
//    // ����4: ����ͼ
//    ColorScheme terrain;
//    terrain.name = "����ͼ";
//    terrain.colors = {
//        QColor(0, 0, 70),      // ����ɫ
//        QColor(0, 0, 255),     // ��ɫ
//        QColor(0, 128, 255),   // ǳ��ɫ
//        QColor(240, 240, 64),  // ��ɫ
//        QColor(32, 160, 0),    // ��ɫ
//        QColor(0, 96, 0),      // ����ɫ
//        QColor(128, 128, 128)  // ��ɫ
//    };
//    colorSchemes.push_back(terrain);
//
//    // ����5: �Զ�������ɫ
//    ColorScheme vibrant;
//    vibrant.name = "����ɫ";
//    vibrant.colors = {
//        QColor(255, 0, 0),     // ��ɫ
//        QColor(255, 128, 0),   // ��ɫ
//        QColor(255, 255, 0),   // ��ɫ
//        QColor(0, 255, 0),     // ��ɫ
//        QColor(0, 255, 255),   // ��ɫ
//        QColor(0, 128, 255),   // ��ɫ
//        QColor(128, 0, 255)    // ��ɫ
//    };
//    colorSchemes.push_back(vibrant);
//
//    //// ��ӵ�������
//    //colorSchemeComboBox->clear();
//    //for (const auto& scheme : colorSchemes) {
//    //    colorSchemeComboBox->addItem(scheme.name);
//    //}
//}

void IESIsoWidget::updatePlot()
{
    clearPlottables();
    clearItems();
    isoluxCurves.clear();
    illuminanceLevels.clear();


    updateColorMapData();

    double halfRange = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;


    // Ϊÿ����ֵ�ߴ���IsoluxCurve
    std::vector<QColor> colors;
    double dist = maxIlluminance - minIlluminance;
 
    double step = dist/m_levelSize;
    for (int i = 1; i < m_levelSize; i++)
    {
        illuminanceLevels.push_back(minIlluminance + i* step);
    }

    //if (m_levelSize <= static_cast<int>(src_colors.size())) {
    //    // �����Ҫ����ɫ����С�ڵ��ڷ����е���ɫ������ֱ��ȡǰ�����ɫ
    //    for (int i = 0; i < m_levelSize; ++i) {
    //        colors.push_back(src_colors[i]);
    //    }
    //}
    //else 
    //{
    //    int baseColors = static_cast<int>(src_colors.size());
    //    for (int i = 0; i < m_levelSize; ++i)
    //    {
    //        double t = static_cast<double>(i) / (m_levelSize - 1);
    //        double baseIndex = t * (baseColors - 1);
    //        int index1 = static_cast<int>(std::floor(baseIndex));
    //        int index2 = std::min(index1 + 1, baseColors - 1);
    //        double alpha = baseIndex - index1;

    //        QColor color1 = src_colors[index1];
    //        QColor color2 = src_colors[index2];

    //        int r = static_cast<int>(color1.red() * (1 - alpha) + color2.red() * alpha);
    //        int g = static_cast<int>(color1.green() * (1 - alpha) + color2.green() * alpha);
    //        int b = static_cast<int>(color1.blue() * (1 - alpha) + color2.blue() * alpha);

    //        colors.push_back(QColor(r, g, b));
    //    }
    //}


    MarchingSquares contourPlot(illuminanceGriddata, -halfRange, halfRange, -halfRange, halfRange);

    vector<MarchingSquares::levelPaths> contours = contourPlot.mkIsos(illuminanceLevels);

    QCPCurve* base;
    for (int l = 0; l < contours.size(); l++) {
        for (int j = 0; j < contours[l].size(); j++) {
            base = new QCPCurve(xAxis, yAxis);
            QVector<double> x, y;
            //base->setPen(QPen(colors[l]));
            base->setPen(QPen(Qt::black));
            //base->setBrush(QBrush(colors[l]));
            if (contours[l][j].size() > 4) 
            {
                for (int i = 0; i < contours[l][j].size(); i++)
                {
                    x.push_back(contours[l][j][i].x());
                    y.push_back(contours[l][j][i].y());
                }
            }
            base->setData(y, x);
        }
    }

    // ���������᷶Χ
    xAxis->setRange(-halfRange, halfRange);
    yAxis->setRange(-halfRange, halfRange);

    //��ʾ������
    if (m_bUseGrid)
    {
        xAxis->grid()->setLayer("gridLayer");
        yAxis->grid()->setLayer("gridLayer");

        xAxis->grid()->setSubGridVisible(true); // ȷ���������߿ɼ�
        yAxis->grid()->setSubGridVisible(true);
        xAxis->grid()->setLayer("gridLayer");
        yAxis->grid()->setLayer("gridLayer");

        moveLayer(layer("gridLayer"), layer("main"));
    }
    else
    {
        moveLayer(layer("main"),layer("gridLayer"));
        //removeLayer(layer("gridLayer"));
    }
    this->replot();
}

void IESIsoWidget::calculateXZPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;

    double halfRange = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    for (int i = 0; i <= gridSize; ++i) {
        std::vector<double> oneLine;
        double x = -halfRange + i * gridSpacing;
        for (int j = 0; j <= gridSize; ++j) {
            double z = -halfRange + j * gridSpacing;
            double y = 0;  // XYƽ��Ĺ̶��߶�

            double illuminance = calculateIlluminanceAtPoint(x,0,  z);

            IlluminancePoint point;
            point.x = x;
            point.y = y;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine.push_back(illuminance);
            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata.push_back(oneLine);
    }
}
void IESIsoWidget::calculateYZPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;

    double halfRange = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    for (int i = 0; i <= gridSize; ++i) {
        std::vector<double> oneLine;
        double y = -halfRange + i * gridSpacing;
        for (int j = 0; j <= gridSize; ++j) {
            double z = -halfRange + j * gridSpacing;
            double x = 0;  // YZƽ��Ĺ̶�Xλ��

            double illuminance = calculateIlluminanceAtPoint(0,y, z);

            IlluminancePoint point;
            point.x = x;
            point.y = y;
            point.z = z;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine.push_back(illuminance);
            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata.push_back(oneLine);
    }
}
double IESIsoWidget::calculateIlluminanceAtPoint(double x, double y, double z)
{
    // �������
    double dx = x - fixtureX;
    double dy = y - fixtureY;
    double dz = z - fixtureZ;
    double totalDistance = sqrt(dx * dx + dy * dy + dz * dz);

    if (totalDistance == 0) return 0;

    // ���㴹ֱ�Ƕ� (�ӵƾ�����Ϊ��)
    double verticalAngle = acos(-dz / totalDistance) * 180.0 / M_PI;  // ע�����
 
    // ����ˮƽ�Ƕ�
    double horizontalAngle = atan2(dy, dx) * 180.0 / M_PI;
    if (horizontalAngle < 0) horizontalAngle += 360;

    // ��ȡ��ǿֵ
    double candela = IESLoader::instance().getCandelaValue(verticalAngle, horizontalAngle);

    // �����ն� (����ƽ�����ȶ��� + ���Ҷ���)
    double cosIncidence = -dz / totalDistance;  // ���������
    double illuminance = candela / (totalDistance * totalDistance) * cosIncidence;

    return std::max(0.0, illuminance);
}

void IESIsoWidget::calculateXYPlaneIlluminance()
{
    illuminanceGrid.clear();
    illuminanceGriddata.clear();
    maxIlluminance = 0;
    minIlluminance = 1e9;

    gridSpacing = calculationWidth / m_numOfPoints;

    double halfWidth = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;


    for (int i = 0; i <= gridSize; ++i) {
        std::vector<double> oneLine;
        double x = -halfWidth + i * gridSpacing;
        for (int j = 0; j <= gridSize; ++j) {
            double y = -halfWidth + j * gridSpacing;

            double illuminance = calculateIlluminanceAtPoint(x, y,0);

            IlluminancePoint point;
            point.x = x;
            point.y = y;
            point.illuminance = illuminance;

            illuminanceGrid.push_back(point);
            oneLine.push_back(illuminance);
            if (illuminance > maxIlluminance) maxIlluminance = illuminance;
            if (illuminance < minIlluminance && illuminance > 0) minIlluminance = illuminance;
        }
        illuminanceGriddata.push_back(oneLine);
    }
}

void IESIsoWidget::updateColorMapData()
{
    if (illuminanceGrid.empty()) return;


    m_colorMap = new QCPColorMap(xAxis, yAxis);
    m_colorMap->setColorScale(m_colorScale);
    //m_colorMap->setGradient(QCPColorGradient::gpJet);
    QCPColorGradient gradient;
    gradient.setColorStopAt(0, QColor(0, 0, 255));     // ��ɫ
    gradient.setColorStopAt(0.25, QColor(0, 255, 255)); // ��ɫ
    gradient.setColorStopAt(0.5, QColor(0, 255, 0));   // ��ɫ
    gradient.setColorStopAt(0.75, QColor(255, 255, 0)); // ��ɫ
    gradient.setColorStopAt(1, QColor(255, 0, 0));     // ��ɫ

    m_colorMap->setGradient(gradient);
    m_colorMap->setInterpolate(true);

    gridSpacing = calculationWidth / m_numOfPoints;

    double halfRange = calculationWidth / 2.0;
    int gridSize = calculationWidth / gridSpacing;

    //QCPColorMap* colorMap = new QCPColorMap(this->xAxis, this->yAxis);
    //QCPColorMapData* data = m_colorMap->data();
    QCPColorMapData* m_colorMapData = new QCPColorMapData(gridSize, gridSize,QCPRange(-halfRange, halfRange),
        QCPRange(-halfRange, halfRange));


    double minIllu=INFINITY;
    double maxIllu=-INFINITY;
    // �������
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            int idx = i * (gridSize + 1) + j;
            if (idx < illuminanceGrid.size()) {
                m_colorMapData->setCell(i, j, illuminanceGrid[idx].illuminance);
                if (minIllu > illuminanceGrid[idx].illuminance)minIllu = illuminanceGrid[idx].illuminance;
                if (maxIllu < illuminanceGrid[idx].illuminance)maxIllu = illuminanceGrid[idx].illuminance;
            }
        }
    }

    // �������ݷ�Χ
    m_colorMap->setData(m_colorMapData);
    m_colorMap->setDataRange(QCPRange(minIllu, maxIllu));
    m_colorScale->setDataRange(QCPRange(minIllu, maxIllu));
}
void IESIsoWidget::updateIESXY(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = 0;
    fixtureZ = distance;
    calculationWidth = halfmap * 2;
    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXYPlaneIlluminance();

    updatePlot();
}

void IESIsoWidget::updateIESYZ(double distance, double halfmap)
{
    fixtureX = distance;
    fixtureY = 0;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;
    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateYZPlaneIlluminance();
   
    updatePlot();
}
void IESIsoWidget::updateIESXZ(double distance, double halfmap)
{
    fixtureX = 0;
    fixtureY = distance;
    fixtureZ = 0;
    calculationWidth = halfmap * 2;
    if (IESLoader::instance().light.candela.size() < 1)
        return;

    calculateXZPlaneIlluminance();

    updatePlot();
}