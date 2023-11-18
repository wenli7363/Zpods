#ifndef FILEFILTERDIALOG_H
#define FILEFILTERDIALOG_H

#include <QDialog>
#include "fs.h"
#include <map>
#include <QMessageBox>

namespace Ui {
    class FileFilterDialog;
}

using FileType = std::filesystem::file_type;

struct FilterConfig
{
    bool sizeChk, dateChk, typeChk, reChk;
    uintmax_t minSize , maxSize;
    std::chrono::year_month_day min_date, max_date;
    std::unordered_set<FileType> types;     // types to backup
    std::vector<std::string> re_list;       // regular expressions

    FilterConfig()
    {
        minSize = 0, maxSize = (uintmax_t)-1;
        sizeChk = dateChk = typeChk = reChk = false;
        min_date = zpods::fs::make_year_month_day(1, 1, 1);
        max_date = zpods::fs::make_year_month_day(9999,12,31);
        types = {FileType::regular};
    }

};


class FileFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileFilterDialog(QWidget *parent = nullptr);
    ~FileFilterDialog();

    void connectInit();
    void closeEvent(QCloseEvent *event);
signals:
    void sentFilterConfig(std::shared_ptr<FilterConfig> config);


private:
    Ui::FileFilterDialog *ui;
    bool dateChk, sizeChk, reChk, typeChk;  // the state of 4 filter checkBox, false initialy

    std::vector<std::string> reTmpList;
};

#endif // FILEFILTERDIALOG_H
