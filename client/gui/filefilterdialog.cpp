#include "filefilterdialog.h"
#include "ui_filefilterdialog.h"

FileFilterDialog::FileFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileFilterDialog)
{
    ui->setupUi(this);
    sizeChk = dateChk = reChk = typeChk = false;

    ui->minSizeSpinBox->setEnabled(false);
    ui->maxSizeSpinBox->setEnabled(false);
    ui->maxDateWidget->setEnabled(false);
    ui->minDateWidget->setEnabled(false);
    ui->typeFilterWidget->setEnabled(false);
    ui->reLineEdit->setEnabled(false);

    connectInit();

}

FileFilterDialog::~FileFilterDialog()
{
    delete ui;
}

void FileFilterDialog::connectInit()
{
    connect(ui->sizeChkBox, &QCheckBox::stateChanged, this, [=](){
        bool sizeChecked = ui->sizeChkBox->isChecked();
        if(sizeChecked){
            ui->minSizeSpinBox->setEnabled(true);
            ui->maxSizeSpinBox->setEnabled(true);
        }else{
            ui->minSizeSpinBox->setEnabled(false);
            ui->maxSizeSpinBox->setEnabled(false);
        }
    });


    connect(ui->dateChkBox, &QCheckBox::stateChanged, this, [=](){
        bool dateChecked = ui->dateChkBox->isChecked();
        if(dateChecked){
            ui->minDateWidget->setEnabled(true);
            ui->maxDateWidget->setEnabled(true);
        }else{
            ui->minDateWidget->setEnabled(false);
            ui->maxDateWidget->setEnabled(false);
        }
    });

    connect(ui->reChkBox, &QCheckBox::stateChanged, this, [=](){
        bool reChecked = ui->reChkBox->isChecked();
        if(reChecked){
            ui->reLineEdit->setEnabled(true);
        }else{
            ui->reLineEdit->setEnabled(false);
        }
    });

    connect(ui->typeChkBox, &QCheckBox::stateChanged, this, [=](){
        bool typeChecked = ui->typeChkBox->isChecked();
        if(typeChecked){
            ui->typeFilterWidget->setEnabled(true);
        }else{
            ui->typeFilterWidget->setEnabled(false);
        }
    });
}
