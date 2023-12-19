#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#pragma once
#include "pch.h"
#include <QDialog>
#include <termios.h>
#include <QString>
#include "../grpc_client.h"
//#include "ZpodsLib/src/zpods_lib.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    zpods::RpcUser user;
    bool logined;

    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void loginDialogReset();

    std::string get_password();
    std::string get_username();

    // 注册页面
    std::string get_Rpassword();
    std::string get_Rusername();
    std::string get_Rpassword2();

    void connectLoginBtn();
    void connectRegBtn();
    void connectRegBtn2();
    void connectReturnBtn();

protected:
    void closeEvent(QCloseEvent *event) ;

signals:
    void sentLoginFailed();

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
