// BackupThread.h
// execute a backup action
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
    int interval;       //period backup

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

    // set backup parameters
    void setBackupParameters(BackupOptions backupOptions);
    // when value = true, the thread should stop
    void setShouldExit(bool value);

    ThreadInfo setThreadInfo();

  signals:
    // the backup thread started
    void startedSignal(ThreadInfo info);
    // the backup thread finished
    void finishedSignal(ThreadInfo info);

  protected:
    void run() override;

  private:
    // all options about this backup action
    BackupOptions backupOptions;
    // decide the thread to stop or not
    std::atomic<bool> shouldExit;
};

#endif // BACKUPTHREAD_H
