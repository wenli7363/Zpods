#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <network.h>
#include "zpods_lib.h"
#include "termios.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Zpods");

    LoginDialog* loginDialog = new LoginDialog(this);
    loginDialog->hide();
    enableRemote(loginDialog);

    enableFileFilter();

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::enableFileFilter()
{
    // enable FileFilter when clicked checkBox
    connect(ui->filterChkBox, &QCheckBox::stateChanged, this, [=](){
        bool filterChecked = ui->filterChkBox->isChecked();
        if(filterChecked){
            FileFilterDialog* filterDialog = new FileFilterDialog(this);
            filterDialog->exec();
        }
    });

}

bool MainWindow::enableRemote(LoginDialog* loginDialog)
{
    // enable Remote when clicked remoteCheckBox
    connect(ui->remoteChkBox, &QCheckBox::stateChanged, this, [=](){
        bool remoteChecked = ui->remoteChkBox->isChecked();
        if(remoteChecked && (!loginDialog->logined)){
           loginDialog->show();
           loginDialog->exec();
//           loginDialog->logined = true;
        }
    });
}
