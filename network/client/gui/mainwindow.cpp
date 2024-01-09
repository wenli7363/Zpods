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

//    this->daemonThread = new DaemonThread();

//    fd = new RemoteFileDialog();

    this->filterConfig = nullptr;
    backupOptions = BackupOptions();

    ui->srcListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->srcListWidgetDL->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // ======================================   monitor绘制  ============================
    QStringList headerLabels;
    headerLabels <<"任务号"<<"保存的文件名"<<"压缩"<<"加密"<<"同步"<<"远程"<<"周期(s)";
    ui->ingTable->setRowCount(6);
    ui->edTable->setRowCount(6);
    ui->ingTable->verticalHeader()->setVisible(false);
    ui->edTable->verticalHeader()->setVisible(false);
    ui->ingTable->setColumnCount(7);
    ui->edTable->setColumnCount(7);
    ui->ingTable->setHorizontalHeaderLabels(headerLabels);
    ui->edTable->setHorizontalHeaderLabels(headerLabels);
    ui->ingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->edTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->edTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ingTable->setSelectionMode(QAbstractItemView::SingleSelection);


    // 定义每一列的百分比
    QList<int> columnWidths = {10, 50, 7, 7, 7, 7, 12};  // 以百分比表示每一列的宽度

    int ingWidth = ui->ingTable->width();  // 获取TableWidget的宽度
    int edWidth  = ui->edTable->width();
    // 计算并设置每一列的宽度
    for (int i = 0; i < columnWidths.size(); ++i) {
        int width1 = ingWidth * columnWidths[i] / 100;
        int width2 = edWidth * columnWidths[i] / 100;
        ui->ingTable->horizontalHeader()->resizeSection(i, width1);
        ui->edTable->horizontalHeader()->resizeSection(i,width2);
    }

 // =================================================================================


    ingRow = edRow = 0;

    qRegisterMetaType<ThreadInfo>("ThreadInfo");

    // connect function for all checkbox
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
            connect(filterDialog,&FileFilterDialog::sentFilterConfig,this,[this](std::shared_ptr<FilterConfig> filterconfig){
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
        if(remoteChecked){
           backupOptions.remoteChk = true;

           loginDialog->loginDialogReset();
           loginDialog->show();
           loginDialog->exec();


          if(loginDialog->logined){
                ui->targetLabel->setText("远程服务器IP：端口号");
                ui->targetPath->setEnabled(false);
                ui->targetPath->setText("47.108.88.125:50051");
                ui->encryptChkBox->setEnabled(false);
                ui->periodicWidget->setEnabled(false);
          }

        }else{
            // uncheck状态 = 未登录/退出已登录
            backupOptions.remoteChk = false;
            ui->targetLabel->setText("目标路径");
            ui->targetPath->clear();
            ui->targetPath->setEnabled(true);
            ui->encryptChkBox->setEnabled(true);
            ui->periodicWidget->setEnabled(true);
            loginDialog->logined = false;
            // 若已登录再退出，停止daemon
//            if(daemonThread->isRunning()){
//                daemonThread->terminate();
//                daemonThread->wait();

//                 // 把实例清空
//                delete daemonThread;
//                this->daemonThread =new DaemonThread();
//            }

//            ui->remoteChkBox->setCheckState(Qt::Unchecked);
        }
    });
}


void MainWindow::enableSrcBtn()
{
    connect(ui->srcBtn, &QPushButton::clicked, this, [this](){
        MyFileDialog *dialog =new MyFileDialog(this);
        dialog->setDirectory(QDir::homePath());
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

//            ui->srcListWidget->clear();

            for(const auto& fileName : srcFileList)
            {
                if(!srcSet.contains(fileName))
                {
                    srcSet.insert(fileName);
                    QListWidgetItem* item = new QListWidgetItem(fileName);
                    ui->srcListWidget->addItem(item);
                }
            }
        }
    });
}

