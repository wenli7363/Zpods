#include "filefilterdialog.h"
#include "ui_filefilterdialog.h"

FileFilterDialog::FileFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileFilterDialog)
{
    ui->setupUi(this);
    sizeChk = dateChk = reChk = typeChk = false;

}

FileFilterDialog::~FileFilterDialog()
{
    delete ui;
}

void FileFilterDialog::connectInit()
{
    connect(ui->sizeChkBox,)
}
