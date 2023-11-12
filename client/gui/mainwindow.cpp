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

   // Init variabls
   this->cmpsChk = this->remoteChk = this->encryptChk = this->synChk = false;
   this->filterChk = false;
   this->periodChk = false;
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
            this->filterChk = true;

            FileFilterDialog* filterDialog = new FileFilterDialog(this);
            connect(filterDialog,&FileFilterDialog::sentFilterConfig,this,[&](FilterConfig* filterconfig){
                this->filterConfig = filterconfig;
            });

            filterDialog->exec();   // 非模态
        }else{
           this->filterChk = false;
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
           this->remoteChk = true;
           loginDialog->clearPasswordLineEdit();
           loginDialog->show();
           loginDialog->exec();
//           loginDialog->logined = true;
        }else{
            this->remoteChk = false;
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
//            qDebug()<<dialog->selectedFiles();
            QStringList srcFileList = dialog->selectedFiles();

            ui->srcListWidget->clear();
            src_path_list.clear();

            for(const auto& fileName : srcFileList)
            {
                QListWidgetItem* item = new QListWidgetItem(fileName);
                ui->srcListWidget->addItem(item);

                src_path_list.push_back(fileName.toStdString());
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
            this->target_dir = targetPath.toStdString();
        }
    });
}

void MainWindow::enableCmpsChkBox()
{
    connect(ui->cmpsChkBox, &QCheckBox::stateChanged, this, [&](){
        bool cmpsChecked = ui->cmpsChkBox->isChecked();
        if(cmpsChecked){
            this->cmpsChk = true;
        }else{
            this->cmpsChk = false;
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
                this->password = psw.toStdString();
                this->encryptChk = true;
            } else {
                // 用户取消了输入密码，取消复选框勾选状态
                this->encryptChk = false;

                ui->encryptChkBox->setCheckState(Qt::Unchecked);
            }
        } else if (state == Qt::Unchecked) {
            // 复选框取消勾选时，取消加密
            this->encryptChk = false;
        }
    });
}


void MainWindow::enableSynChkBox()
{
    connect(ui->synChkBox, &QCheckBox::stateChanged, this, [&](){
        bool synChecked = ui->synChkBox->isChecked();
        if(synChecked){
            this->synChk = true;
        }else{
            this->synChk = false;
        }
    });
}

void MainWindow::enablePeriodBox()
{
    connect(ui->periodicWidget,&PeriodicWidget::sentPeriodOpen,this,[&](){
        this->periodChk = true;
    });

    connect(ui->periodicWidget, &PeriodicWidget::sentPeriodClose,this,[&](){
        this->periodChk = false;
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

void MainWindow::handleBackup()
{
    // config paths
   if (src_path_list.empty()) {
       QMessageBox::critical(this, "提示", "请选择要备份的文件！");
       spdlog::error("src path list is empty!");
       return;
    }

   if(target_dir.empty()){
       QMessageBox::critical(this,"提示","请选择要保存的路径");
       spdlog::error("target dir is empty!");
       return;
    }

    QString backupName = QInputDialog::getText(this, "请输入打包文件名", "请输入打包文件名:");
    if(backupName.isEmpty())
    {
        QMessageBox::critical(this, "提示", "请输入打包后的文件名！");
        return;
    }
    config.backup_filename = backupName.toStdString();

    config.filter.paths = std::vector<zpods::fs::zpath>(src_path_list.begin(), src_path_list.end());

    // if use the file filter
    if(filterChk)
    {
        qDebug()<<"选中了filterChk";
    }

    if(cmpsChk)
    {
        config.compress = true;
    }

    if(encryptChk)
    {
        config.crypto_config = zpods::CryptoConfig(password);
    }

    if(remoteChk)
    {
        qDebug()<<"选中了远程，但是没做";
    }

    if(synChk)
    {
        qDebug()<<"选中了同步，还没做";
        zpods::sync_backup(target_dir.c_str(), config);
    }

    if(periodChk){
        interval = ui->periodicWidget->getValue();
    }

//    do{


//    }while(interval>0)


}

//void MainWindow::handleRestore()
//{

//}

void MainWindow::enableStartBtn()
{
    connect(ui->startBtn, &QPushButton::clicked,this,[&](){
       handleBackup();
    });
}
