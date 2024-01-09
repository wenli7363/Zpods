#include "daemonthread.h"

//DaemonThread::DaemonThread(const zpods::DaemonConfig& config) : config(config) {}
//DaemonThread::DaemonThread(){}
DaemonThread::DaemonThread(QObject* parent = nullptr){
    this->isRun = false;
}

void DaemonThread::run()
{
    zpods::zpods_daemon_entry(config);
}

void DaemonThread::SetConfig(const zpods::DaemonConfig &config)
{
    this->config = config;
}

bool DaemonThread::isRunning()
{
    return isRun;
}