void MainWindow::enableDeleteBtn()
{
    connect(ui->deleteBtn,&QPushButton::clicked,this,[&](){
        QList<QListWidgetItem *> selectedItems = ui->srcListWidget->selectedItems();
        for (QListWidgetItem *item : selectedItems)
        {
            int row = ui->srcListWidget->row(item);
            QString itemText = item -> text();
            srcSet.remove(itemText);
            delete ui->srcListWidget->takeItem(row);
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
    enableDeleteBtn();
    enableTargetBtn();

    enableCmpsChkBox();
    enableEncryptChkBox();
    enableSynChkBox();
    enablePeriodBox();
    handleLoginFailed();
    enableStartBtn();
    enableThreadStopBtn();

    enableRSrcBtn();
    enableRTargetBtn();
    enableRestoreBtn();
    enableList();
    enableLogoutBtn();
    enabledeleteBtnDL();
    enableDownloadBtn();
    enableTargetBtn();
    enableTargetBtnDL();
}



void MainWindow::handleBackup(BackupOptions backupOptions)
{
    // 获取src列表
    for (int i = 0; i < ui->srcListWidget->count(); ++i)
    {
        QListWidgetItem *item = ui->srcListWidget->item(i);
        QString text = item->text();
        backupOptions.src_path_list.push_back(text.toStdString());
    }

   // 保存这次备份的所有选项
   if (backupOptions.src_path_list.empty()) {
       QMessageBox::critical(this, "提示", "请选择要备份的文件！");
       spdlog::error("src path list is empty!");
       return;
    }

   if(!backupOptions.remoteChk && backupOptions.target_dir.empty() ){
       QMessageBox::critical(this,"提示","请选择要保存的路径");
       spdlog::error("target dir is empty!");
       return;
    }

    // 保存过滤器信息
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


    // 如果同时启用了remote+sync，开启daemon进行同步
//   if(backupOptions.remoteChk && backupOptions.synChk)
//   {
//            daemonThread->setUser(&loginDialog->user);
//            this->daemonThread->start();
//   }

    if(backupOptions.remoteChk)
    {
        do
            {
                if (backupOptions.target_dir.empty())
                {
                    backupOptions.target_dir = ZPODS_HOME_PATH;
                }

                for (const auto& item : backupOptions.src_path_list)
                {
                    backupOptions.config.tree_dir = item;
                    if (backupOptions.synChk)
                    {
                        zpods::sync_backup(backupOptions.target_dir.c_str(), backupOptions.config);
                    }
                    else
                    {
                        zpods::backup(backupOptions.target_dir.c_str(), backupOptions.config);
                    }
                    let backup_file_path = backupOptions.config.current_pod_path.parent_path();

                    if (backupOptions.remoteChk)
                    {
                        let status = loginDialog->user.upload_pods(backup_file_path.c_str());
                        if (status == zpods::Status::OK)
                        {
                            spdlog::info("upload successfully!");
                        }
                        else
                        {
                            spdlog::info("fail to upload");
                        }
                    }
                }

                if (backupOptions.periodChk)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(backupOptions.interval));
                }
            } while (backupOptions.interval > 0); // periodic backup
    }


    // create Thread here
    BackupThread *backupThread = new BackupThread(this);
    backupThread->setBackupParameters(backupOptions);           // 将backupoption传给线程

    this->backupOptions = BackupOptions();      // 将backupOption清空，供下次使用

    ui->srcListWidget->clear();
    ui->targetPath->clear();
    ui->filterChkBox->setCheckState(Qt::Unchecked);
    ui->cmpsChkBox->setCheckState(Qt::Unchecked);
    ui->encryptChkBox->setCheckState(Qt::Unchecked);
//    ui->remoteChkBox->setCheckState(Qt::Unchecked);       // 保持登录状态
    ui->synChkBox->setCheckState(Qt::Unchecked);
    ui->periodicWidget->setUnchecked();
    srcSet.clear();

    // clear when thread finished
    connect(backupThread, &QThread::finished, backupThread, &QObject::deleteLater);

    // monitor thread
   connect(backupThread, &BackupThread::startedSignal,this,[this,backupThread](ThreadInfo info){
        threadMap.insert(info.taskID,backupThread);

        // 设置monitor界面
       if (ingRow >= ui->ingTable->rowCount()) {
           ui->ingTable->setRowCount(ingRow + 1);
       }
       QStringList record;
       record << QString::number(info.taskID)
              << info.filename
              << info.cmps
              << info.encrypt
              << info.syn
              << info.remote
              << info.period;

       for (int col = 0; col < 7; ++col) {
               QTableWidgetItem *item = new QTableWidgetItem(record[col]);
               ui->ingTable->setItem(ingRow, col, item);
           }

        ingRow++;
   });

    connect(backupThread,&BackupThread::finishedSignal,this,[this,backupThread](ThreadInfo info){
        QString message = QStringLiteral("%1备份成功").arg(info.taskID);
        QMessageBox::information(this,"结束任务提示",message);
        threadMap.remove(info.taskID);
        for (int row = 0; row < ui->ingTable->rowCount(); ++row) {
                if (ui->ingTable->item(row, 0)->text() == QString::number(info.taskID) ) {
                    ui->ingTable->removeRow(row);
                    ingRow--;
                    break;
                }
            }

        // move to edTable
        QStringList record;
        record << QString::number(info.taskID)
               << info.filename
               << info.cmps
               << info.encrypt
               << info.syn
               << info.remote
               << info.period;

        for (int col = 0; col < 7; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(record[col]);
            ui->edTable->setItem(edRow, col, item);
        }
        edRow++;

    });

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
       handleBackup(backupOptions);
    });
}

