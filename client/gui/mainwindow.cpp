#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <network.h>
#include "zpods_lib.h"
#include "termios.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Zpods");
}

MainWindow::~MainWindow()
{
    delete ui;
}

