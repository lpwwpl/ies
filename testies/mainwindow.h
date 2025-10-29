#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>

#include "ui_mainwindow.h"


class ISOLuxDialog;
class PolarDialog;
class ThreeDDialog;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    template <class T>
    void createLeftDock(const QString& label)
    {
        //ct::createDock<T>(this, label, ui->cloudview, ui->cloudtree, ui->console,
        //    Qt::LeftDockWidgetArea, ui->PropertiesDock);
    }

    template <class T>
    void createRightDock(const QString& label)
    {
        //ct::createDock<T>(this, label, ui->cloudview, ui->cloudtree, ui->console,
        //    Qt::RightDockWidgetArea);
    }

    template <class T>
    void createDialog(const QString& label)
    {
        //ct::createDialog<T>(this, label, ui->cloudview, ui->cloudtree, ui->console);
    }
public Q_SLOTS:
    void on_actionIsoLuxCurves_triggered();
    void on_actionPolarCurves_triggered();
    void on_action3DCurves_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionLight_triggered();
    void on_actionDark_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

protected:
    void moveEvent(QMoveEvent* event);

private:
    Ui::MainWindow* ui;
    QTranslator* translator;

    ISOLuxDialog* m_isoDialog;
    PolarDialog* m_polarDialog;
    ThreeDDialog* m_3dDialog;
};
#endif // MAINWINDOW_H
