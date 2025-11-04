#ifndef NEWFILE_H
#define NEWFILE_H

#include <QDialog>
namespace Ui {
    class newFile;
}
enum EIES_VType;

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget* parent = nullptr);
    ~NewFileDialog();
    void init();

public Q_SLOTS:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void on_rbTypeC_toggled();
    void on_rbTypeB_toggled();
    void on_rbTypeA_toggled();

Q_SIGNALS:
    //horizal
    void signalCreateTypeC_0(QString version,EIES_VType);
    void signalCreateTypeC_0_90(QString version, EIES_VType);
    void signalCreateTypeC_0_180(QString version, EIES_VType);
    void signalCreateTypeC_90_270(QString version, EIES_VType);
    void signalCreateTypeC_0_360(QString version, EIES_VType);

    void signalCreateTypeB_m90_p90(QString version, EIES_VType);
    void signalCreateTypeB_0_90(QString version, EIES_VType);

    void signalCreateTypeA_m90_p90(QString version, EIES_VType);
    void signalCreateTypeA_0_90(QString version, EIES_VType);

protected:
    Ui::newFile* ui;
};
#endif // ISOLUXDIALOG_H
