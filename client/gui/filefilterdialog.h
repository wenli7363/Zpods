#ifndef FILEFILTERDIALOG_H
#define FILEFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class FileFilterDialog;
}

class FileFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileFilterDialog(QWidget *parent = nullptr);
    ~FileFilterDialog();

    void connectInit();

private:
    Ui::FileFilterDialog *ui;
    bool sizeChk, dateChk, reChk, typeChk;  // the state of 4 filter checkBox, false initialy
};

#endif // FILEFILTERDIALOG_H
