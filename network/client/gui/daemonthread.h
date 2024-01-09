#ifndef DAEMONTHREAD_H
#define DAEMONTHREAD_H
#include <QThread>
#include <QDebug>
//#include "ZpodsLib/src/network/network.h"
#include "ZpodsLib/src/core/daemon.h"
#include "../grpc_client.h"

class DaemonThread: public QThread
{
    Q_OBJECT

public:
    DaemonThread(QObject* parent);
    void SetConfig(const zpods::DaemonConfig &config);
    bool isRunning();

protected:
    void run() override;

private:
    zpods::DaemonConfig config;
    bool isRun;
};

#endif // DAEMONTHREAD_H


