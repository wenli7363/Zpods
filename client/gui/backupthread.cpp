// BackupThread.cpp
#include "backupthread.h"

BackupThread::BackupThread(QObject *parent) : QThread(parent)
{
    // 初始化备份相关的成员变量
   this->backupOptions = BackupOptions();
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
    // 执行备份任务的实现
    do{
        if(backupOptions.synChk)
        {
            zpods::sync_backup(backupOptions.target_dir.c_str(), backupOptions.config);
        }else{
            zpods::backup(backupOptions.target_dir.c_str(), backupOptions.config);
        }

//        let backup_file_path = fmt::format("{}/{}", backupOptions.target_dir.c_str(), backupOptions.config.backup_filename->c_str());
//        if (backupOptions.remoteChk) {
//            let status = user.upload_file(backup_file_path.c_str());
//            if (status == zpods::Status::OK) {
//                spdlog::info("upload successfully!");
//            } else {
//                spdlog::info("fail to upload");
//            }
//        }

        if(backupOptions.periodChk)
        {
            std::this_thread::sleep_for(std::chrono::seconds(backupOptions.interval));
        }

    }while(backupOptions.interval>0);
}
