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
    DaemonThread(QObject* parent = nullptr);
    bool isRunning();         // 判断daemon是否运行中
    void setUser(zpods::RpcUser* loginedUser);

protected:
    void run() override;

private:
    zpods::RpcUser* daemonUser;
    bool daemonStatus;
};

#endif // DAEMONTHREAD_H
