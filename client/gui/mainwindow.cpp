#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Zpods");        // set window title

    // create loginDialog
    this->loginDialog = new LoginDialog(this);
    this->loginDialog->hide();
    // set the connect function of two chkBox
    connectInit();

    this->tmpBackupOptions = new BackupOptions;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableFileFilter()
{
    // enable FileFilter when clicked checkBox
    connect(ui->filterChkBox, &QCheckBox::stateChanged, this, [&](){
        bool filterChecked = ui->filterChkBox->isChecked();
        if(filterChecked){
            tmpBackupOptions->filterChk = true;

            FileFilterDialog* filterDialog = new FileFilterDialog(this);
            connect(filterDialog,&FileFilterDialog::sentFilterConfig,this,[&](FilterConfig* filterconfig){
                this->filterConfig = filterconfig;

                if(!(filterConfig->sizeChk||filterConfig->typeChk||filterConfig->dateChk||
                     filterConfig->sizeChk||filterConfig->reChk))
                {
                    ui->filterChkBox->setCheckState(Qt::Unchecked);
                    tmpBackupOptions->filterChk = false;

                    // 释放 filterConfig 指向的内存，并将指针置为空
                    delete filterConfig;
                    filterConfig = nullptr;
                }
//                qDebug()<<"size:["<<filterconfig->minSize<<","<<filterconfig->maxSize<<"]";
//                qDebug()<<"date:["<<QString::fromStdString(filterconfig->min_date)<<","<<QString::fromStdString(filterconfig->max_date)<<"]";
//                qDebug()<<"type size: "<<filterconfig->types.size();
//                qDebug()<<"sizeChk:"<<filterconfig->sizeChk;
//                qDebug()<<"dateChk:"<<filterconfig->dateChk;
//                qDebug()<<"tpyeChk:"<<filterconfig->typeChk;
//                qDebug()<<"reChk:"<<filterconfig->reChk;
            });

            filterDialog->exec();   // 非模态
            delete filterDialog;
            filterDialog = nullptr;
        }else{
           tmpBackupOptions->filterChk = false;
        }
//        qDebug()<<"filterChkBox:"<<filterChk;
    });
}

void MainWindow::enableRemote()
{
    // enable Remote when clicked remoteCheckBox
    connect(ui->remoteChkBox, &QCheckBox::stateChanged, this, [&](){
        bool remoteChecked = ui->remoteChkBox->isChecked();
        if(remoteChecked && (!loginDialog->logined)){
           tmpBackupOptions->remoteChk = true;
           loginDialog->clearPasswordLineEdit();
           loginDialog->show();
           loginDialog->exec();
//           loginDialog->logined = true;
        }else{
            tmpBackupOptions->remoteChk = false;
//            ui->remoteChkBox->setCheckState(Qt::Unchecked);
        }
//        qDebug()<<"remoteChkBox: " <<remoteChk;
    });
}


void MainWindow::enableSrcBtn()
{
    connect(ui->srcBtn, &QPushButton::clicked, this, [=](){
        MyFileDialog *dialog =new MyFileDialog(this);
        dialog->setOption(QFileDialog::DontUseNativeDialog,true);

        //支持多选
        QListView *listView = dialog->findChild<QListView*>("listView");
        if (listView)
            listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        QTreeView *treeView = dialog->findChild<QTreeView*>();
        if (treeView)
            treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        QDialogButtonBox *button = dialog->findChild<QDialogButtonBox *>("buttonBox");

        disconnect(button,SIGNAL(accepted()),dialog,SLOT(accept()));//使链接失效
        connect(button,SIGNAL(accepted()),dialog,SLOT(slot_myAccetp()));//改成自己的槽
        if(dialog->exec()==QDialog::Accepted)
        {
            QStringList srcFileList = dialog->selectedFiles();

            ui->srcListWidget->clear();
            tmpBackupOptions->src_path_list.clear();

            for(const auto& fileName : srcFileList)
            {
                QListWidgetItem* item = new QListWidgetItem(fileName);
                ui->srcListWidget->addItem(item);

                tmpBackupOptions->src_path_list.push_back(fileName.toStdString());
            }

        }
    });
}


void MainWindow::enableTargetBtn()
{
    connect(ui->targetBtn, &QPushButton::clicked, this, [&](){
        QString targetPath = QFileDialog::getExistingDirectory(this, "选择目的路径", QDir::homePath());

        if (!targetPath.isEmpty())
        {
            ui->targetPath->setText(targetPath);
            tmpBackupOptions->target_dir = targetPath.toStdString();
        }
    });
}

void MainWindow::enableCmpsChkBox()
{
    connect(ui->cmpsChkBox, &QCheckBox::stateChanged, this, [&](){
        bool cmpsChecked = ui->cmpsChkBox->isChecked();
        if(cmpsChecked){
            tmpBackupOptions->cmpsChk = true;
        }else{
            tmpBackupOptions->cmpsChk = false;
        }
    });
}

