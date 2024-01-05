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

std::map<QString,int> unitMap =
{
    {"B",1},
    {"KB",1024},
    {"MB",1024*1024},
    {"GB",1024*1024*1024},
};

FileFilterDialog::FileFilterDialog(QWidget *parent) :
    QDialog(parent),
    sizeChk(false), reChk(false), dateChk(false), typeChk(false),
    ui(new Ui::FileFilterDialog)
{
    ui->setupUi(this);

    ui->minSizeSpinBox->setEnabled(false);
    ui->maxSizeSpinBox->setEnabled(false);
    ui->maxDateWidget->setEnabled(false);
    ui->minDateWidget->setEnabled(false);
    ui->typeFilterWidget->setEnabled(false);
    ui->reLineEdit->setEnabled(false);
    ui->rePushButton->setEnabled(false);
    ui->minComboBox->setEnabled(false);
    ui->maxComboBox->setEnabled(false);


    QStringList unit = {"B","KB","MB","GB"};
    ui->minComboBox->addItems(unit);
    ui->maxComboBox->addItems(unit);

    connectInit();
}

FileFilterDialog::~FileFilterDialog()
{
    delete ui;
}

void FileFilterDialog::connectInit()
{
    connect(ui->sizeChkBox, &QCheckBox::stateChanged, this, [this](){
        bool sizeChecked = ui->sizeChkBox->isChecked();
        if(sizeChecked){
            sizeChk = true;
            ui->minSizeSpinBox->setEnabled(true);
            ui->maxSizeSpinBox->setEnabled(true);
            ui->minComboBox->setEnabled(true);
            ui->maxComboBox->setEnabled(true);
        }else{
            sizeChk = false;
            ui->minSizeSpinBox->setEnabled(false);
            ui->maxSizeSpinBox->setEnabled(false);
            ui->minComboBox->setEnabled(false);
            ui->maxComboBox->setEnabled(false);
        }
    });

    connect(ui->dateChkBox, &QCheckBox::stateChanged, this, [this](){
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

    connect(ui->reChkBox, &QCheckBox::stateChanged, this, [this](){
        bool reChecked = ui->reChkBox->isChecked();
        if(reChecked){
            reTmpList.clear();
            reChk = true;
            ui->reLineEdit->setEnabled(true);
            ui->rePushButton->setEnabled(true);
        }else{
            reChk = false;
            ui->reLineEdit->setEnabled(false);
            ui->rePushButton->setEnabled(false);
        }
    });

    connect(ui->typeChkBox, &QCheckBox::stateChanged, this, [this](){
        bool typeChecked = ui->typeChkBox->isChecked();
        if(typeChecked){
            typeChk = true;
            ui->typeFilterWidget->setEnabled(true);
        }else{
            typeChk = false;
            ui->typeFilterWidget->setEnabled(false);
        }
    });

    connect(ui->rePushButton,&QPushButton::clicked,this,[this]()
    {
        QString reInput = ui->reLineEdit->text();
        if(!reInput.isEmpty())
        {
            QRegularExpression regex(reInput);

            if(regex.isValid()){
                reTmpList.push_back(reInput.toStdString());
            }else{
                QMessageBox::critical(this,"错误","无效的正则表达式，请重新输入！");
            }
            ui->reLineEdit->clear();
        }
    });
}


void FileFilterDialog::closeEvent(QCloseEvent *event)
{
    // 保存所有的配置信息
    std::shared_ptr<FilterConfig> config = std::make_shared<FilterConfig>();

    {
        config->dateChk = dateChk;
        config->sizeChk = sizeChk;
        config->typeChk = typeChk;
        config->reChk = reChk;

        if(dateChk)
        {
            // 获取日期
            QString minDateQString = ui->minDateWidget->dateLineEdit->text();
            QString maxDateQString = ui->maxDateWidget->dateLineEdit->text();

            QDate date1 = QDate::fromString(minDateQString, "yyyy-MM-dd");
            QDate date2 = QDate::fromString(maxDateQString, "yyyy-MM-dd");

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
                QMessageBox::critical(this, "日期输入有误", "最小日期要小于最大日期！设置成默认值了！");
                date1 = QDate(1, 1, 1); // 重置为默认最小日期
                date2 = QDate(9999, 12, 31); // 重置为默认最大日期
            }

            std::string minDateStd = date1.toString("yyyy-MM-dd").toStdString();
            let year1 = std::stoi(minDateStd.substr(0, 4));
            let month1 = std::stoi(minDateStd.substr(5, 2));
            let day1 = std::stoi(minDateStd.substr(8, 2));
            config->min_date = zpods::fs::make_year_month_day(year1, month1, day1);


            std::string maxDateStd = date2.toString("yyyy-MM-dd").toStdString();
            let year2 = std::stoi(maxDateStd.substr(0, 4));
            let month2 = std::stoi(maxDateStd.substr(5, 2));
            let day2 = std::stoi(maxDateStd.substr(8, 2));
            config->max_date= zpods::fs::make_year_month_day(year2, month2, day2);
        }

        if(sizeChk)
        {
            int min = ui->minSizeSpinBox->value();
            int max = ui->maxSizeSpinBox->value();

            QString minUnit = ui->minComboBox->currentText();
            QString maxUnit = ui->maxComboBox->currentText();

            config->minSize = min * unitMap[minUnit];
            config->maxSize = max * unitMap[maxUnit];

            if(config->minSize>config->maxSize)
            {
                QMessageBox::critical(this,"大小输入有误","大小输入有误，设为默认值了");
                config->minSize = 0;
                config->maxSize = (uintmax_t)-1;
            }
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


        if(reChk)
        {
            config->re_list = reTmpList;
        }
    }


    // 在对话框关闭时发射sentFilterConfig信号
    emit sentFilterConfig(config);
    // 调用父类的关闭事件处理函数
    QDialog::closeEvent(event);
}
