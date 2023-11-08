#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#pragma once
#include "pch.h"
#include <QDialog>
#include <termios.h>
#include <QString>


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    bool logined = false;

    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void clearPasswordLineEdit();

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

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
