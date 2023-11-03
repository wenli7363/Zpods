#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("登陆");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::clearPasswordLineEdit() {
    ui->pswLineEdit->clear();
}
