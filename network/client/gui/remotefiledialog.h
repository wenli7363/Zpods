#ifndef REMOTEFILEDIALOG_H
#define REMOTEFILEDIALOG_H

#include <QDialog>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QPair>
namespace Ui {
class RemoteFileDialog;
}

class RemoteFileDialog : public QDialog
{
    Q_OBJECT

public:
    QStringList selectedDLFiles;

    explicit RemoteFileDialog(QWidget *parent = nullptr, const QList<QPair<QString,QString>>& fileList =QList<QPair<QString,QString>>());
    ~RemoteFileDialog();

private:
    Ui::RemoteFileDialog *ui;
    QList<QPair<QString,QString>>  fileList;           // 远程服务器上的文件列表
};

#endif // REMOTEFILEDIALOG_H
