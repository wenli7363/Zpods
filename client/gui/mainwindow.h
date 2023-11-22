#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "backupthread.h"

#include <network.h>

#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 方法
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 槽函数初始化
    void connectInit();
    // 槽函数
    void enableFileFilter();
    void enableRemote();
    void enableSrcBtn();
    void enableDeleteBtn();
    void enableTargetBtn();
    void enableCmpsChkBox();
    void enableEncryptChkBox();
    void enableSynChkBox();
    void enablePeriodBox();
    void handleRegist();
    void handleBackup(BackupOptions backupOptions);
    void handleRestore(std::string src, std::string target, QString psw);
    void enableStartBtn();

    void enableRSrcBtn();
    void enableRTargetBtn();
    void enableRestoreBtn();

    // 功能函数
    void regist();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;
    std::shared_ptr<FilterConfig> filterConfig;

    BackupOptions backupOptions;    // mainWindow Options
    zpods::User user;

};
#endif // MAINWINDOW_H
