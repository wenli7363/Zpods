#include "periodicwidget.h"
#include "ui_periodicwidget.h"
#include <QCheckBox>
#include <QDebug>

PeriodicWidget::PeriodicWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeriodicWidget)
{
    ui->setupUi(this);
    ui->checkBox->setTristate(false);
    ui->spinBox->setEnabled(false);

    // enable spinBox by checkbox
    connect(ui->checkBox, &QCheckBox::stateChanged, this, [&](){
        bool periodicChecked = ui->checkBox->isChecked();
        if(periodicChecked){
             ui->spinBox->setEnabled(true);
        }else{
             ui->spinBox->setEnabled(false);
        }
    });
}

PeriodicWidget::~PeriodicWidget()
{
    delete ui;
}
