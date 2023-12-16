#include "myfiledialog.h"

MyFileDialog::MyFileDialog(QWidget *parent) : QFileDialog(parent)
{

}


void MyFileDialog::slot_myAccetp()
{
    QDialog::accept();
}
