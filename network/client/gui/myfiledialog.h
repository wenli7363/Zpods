// 因为QT自带的文件对话框不能同时选择文件和文件夹/目录，这里我自定义了一个文件对话框
#ifndef MYFILEDIALOG_H
#define MYFILEDIALOG_H
#include <QFileDialog>
#include <QWidget>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QDialogButtonBox>

class MyFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    explicit MyFileDialog(QWidget *parent = nullptr);

public slots:
    void slot_myAccetp();

signals:

};
#endif // MYFILEDIALOG_H
