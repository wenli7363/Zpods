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

private:
    Ui::FileFilterDialog *ui;
};

#endif // FILEFILTERDIALOG_H
