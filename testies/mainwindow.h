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

    void populateTableFromIESData();
    void setupHeaderEditing();
    std::vector<double> getVerticalAngles() const;
    std::vector<double> getHorizontalAngles() const;
    std::vector<std::vector<double>> getIntensityData() const;

    void setTableData(const std::vector<double>& horizontalAngles,
        const std::vector<double>& verticalAngles,
        const std::vector<std::vector<double>>& intensityData);
    void rearrangeTableDataAfterHorizontalSort(const std::vector<int>& newColumnOrder);
    void sortHorizontalAngles();
    void sortVerticalAngles();
    void rearrangeTableDataAfterVerticalSort(const std::vector<int>& newRowOrder);

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
    void addRow();
    void deleteRow();
    void addColumn();
    void deleteColumn();
    void slotDoubleValueChanged();
    void slot_heditor();
    void slot_veditor();
    void enableAutoSort(bool enable);
    void sortTable(bool);
    void showContextMenu(const QPoint& pos);
protected:
    void moveEvent(QMoveEvent* event);
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
    Ui::MainWindow* ui;
    QTranslator* translator;

    QVector<QString> rowClipboard;
    QVector<QString> columnClipboard;
    QString horizontalHeaderClipboard;
    QString verticalHeaderClipboard;

    ISOLuxDialog* m_isoDialog;
    PolarDialog* m_polarDialog;
    ThreeDDialog* m_3dDialog;

    QLineEdit* m_heditor;
    QLineEdit* m_veditor;
    int logicalIndex;

    QString m_filePath;
    QAction* autoSortAction;
    QAction* sortAction;
    bool autoSortEnabled;
    QString m_editor_old_value;
    bool m_editor_focusOut;
};
#endif // MAINWINDOW_H
