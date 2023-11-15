#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pch.h"
#include "logindialog.h"
#include "filefilterdialog.h"
#include "myfiledialog.h"
#include "periodicwidget.h"
#include "zpods_lib.h"
#include "termios.h"
#include <network.h>
#include <QString>

struct BackupOptions
{
    // 成员变量
    std::vector<std::string> src_path_list;
    std::string target_dir;
    std::string password;
    zpods::BackupConfig config;
    std::string min_date;
    std::string max_date;
    int interval;

    // fileter flag
    bool cmpsChk, encryptChk, synChk;
    bool periodChk;
    bool filterChk;
    bool remoteChk; //表示是否开启远程功能，只有成功登陆了才为true


    BackupOptions()
            : cmpsChk(false), encryptChk(false), synChk(false),
              periodChk(false), filterChk(false), remoteChk(false) {

        std::string min_date = "0001-01-01";
        std::string max_date = "9999-12-31";
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
    void handleBackup();
    void handleRestore();
    void enableStartBtn();

    // 功能函数
    void regist();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;
    FilterConfig* filterConfig;

    BackupOptions* tmpBackupOptions;    // 每次主界面的配置，之后会存起来持久化
    zpods::User user;

};
#endif // MAINWINDOW_H
