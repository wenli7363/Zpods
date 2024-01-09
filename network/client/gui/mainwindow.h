#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "backupthread.h"
#include "remotefiledialog.h"

//#include "daemonthread.h"

#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>
#include <QSet>
#include <QDebug>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <libssh/libssh.h>
#include <libssh/sftp.h>




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
    void handleLoginFailed();
    void enableStartBtn();


    void enableRSrcBtn();
    void enableRTargetBtn();
    void enableRestoreBtn();
    void enableThreadStopBtn();

    // 功能函数
    void regist();

    void enableList();
    void enableLogoutBtn();
    void enabledeleteBtnDL();
    void enableDownloadBtn();
    void enableTargetBtnDL();
     QList<QPair<QString,QString>> getRemoteFileList();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;              // 只有一个对话框，一次登录一个用户
    std::shared_ptr<FilterConfig> filterConfig;
//    DaemonThread* daemonThread;  // 一次只有一个用户，也只有一个daemon

    BackupOptions backupOptions;    // mainWindow Options
    QSet<QString> srcSet;
    QSet<QString> DLset;

//   QMap<QThread*, QListWidgetItem*> threadItemMap;
    QMap<uint, BackupThread*> threadMap;
    int ingRow,edRow;  // 当前监视列表的任务数（行号+1）

    // DOWLOAD PAGE
    RemoteFileDialog* fd;
    std::string dowloadTargetPath;
};
#endif // MAINWINDOW_H



