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

private:
    Ui::PolarDialog*ui;

    IESPolarWidget* m_polarWidget;
};

#endif // ISOLUXDIALOG_H
