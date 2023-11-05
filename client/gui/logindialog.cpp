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

std::string LoginDialog::get_password() {
    std::cout << "Password: ";

    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string password;
    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    std::cout << std::endl;
    return password;
}

std::string LoginDialog::get_username() {
    std::cout << "Username: ";
    std::string username;
    std::getline(std::cin, username);
    return username;
}
