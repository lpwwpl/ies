#ifndef NEWFILE_H
#define NEWFILE_H

#include <QDialog>
namespace Ui {
    class newFile;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget* parent = nullptr);
    ~NewFileDialog();
    

public Q_SLOTS:


private:
    Ui::newFile* ui;


};
#endif // ISOLUXDIALOG_H
