#include "daemonthread.h"
DaemonThread::DaemonThread(QObject *parent ) : QThread(parent) {
    this->daemonStatus = false;
}


void DaemonThread::run()
{
    zpods::DaemonConfig config;
    config.query_pods = [&](PodsQueryResult& result) {
            return daemonUser->query_pods(result);
     };
     config.download_pod = [&](const std::string& pods_name,
                               const std::string& pod_name,
                               const std::string& dir) {
            return daemonUser->download_pod(pods_name, pod_name, dir);
     };
     config.upload_pod = [&](const std::string& pod_path) {
            return daemonUser->upload_pod(pod_path);
     };
     zpods::zpods_daemon_entry(config);

    daemonStatus = true;
}

bool DaemonThread::isRunning()
{
    return this->daemonStatus;
}

void DaemonThread::setUser(zpods::RpcUser* loginedUser)
{
    if(loginedUser) {
        this->daemonUser = loginedUser;
    }
}
