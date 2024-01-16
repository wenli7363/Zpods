// BackupThread.cpp
#include "backupthread.h"

BackupThread::BackupThread(QObject* parent) : QThread(parent)
{
    // 初始化备份相关的成员变量
    this->backupOptions = BackupOptions();
    this->shouldExit = false;
}

BackupThread::~BackupThread()
{
    // 在线程销毁时进行清理工作
}

void BackupThread::setBackupParameters(BackupOptions backupOptions)
{
    // 设置备份相关参数的实现
    this->backupOptions = backupOptions;
}

void BackupThread::run()
{
    // 发送线程信息，交给monitor
    ThreadInfo Info = this->setThreadInfo();
    emit startedSignal(Info);

    do
    {
        if (backupOptions.remoteChk)
        {
            break;
        }

        for (const auto& item : backupOptions.src_path_list)
        {
            backupOptions.config.tree_dir = item;
            if (backupOptions.synChk)
            {
                zpods::sync_backup(backupOptions.target_dir.c_str(),
                                   backupOptions.config);
            }
            else
            {
                zpods::backup(backupOptions.target_dir.c_str(),
                              backupOptions.config);
            }
        }

        if (backupOptions.periodChk)
        {
            std::this_thread::sleep_for(
                std::chrono::seconds(backupOptions.interval));
        }
    } while (backupOptions.interval > 0 && !shouldExit.load());

    emit finishedSignal(Info);
}

ThreadInfo BackupThread::setThreadInfo()
{
    ThreadInfo Info;
    Info.taskID = qHash(QThread::currentThreadId()) % 1007;
    Info.filename = QString::fromStdString(
        zpods::fs::get_last_part(backupOptions.src_path_list[0]));
    Info.cmps = backupOptions.cmpsChk ? "√" : "";
    Info.syn = backupOptions.synChk ? "√" : "";
    Info.encrypt = backupOptions.encryptChk ? "√" : "";
    Info.remote = backupOptions.remoteChk ? "√" : "";

    Info.period =
        backupOptions.periodChk ? QString::number(backupOptions.interval) : "";
    return Info;
}

void BackupThread::setShouldExit(bool value)
{
    shouldExit.store(value);
}
