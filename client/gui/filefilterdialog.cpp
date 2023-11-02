#include "filefilterdialog.h"
#include "ui_filefilterdialog.h"

FileFilterDialog::FileFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileFilterDialog)
{
    ui->setupUi(this);
}

FileFilterDialog::~FileFilterDialog()
{
    delete ui;
}
