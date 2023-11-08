#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("登陆");
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    connectLoginBtn();
    connectRegBtn();
    connectRegBtn2();
    connectReturnBtn();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::clearPasswordLineEdit() {
    ui->pswLineEdit->clear();
}

std::string LoginDialog::get_username() {
    QString username = ui->usrLineEdit->text();
    if (username.isEmpty())
       {
           // 文本框为空，弹出非模态对话框
            QMessageBox::critical(this, "提示", "请输入用户名！");
            return "";
       }
       else
       {
           return username.toStdString();
       }
}

std::string LoginDialog::get_password() {
    QString psw = ui->pswLineEdit->text();
    if (psw.isEmpty())
       {
           // 文本框为空，弹出非模态对话框
            QMessageBox::critical(this, "提示", "请输入密码！");
            return "";
       }
       else
       {
           return psw.toStdString();
       }
}

void LoginDialog::connectLoginBtn()
{
    connect(ui->loginBtn, &QPushButton::clicked,[&]()
    {
        qDebug()<<"usrname:"<<QString::fromStdString(get_username());
        qDebug()<<"psw: "<<QString::fromStdString(get_password());
    });
}

void LoginDialog::connectRegBtn()
{
    connect(ui->registerBtn, &QPushButton::clicked,[&](){
        ui->stackedWidget->setCurrentWidget(ui->regPage);
    });
}

void LoginDialog::connectRegBtn2()
{
    connect(ui->registerBtn2,&QPushButton::clicked,[&](){
         std::string userName = get_Rusername();
         std::string psw = get_Rpassword();
         std::string psw2 = get_Rpassword2();

         //之前提醒了用户，如果用户还是不输入，就不执行了。
         // 避免了用户输入为空时，注册了一个空用户
         if(userName == "" || psw == "")
         {
            return;
         }

         if(psw == psw2)
         {
//             qDebug()<<"usr: " << QString::fromStdString(userName)<<" psw: "<< QString::fromStdString(psw);
            // 注册逻辑
         }else{
            QMessageBox::critical(this, "提示", "两次输入密码不同，请重新输入！");
            ui->pswRegLine->clear();
            ui->pswRegLine2->clear();
         }
    });
}

void LoginDialog::connectReturnBtn()
{
    connect(ui->returnBtn, &QPushButton::clicked,[&](){
        ui->stackedWidget->setCurrentWidget(ui->loginPage);
    });
}

std::string LoginDialog::get_Rusername()
{
    QString usr = ui->usrLineEdit2->text();
    if (usr.isEmpty())
       {
           // 文本框为空，弹出非模态对话框
            QMessageBox::critical(this, "提示", "请输入用户名！");
            return "";
       }
       else
       {
           return usr.toStdString();
       }
}

std::string LoginDialog::get_Rpassword()
{
    QString psw = ui->pswRegLine->text();
    if (psw.isEmpty())
       {
           // 文本框为空，弹出非模态对话框
            QMessageBox::critical(this, "提示", "请输入密码！");
            return "";
       }
       else
       {
           return psw.toStdString();
       }
}

std::string LoginDialog::get_Rpassword2()
{
    QString psw = ui->pswRegLine2->text();
    if (psw.isEmpty())
       {
           // 文本框为空，弹出非模态对话框
            QMessageBox::critical(this, "提示", "请再次输入密码！");
            return "";
       }
       else
       {
           return psw.toStdString();
       }
}
