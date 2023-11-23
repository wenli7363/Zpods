// BackupThread.h
#ifndef BACKUPTHREAD_H
#define BACKUPTHREAD_H

#include "pch.h"
#include "logindialog.h"
#include "filefilterdialog.h"
#include "myfiledialog.h"
#include "periodicwidget.h"
#include "zpods_lib.h"
#include "termios.h"
#include "fs.h"

#include <QThread>
#include <QString>
#include <QDebug>

struct ThreadInfo{
    int taskID;
    QString filename;
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
            : cmpsChk(false), encryptChk(false), synChk(false),
              periodChk(false), filterChk(false), remoteChk(false) {

        interval = -1;

    }
};


class BackupThread : public QThread
{
    Q_OBJECT

public:
    explicit BackupThread(QObject *parent = nullptr);
    ~BackupThread();

    // 设置备份相关参数的函数
    void setBackupParameters(BackupOptions backupOptions);
signals:
    void startedSignal(ThreadInfo info);
    void finishedSignal(ThreadInfo info);

protected:
    void run() override;

private:
    // 定义备份相关的私有成员变量
    BackupOptions backupOptions;
};

#endif // BACKUPTHREAD_H

