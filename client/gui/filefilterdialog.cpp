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
            // 获取日期
            QString minDateString = ui->minDateWidget->dateLineEdit->text();
            QString maxDateString = ui->maxDateWidget->dateLineEdit->text();

            if(minDateString.isEmpty())
            {
                // 设置默认最小日期
                minDateString = "0000-01-01";
            }

            if(maxDateString.isEmpty())
            {
                // 设置默认最大日期
                maxDateString = "9999-12-31";
            }

            QDate date1 = QDate::fromString(minDateString, "yyyy-MM-dd");
            QDate date2 = QDate::fromString(maxDateString, "yyyy-MM-dd");

            if(!date1.isValid())
            {
                date1 = QDate(1, 1, 1); // 设置默认最小日期
            }

            if(!date2.isValid())
            {
                date2 = QDate(9999, 12, 31); // 设置默认最大日期
            }

            if(date1 > date2)
            {
                QMessageBox::critical(this, "日期输入有误", "最小日期要小于最大日期！");
                date1 = QDate(1, 1, 1); // 重置为默认最小日期
                date2 = QDate(9999, 12, 31); // 重置为默认最大日期
            }

            qDebug() << date1 << date2;
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
