#include "daemonthread.h"

DaemonThread::DaemonThread(QObject* parent)
    : QThread(parent), stopRequested(false)
{
}

void DaemonThread::SetConfig(const zpods::DaemonConfig& config)
{
    this->config = config;
}

void DaemonThread::run()
{
    zpods::zpods_daemon_entry(config, stopRequested);
}

void DaemonThread::stopThread()
{
    stopRequested.store(true);
}

bool DaemonThread::isRunning()
{
    return !stopRequested;
}