void MainWindow::enableRSrcBtn()
{
    connect(ui->rSrcBtn, &QPushButton::clicked,this,[&](){
        QString fileName = QFileDialog::getExistingDirectory(this,
        "Open File",
        QDir::homePath());

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
    connect(ui->restoreBtn,&QPushButton::clicked,this,[this](){
        QString targetPath = ui->rTargetPath->text();
        QString srcPath = ui->rSrcPath->text();
        QString psw = ui->rPswLineEdit->text();
        handleRestore(srcPath.toStdString(),targetPath.toStdString(),psw);
    });
}

void MainWindow::enableThreadStopBtn()
{
    connect(ui->threadStopBtn,&QPushButton::clicked,this,[this](){
        QTableWidgetItem* selectedItem = ui->ingTable->currentItem();
        if (selectedItem) {
            uint taskId = ui->ingTable->item(selectedItem->row(),0)->text().toUInt();
            BackupThread* associatedThread = threadMap.value(taskId, nullptr);
            associatedThread->setShouldExit(true);
        }else{
            QMessageBox::critical(this,"warning","未选中任何正在执行的任务！");
        }
    });
}

void MainWindow::handleLoginFailed()
{
    connect(loginDialog, &LoginDialog::sentLoginFailed, this, [&]()
    {
        ui->remoteChkBox->setCheckState(Qt::Unchecked);
    });
}

// 选择要下载的文件
void MainWindow::enableList()
{
    connect(ui->listBtn, &QPushButton::clicked,this, [this](){
        if(loginDialog->logined){
            auto fileList = getRemoteFileList();
            fd = new RemoteFileDialog(this, fileList);          // 这里会产生多个实例，要改！！！！
            fd->show();
            fd->exec();
            for(const auto& file : fd->selectedDLFiles)
            {
                if(!DLset.contains(file))
                {
                    DLset.insert(file);
                    QListWidgetItem* item = new QListWidgetItem(file);
                    ui->srcListWidgetDL->addItem(item);
                }
            }
        }else{
            QMessageBox::critical(this,"警告","请先登录！");
            ui->remoteChkBox->setCheckState(Qt::Checked);
        }




    });
}


 QList<QPair<QString,QString>>  MainWindow::getRemoteFileList(){
//    QStringList remotePodsList;
//    QMap<QString, QStringList> remoteFiles;
     QList<QPair<QString,QString>> remoteFiles;
    PodsQueryResult result;
    loginDialog->user.query_pods(result);
    for (const auto& [pods_name, pod_list] : result)
    {
        for (const auto& [pod_name, last_modified_ts] : pod_list)
        {
//            spdlog::info("{}/{}", pods_name, pod_name);
            QString pod = QString::fromStdString(pod_name);
            QString pods = QString::fromStdString(pods_name);

            if(!pod.contains(".META")){
                remoteFiles.append({pods,pod});
            }

        }
    }

    return remoteFiles;
}

void MainWindow::enableLogoutBtn()
{
    connect(ui->logoutBtn, &QPushButton::clicked, this, [this](){
        ui->remoteChkBox->setCheckState(Qt::Unchecked);
    });

}

void MainWindow::enabledeleteBtnDL()
{
    connect(ui->deleteBtnDL, &QPushButton::clicked, this, [this](){
        QList<QListWidgetItem *> selectedItems = ui->srcListWidgetDL->selectedItems();
        for (QListWidgetItem *item : selectedItems)
        {
            int row = ui->srcListWidgetDL->row(item);
            QString itemText = item -> text();
            DLset.remove(itemText);
            delete ui->srcListWidgetDL ->takeItem(row);
        }
    });
}

void MainWindow::enableDownloadBtn()
{
    connect(ui->downloadBtn, &QPushButton::clicked, this,[this](){
        if(this->DLset.empty())
        {
            QMessageBox::critical(this,"错误","请选择需要下载的PODS");
            return;
        }

        if(ui->targetPathDL->text().isEmpty())
        {
            QMessageBox::critical(this,"错误","请选择一个保存的路径");
            return;
        }

        // 补充代码
        for(const QString &item : this->DLset)
        {
                qDebug()<<item;
                QStringList parts = item.split('/');
                if(parts.size() == 2)
                {
                    auto pods = parts[0].toStdString();
                    auto pod = parts[1].toStdString();
                    loginDialog->user.download_pod(pods, pod, this->dowloadTargetPath);
                }else{
                    qDebug()<<"Failed to split the string"<<item;
                }
        }
    });
}

void MainWindow::enableTargetBtnDL()
{
    connect(ui->targetBtnDL, &QPushButton::clicked, this, [this](){
        QString targetPath = QFileDialog::getExistingDirectory(this, "选择目的路径", QDir::homePath());

        if (!targetPath.isEmpty())
        {
            ui->targetPathDL->setText(targetPath);
            dowloadTargetPath = targetPath.toStdString();
//            backupOptions.target_dir = targetPath.toStdString();
//            qDebug()<<targetPath;
        }
    });
}
