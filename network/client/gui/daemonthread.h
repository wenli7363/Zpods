// Daemon Thread
// daemon use for monitoring synchronization tasks
#ifndef DAEMONTHREAD_H
#define DAEMONTHREAD_H
#include <QDebug>
#include <QThread>
#include "../grpc_client.h"
#include "ZpodsLib/src/core/daemon.h"

class DaemonThread : public QThread
{
    Q_OBJECT

  public:
    DaemonThread(QObject* parent);
    void SetConfig(const zpods::DaemonConfig& config);
    bool isRunning();
    void stopThread();

  protected:
    void run() override;

  private:
    zpods::DaemonConfig config;
    std::atomic<bool> stopRequested;
};

#endif // DAEMONTHREAD_H
