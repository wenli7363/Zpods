#include "mainwindow.h"

using namespace zpods;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Zpods");        // set window title

    // create loginDialog
    this->loginDialog = new LoginDialog(this);
    this->loginDialog->hide();

    this->filterConfig = nullptr;
    backupOptions = BackupOptions();
    // set the connect function of two chkBox
    connectInit();
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
            backupOptions.filterChk = true;

            FileFilterDialog* filterDialog = new FileFilterDialog(this);
            connect(filterDialog,&FileFilterDialog::sentFilterConfig,this,[=](std::shared_ptr<FilterConfig> filterconfig){
                Q_ASSERT(filterconfig != nullptr);
                this->filterConfig = filterconfig;

                if(!(filterConfig->sizeChk||filterConfig->typeChk||
                     filterConfig->dateChk||filterConfig->reChk))
                {
                    ui->filterChkBox->setCheckState(Qt::Unchecked);
                    backupOptions.filterChk = false;
                }
            });

            filterDialog->exec();   // 非模态
            delete filterDialog;
            filterDialog = nullptr;
        }else{
           backupOptions.filterChk = false;
        }
    });
}

void MainWindow::enableRemote()
{
    // enable Remote when clicked remoteCheckBox
    connect(ui->remoteChkBox, &QCheckBox::stateChanged, this, [&](){
        bool remoteChecked = ui->remoteChkBox->isChecked();
        if(remoteChecked && (!loginDialog->logined)){
           backupOptions.remoteChk = true;
           loginDialog->clearPasswordLineEdit();
           loginDialog->show();
           loginDialog->exec();
//           loginDialog->logined = true;
        }else{
            backupOptions.remoteChk = false;
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
            backupOptions.src_path_list.clear();

            for(const auto& fileName : srcFileList)
            {
                QListWidgetItem* item = new QListWidgetItem(fileName);
                ui->srcListWidget->addItem(item);

                backupOptions.src_path_list.push_back(fileName.toStdString());
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
            backupOptions.target_dir = targetPath.toStdString();
        }
    });
}

void MainWindow::enableCmpsChkBox()
{
    connect(ui->cmpsChkBox, &QCheckBox::stateChanged, this, [&](){
        bool cmpsChecked = ui->cmpsChkBox->isChecked();
        if(cmpsChecked){
            backupOptions.cmpsChk = true;
        }else{
            backupOptions.cmpsChk = false;
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
                backupOptions.password = psw.toStdString();
                backupOptions.encryptChk = true;
            } else {
                // 用户取消了输入密码，取消复选框勾选状态
                backupOptions.encryptChk = false;

                ui->encryptChkBox->setCheckState(Qt::Unchecked);
            }
        } else if (state == Qt::Unchecked) {
            // 复选框取消勾选时，取消加密
            backupOptions.encryptChk = false;
        }
    });
}


void MainWindow::enableSynChkBox()
{
    connect(ui->synChkBox, &QCheckBox::stateChanged, this, [&](){
        bool synChecked = ui->synChkBox->isChecked();
        if(synChecked){
            backupOptions.synChk = true;
        }else{
            backupOptions.synChk = false;
        }
    });
}

