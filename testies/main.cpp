#include "MainWindow.h"
#include <QApplication>
#include <QSurfaceFormat>
//#include <Quarter/Quarter.h>
#include <vtkOutputWindow.h>
#include <cmath> 
//#include <matplot/matplot.h> 


//int main()
//{
//    using namespace matplot;
//    auto [X, Y] = meshgrid(iota(-8, .5, +8));
//    auto Z = transform(X, Y, [](double x, double y)
//        { double eps = std::nextafter(0.0, 1.0);
//    double R = sqrt(pow(x, 2) + pow(y, 2)) + eps;
//    return sin(R) / R; });
//    mesh(X, Y, Z);
//    show();// ��Ҫ����show������ʾ return 0; 
//}
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ����OpenGL�汾������
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    vtkOutputWindow::SetGlobalWarningDisplay(0); // �رվ�����ʾ
    // ��������ʾ������
    MainWindow window;
    window.show();

    return  app.exec();
}
