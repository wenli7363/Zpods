#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "filefilterdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // use file filter
    bool enableFileFilter();

    // use remote
    bool enableRemote();

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDialog;
};
#endif // MAINWINDOW_H