void MainWindow::enablePeriodBox()
{
    connect(ui->periodicWidget,&PeriodicWidget::sentPeriodOpen,this,[&](){
        backupOptions.periodChk = true;
    });

    connect(ui->periodicWidget, &PeriodicWidget::sentPeriodClose,this,[&](){
        backupOptions.periodChk = false;
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

    enableRSrcBtn();
    enableRTargetBtn();
    enableRestoreBtn();
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

void MainWindow::handleBackup(BackupOptions backupOptions)
{
    // config paths
   if (backupOptions.src_path_list.empty()) {
       QMessageBox::critical(this, "提示", "请选择要备份的文件！");
       spdlog::error("src path list is empty!");
       return;
    }

   if(backupOptions.target_dir.empty()){
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

    backupOptions.config.filter.paths = std::vector<zpods::fs::zpath>(backupOptions.src_path_list.begin(), backupOptions.src_path_list.end());

    // if use the file filter
    if(backupOptions.filterChk)
    {
        // 把filterconfig一个个复制到对应变量中
        backupOptions.config.filter.min_date = filterConfig->min_date;
        backupOptions.config.filter.max_date = filterConfig->max_date;
        backupOptions.config.filter.min_size = filterConfig->minSize;
        backupOptions.config.filter.max_size = filterConfig->maxSize;
        backupOptions.config.filter.types = filterConfig->types;
        backupOptions.config.filter.re_list = filterConfig->re_list;
    }

    if(backupOptions.periodChk){
        backupOptions.interval = ui->periodicWidget->getValue();
    }else{
        backupOptions.interval = -1;
    }

    if(backupOptions.cmpsChk)
    {
        backupOptions.config.compress = true;
    }else{
        backupOptions.config.compress = false;
    }

    if(backupOptions.encryptChk)
    {
        backupOptions.config.crypto_config = zpods::CryptoConfig(backupOptions.password);
    }

    // create Thread here
    BackupThread *backupThread = new BackupThread(this);
    backupThread->setBackupParameters(backupOptions);

    // clear when thread finished
    connect(backupThread, &QThread::finished, backupThread, &QObject::deleteLater);

    ui->srcListWidget->clear();
    ui->targetPath->clear();
    ui->filterChkBox->setCheckState(Qt::Unchecked);
    ui->cmpsChkBox->setCheckable(Qt::Unchecked);
    ui->encryptChkBox->setCheckable(Qt::Unchecked);
    ui->remoteChkBox->setCheckable(Qt::Unchecked);
    this->backupOptions = BackupOptions();

    // 启动线程
    backupThread->start();
}


void MainWindow::handleRestore(std::string src, std::string target, QString psw)
{
    zpods::BackupConfig config;
    if(!psw.isEmpty())
    {
        config.crypto_config = zpods::CryptoConfig(psw.toStdString());
    }

    let status = zpods::restore(src.c_str(), target.c_str(), config);

    switch (status) {
        case zpods::Status::OK:
            spdlog::info("restore succeeded");
            QMessageBox::information(this,"成功","还原成功！！");
            ui->rTargetPath->clear();
            ui->rSrcPath->clear();
            ui->rPswLineEdit->clear();
            break;
        case zpods::Status::NOT_ZPODS_FILE:
            spdlog::error("restore failed: not a zpods file");
            break;
        case zpods::Status::ERROR:
            spdlog::error("restore failed: unknown error");
            break;
        case zpods::Status::PASSWORD_NEEDED:
            QMessageBox::critical(this,"错误","请输入密码！");
            ui->rPswLineEdit->clear();
            spdlog::error("restore failed: password needed");
            break;
        case zpods::Status::WRONG_PASSWORD:
            QMessageBox::critical(this,"错误","解压密码错误！");
            ui->rPswLineEdit->clear();
            spdlog::error("restore failed: wrong password");
            break;
        case zpods::Status::CHECKSUM_ERROR:
            QMessageBox::critical(this,"错误","文件损坏!");
            spdlog::error("restore failed: checksum error");
            break;
        default:
            QMessageBox::critical(this,"错误","未知错误");
            spdlog::error("restore failed: unknown error");
        }
}

void MainWindow::enableStartBtn()
{
    connect(ui->startBtn, &QPushButton::clicked,this,[&](){

       // 另起一个thread
       handleBackup(backupOptions);

       // 删除旧配置

    });
}

void MainWindow::enableRSrcBtn()
{
    connect(ui->rSrcBtn, &QPushButton::clicked,this,[&](){
        QString fileName = QFileDialog::getOpenFileName(this,
        "Open File",
        QDir::homePath(),
        "Zpods Files (*.pods);;All Files (*)");

        // 如果用户选择了文件，显示文件名
        if (!fileName.isEmpty()) {
            ui->rSrcPath->setText(fileName);
        }
    });

}

void MainWindow::enableRTargetBtn()
{
    connect(ui->rTargetBtn, &QPushButton::clicked, this, [&](){
        QString targetPath = QFileDialog::getExistingDirectory(this, "选择目的路径", QDir::homePath());

        if (!targetPath.isEmpty())
        {
            ui->rTargetPath->setText(targetPath);
        }
    });
}

void MainWindow::enableRestoreBtn()
{
    connect(ui->restoreBtn,&QPushButton::clicked,this,[=](){
        QString targetPath = ui->rTargetPath->text();
        QString srcPath = ui->rSrcPath->text();
        QString psw = ui->rPswLineEdit->text();
        handleRestore(srcPath.toStdString(),targetPath.toStdString(),psw);
    });
}
