#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "pch.h"
#include "logindialog.h"
#include "filefilterdialog.h"
#include "myfiledialog.h"
#include "periodicwidget.h"
#include "zpods_lib.h"
#include "termios.h"
#include "fs.h"

#include <network.h>
#include <QString>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>


struct BackupOptions
{
    // 成员变量
    std::vector<std::string> src_path_list;
    std::string target_dir;
    std::string password;
    zpods::BackupConfig config;
    int interval;

    // fileter flag
    bool cmpsChk, encryptChk, synChk;
    bool periodChk;
    bool filterChk;
    bool remoteChk; //表示是否开启远程功能，只有成功登陆了才为true


    BackupOptions()
            : cmpsChk(false), encryptChk(false), synChk(false),
              periodChk(false), filterChk(false), remoteChk(false) {

        int interval = -1;

    }
};

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
    void enableTargetBtn();
    void enableCmpsChkBox();
    void enableEncryptChkBox();
    void enableSynChkBox();
    void enablePeriodBox();
    void handleRegist();
    void handleBackup(BackupOptions* backupOptions);
    void handleRestore();
    void enableStartBtn();

    // 功能函数
    void regist();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;
    std::shared_ptr<FilterConfig> filterConfig;

    BackupOptions* backupOptions;    // mainWindow Options
    zpods::User user;

};
#endif // MAINWINDOW_H
