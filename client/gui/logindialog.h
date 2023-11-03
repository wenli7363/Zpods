#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