void MainWindow::enableEncryptChkBox()
{
    connect(ui->encryptChkBox, &QCheckBox::stateChanged, this, [this](int state){
        if (state == Qt::Checked) {
            // 复选框勾选上时，弹出密码输入对话框
            QString psw = QInputDialog::getText(this, "输入密码", "请输入密码:", QLineEdit::Password);
            if (!psw.isEmpty()) {
                // 用户输入了密码，你可以在这里处理密码，例如存储到变量中
                tmpBackupOptions->password = psw.toStdString();
                tmpBackupOptions->encryptChk = true;
            } else {
                // 用户取消了输入密码，取消复选框勾选状态
                tmpBackupOptions->encryptChk = false;

                ui->encryptChkBox->setCheckState(Qt::Unchecked);
            }
        } else if (state == Qt::Unchecked) {
            // 复选框取消勾选时，取消加密
            tmpBackupOptions->encryptChk = false;
        }
    });
}


void MainWindow::enableSynChkBox()
{
    connect(ui->synChkBox, &QCheckBox::stateChanged, this, [&](){
        bool synChecked = ui->synChkBox->isChecked();
        if(synChecked){
            tmpBackupOptions->synChk = true;
        }else{
            tmpBackupOptions->synChk = false;
        }
    });
}

void MainWindow::enablePeriodBox()
{
    connect(ui->periodicWidget,&PeriodicWidget::sentPeriodOpen,this,[&](){
        tmpBackupOptions->periodChk = true;
    });

    connect(ui->periodicWidget, &PeriodicWidget::sentPeriodClose,this,[&](){
        tmpBackupOptions->periodChk = false;
    });
}

void MainWindow::connectInit()
{
    enableRemote();
    enableFileFilter();
    enableSrcBtn();
    enableTargetBtn();

    enableCmpsChkBox();
    enableEncryptChkBox();
    enableSynChkBox();
    enablePeriodBox();
    handleRegist();
    enableStartBtn();
}

void MainWindow::handleRegist()
{
    connect(this->loginDialog, &LoginDialog::sentRegist,this,[&](std::string username, std::string psw){
        user.username = username;
        user.password = psw;

        let status = user.register_();
                if (status == zpods::Status::USER_ALREADY_EXISTS) {
                    spdlog::info("user already exists!");
                } else if (status == zpods::Status::OK) {
                    spdlog::info("register succeeded!");
                } else {
                    spdlog::info("register failed!");
                }
    });
}

void MainWindow::handleBackup(BackupOptions * backupOptions)
{
    // config paths
   if (backupOptions->src_path_list.empty()) {
       QMessageBox::critical(this, "提示", "请选择要备份的文件！");
       spdlog::error("src path list is empty!");
       return;
    }

   if(backupOptions->target_dir.empty()){
       QMessageBox::critical(this,"提示","请选择要保存的路径");
       spdlog::error("target dir is empty!");
       return;
    }

// 是否指定文件名
//    QString backupName = QInputDialog::getText(this, "请输入打包文件名", "请输入打包文件名:");
//    if(backupName.isEmpty())
//    {
//        QMessageBox::critical(this, "提示", "请输入打包后的文件名！");
//        return;
//    }
//    backupOptions->config.backup_filename = backupName.toStdString();

    backupOptions->config.filter.paths = std::vector<zpods::fs::zpath>(backupOptions->src_path_list.begin(), backupOptions->src_path_list.end());

    // if use the file filter
    if(backupOptions->filterChk)
    {
        qDebug()<<"选中了filterChk";
        // 把filterconfig一个个复制到对应变量中
    }

    if(backupOptions->periodChk){
        backupOptions->interval = ui->periodicWidget->getValue();
    }else{
        backupOptions->interval = -1;
    }

    do{
        if(backupOptions->cmpsChk)
        {
            backupOptions->config.compress = true;
        }else{
            backupOptions->config.compress = false;
        }

        if(backupOptions->encryptChk)
        {
            backupOptions->config.crypto_config = zpods::CryptoConfig(backupOptions->password);
        }

        if(backupOptions->synChk)
        {
            zpods::sync_backup(backupOptions->target_dir.c_str(), backupOptions->config);
        }else{
            zpods::backup(backupOptions->target_dir.c_str(), backupOptions->config);
        }

        let backup_file_path = fmt::format("{}/{}", backupOptions->target_dir.c_str(), backupOptions->config.backup_filename->c_str());
        if (backupOptions->remoteChk) {
            let status = user.upload_file(backup_file_path.c_str());
            if (status == zpods::Status::OK) {
                spdlog::info("upload successfully!");
            } else {
                spdlog::info("fail to upload");
            }
        }

        if(backupOptions->periodChk)
        {
            std::this_thread::sleep_for(std::chrono::seconds(backupOptions->interval));
        }

    }while(backupOptions->interval>0);
}

//void MainWindow::handleRestore()
//{

//}

void MainWindow::enableStartBtn()
{
    connect(ui->startBtn, &QPushButton::clicked,this,[&](){

       // 这里其实可以开多线程了
       BackupOptions * backupOptions = new BackupOptions;
       backupOptions = tmpBackupOptions;
       handleBackup(backupOptions);
    });
}
