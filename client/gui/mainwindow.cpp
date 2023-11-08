#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QStringList>
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
            filterDialog->exec();
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
            srcFileList = dialog->selectedFiles();
            ui->srcListWidget->clear();
            for(const auto& fileName : srcFileList)
            {
                QListWidgetItem* item = new QListWidgetItem(fileName);
                ui->srcListWidget->addItem(item);
            }
//            QSet<QString> uniqueFileNames;
//            for (const QString &fileName : srcFileList)
//            {
//                uniqueFileNames.insert(fileName);
//            }

//            // 将去重后的文件路径存回QStringList
//            srcFileList = uniqueFileNames.toList();

//            // 遍历选定的文件路径，将它们添加到QListWidget中
//            for (const QString &fileName : srcFileList)
//            {
//             this->src_path_list.push_back(qstr.toStdString());
//               QListWidgetItem *item = new QListWidgetItem(fileName);
//               ui->srcListWidget->addItem(item);
//            }
// Debug info
//            for(const std::string& path : src_path_list)
//            {
//                qDebug()<<path.c_str();
//            }

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
//        qDebug()<<"cmpsChk:"<<cmpsChk;
    });
}

void MainWindow::enableEncryptChkBox()
{
    connect(ui->encryptChkBox, &QCheckBox::stateChanged, this, [&](){
        bool encryptChecked = ui->encryptChkBox->isChecked();
        if(encryptChecked){
             this->encryptChk = true;
        }else{
             this->encryptChk = false;
        }
//        qDebug()<<"encryptChk:"<<encryptChk;
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
//        qDebug()<<"synChk:"<<synChk;
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
}

