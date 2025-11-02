#include "newfiledialog.h"
#include "ui_newfile.h"

NewFileDialog::NewFileDialog(QWidget* parent) : QDialog(parent)
, ui(new Ui::newFile)
{
    ui->setupUi(this);


}

NewFileDialog::~NewFileDialog()
{

}