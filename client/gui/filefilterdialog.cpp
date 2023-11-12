#include "filefilterdialog.h"
#include "ui_filefilterdialog.h"
#include <QDebug>

std::map<QString, FileType> fileTypeMap = {
    {"none", FileType::none},
    {"symlink", FileType::symlink},
    {"regular", FileType::regular},
    {"directory", FileType::directory},
    {"unknown",FileType::unknown},
    {"character",FileType::character},
    {"block",FileType::block},
    {"fifo",FileType::fifo},
    {"socket",FileType::socket},
};

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
            sizeChk = true;
            ui->minSizeSpinBox->setEnabled(true);
            ui->maxSizeSpinBox->setEnabled(true);
        }else{
            sizeChk = false;
            ui->minSizeSpinBox->setEnabled(false);
            ui->maxSizeSpinBox->setEnabled(false);
        }
    });

    connect(ui->dateChkBox, &QCheckBox::stateChanged, this, [=](){
        bool dateChecked = ui->dateChkBox->isChecked();
        if(dateChecked){
            dateChk = true;
            ui->minDateWidget->setEnabled(true);
            ui->maxDateWidget->setEnabled(true);
        }else{
            dateChk = false;
            ui->minDateWidget->setEnabled(false);
            ui->maxDateWidget->setEnabled(false);
        }
    });

    connect(ui->reChkBox, &QCheckBox::stateChanged, this, [=](){
        bool reChecked = ui->reChkBox->isChecked();
        if(reChecked){
            reChk = true;
            ui->reLineEdit->setEnabled(true);
        }else{
            reChk = false;
            ui->reLineEdit->setEnabled(false);
        }
    });

    connect(ui->typeChkBox, &QCheckBox::stateChanged, this, [=](){
        bool typeChecked = ui->typeChkBox->isChecked();
        if(typeChecked){
            typeChk = true;
            ui->typeFilterWidget->setEnabled(true);
        }else{
            typeChk = false;
            ui->typeFilterWidget->setEnabled(false);
        }
    });
}


void FileFilterDialog::closeEvent(QCloseEvent *event)
{
    // 保存所有的配置信息
    FilterConfig* config = new FilterConfig;

    {
        config->dateChk = dateChk;
        config->sizeChk = sizeChk;
        config->typeChk = typeChk;
        config->reChk = reChk;

        if(dateChk)
        {
            config->min_date = ui->minDateWidget->dateLineEdit->text().toStdString();
            config->max_date = ui->maxDateWidget->dateLineEdit->text().toStdString();
        }

        if(sizeChk)
        {
            config->minSize = ui->minSizeSpinBox->value();
            config->maxSize = ui->maxSizeSpinBox->value();
        }

        if(typeChk)
        {
            QStringList qTypeTmpList = ui->typeFilterWidget->currentText();
            if(!qTypeTmpList.isEmpty())
            {
                auto qTypeList = qTypeTmpList[0].split(";");
                for (const QString& qType : qTypeList) {
                    FileType fileType = fileTypeMap[qType];
                    config->types.insert(fileType);
                }
            }else{
                config->typeChk = false;
            }
        }


//        if(reChk)
//        {

//        }
    }


    // 在对话框关闭时发射sentFilterConfig信号
    emit sentFilterConfig(config);
    delete config;
    // 调用父类的关闭事件处理函数
    QDialog::closeEvent(event);
}
