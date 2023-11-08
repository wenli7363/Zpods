#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pch.h"
#include "logindialog.h"
#include "filefilterdialog.h"
#include "myfiledialog.h"
#include "zpods_lib.h"
#include "termios.h"
#include <network.h>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 成员变量
//    std::vector<std::string> src_path_list;
    QStringList srcFileList;
    std::string target_dir;
    std::string password;
    zpods::BackupConfig config;
    zpods::User user;
    std::string min_date;
    std::string max_date;
    int interval = -1;

    // fileter flag
    bool cmpsChk, encryptChk, remoteChk, synChk;
    bool periodChk;
    bool filterChk;

    // 方法
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 槽函数初始化
    void connectInit();
    // connect function of FileFilter CheckBox
    void enableFileFilter();
    // connect function of Remote CheckBox
    void enableRemote();
    // connect function of srcPushBtn
    void enableSrcBtn();
    void enableTargetBtn();
    // connect function of chkBox
    void enableCmpsChkBox();
    void enableEncryptChkBox();
    void enableSynChkBox();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;
};
#endif // MAINWINDOW_H
