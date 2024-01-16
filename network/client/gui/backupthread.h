// BackupThread.h
#ifndef BACKUPTHREAD_H
#define BACKUPTHREAD_H

#include "ZpodsLib/src/base/fs.h"
#include "filefilterdialog.h"
#include "logindialog.h"
#include "myfiledialog.h"
#include "pch.h"
#include "periodicwidget.h"
#include "termios.h"
#include "zpods_lib.h"

#include <QDebug>
#include <QString>
#include <QThread>

struct ThreadInfo
{
    uint taskID;
    QString filename;
    QString cmps, syn, encrypt, remote, period;
};

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
        : cmpsChk(false),
          encryptChk(false),
          synChk(false),
          periodChk(false),
          filterChk(false),
          remoteChk(false)
    {

        interval = -1;
    }
};

class BackupThread : public QThread
{
    Q_OBJECT

  public:
    explicit BackupThread(QObject* parent = nullptr);
    ~BackupThread();

    // 设置备份相关参数的函数
    void setBackupParameters(BackupOptions backupOptions);
    void setShouldExit(bool value);

    ThreadInfo setThreadInfo();

  signals:
    void startedSignal(ThreadInfo info);
    void finishedSignal(ThreadInfo info);

  protected:
    void run() override;

  private:
    // 定义备份相关的私有成员变量
    BackupOptions backupOptions;
    std::atomic<bool> shouldExit;
};

#endif // BACKUPTHREAD_H
