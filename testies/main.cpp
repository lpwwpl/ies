#include "MainWindow.h"
#include <QApplication>
#include <QSurfaceFormat>
//#include <Quarter/Quarter.h>
#include <vtkOutputWindow.h>
#include <cmath> 
#include <QTextCodec>

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
//    show();// 需要调用show才能显示 return 0; 
//}
void initApplication() {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef WIN32
    QApplication::setFont(QFont("Microsoft YaHei UI", 9));
#endif

    QSurfaceFormat openGLFormat;
    openGLFormat.setVersion(3, 3);
    openGLFormat.setProfile(QSurfaceFormat::CoreProfile);
    openGLFormat.setDepthBufferSize(24);
    openGLFormat.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(openGLFormat);
}
int main(int argc, char* argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    initApplication();


    QApplication app(argc, argv);

    vtkOutputWindow::SetGlobalWarningDisplay(0); // 关闭警告显示
    // 创建并显示主窗口
    MainWindow window;
    window.show();

    return  app.exec();
}
