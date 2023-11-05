#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#pragma once
#include "pch.h"
#include <QDialog>
#include <termios.h>


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

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
