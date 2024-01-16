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
    logined = false;
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    this->daemonThread = new DaemonThread(this);

    connectLoginBtn();
    connectRegBtn();
    connectRegBtn2();
    connectReturnBtn();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::loginDialogReset() {
    delete daemonThread;
    daemonThread = new DaemonThread(this);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    ui->pswLineEdit->clear();
    logined = false;
    user = {};
//    if (daemonThread->isRunning())
//    {
//        daemonThread->terminate();
//        daemonThread->wait();

//        // 把实例清空
//        delete daemonThread;
//        this->daemonThread = new DaemonThread(this);
//    }
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
        user.username = get_username();
        user.password = get_password();
        if(zpods::Status::OK == user.login())
        {
            logined = true;
            QMessageBox::information(this,"登录成功","登录成功");
            // 开启daemon
            startDaemon();
        }else{
            QMessageBox::critical(this,"错误","登录失败，请联系管理员");
            logined = false;
        }
    });
}

void LoginDialog::connectRegBtn()
{
    connect(ui->registerBtn, &QPushButton::clicked,[&](){
        ui->usrLineEdit2->clear();
        ui->pswRegLine->clear();
        ui->pswRegLine2->clear();
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
//            QMessageBox::critical(this,"提示","用户名和密码不能为空");
            return;
         }

         if(psw == psw2)
         {
            // register directly
             user.username = userName;
             user.password = psw;
             user.password = psw2;
             auto status = user.register_();

             if(status == zpods::Status::OK)
             {
                 QMessageBox::information(this,"成功","注册成功");
             }else{
                 QMessageBox::critical(this,"失败","注册失败，详细原因看日志");
             }
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

void LoginDialog::closeEvent(QCloseEvent *event)
{
    if(!this->logined)
    {
        emit sentLoginFailed();
    }

    QDialog::closeEvent(event);
}

void LoginDialog::startDaemon()
{
    zpods::DaemonConfig config;
//    DaemonThread* daemonThread = new DaemonThread(this);

    config.query_pods = [&](PodsQueryResult& result) {
        return user.query_pods(result);
    };
    config.download_pod = [&](const std::string& pods_name,
                              const std::string& pod_name,
                              const std::string& dir) {
        return this->user.download_pod(pods_name, pod_name, dir);
    };
    config.upload_pod = [&](const std::string& pod_path) {
        return this->user.upload_pod(pod_path);
    };
        daemonThread->SetConfig(config);
        daemonThread->start();
}
