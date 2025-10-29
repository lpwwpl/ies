#ifndef POLARDIALOG_H
#define POLARDIALOG_H

#include <QDialog>

class IESPolarWidget;

namespace Ui {
class PolarDialog;
}

class PolarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PolarDialog(QWidget *parent = nullptr);
    ~PolarDialog();
    void updateIES();

public Q_SLOTS:
    void on_chkFillBlue_stateChanged(int value);
    void on_chkFillRed_stateChanged(int);
    void on_chkViewRed_stateChanged(int);

    void on_chkFillGreen_stateChanged(int);
    void on_chkFillYellow_stateChanged(int);
    void on_chkViewYellow_stateChanged(int);

private:
    Ui::PolarDialog*ui;

    IESPolarWidget* m_polarWidget;
};

#endif // ISOLUXDIALOG_H
