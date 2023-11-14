#ifndef FILEFILTERDIALOG_H
#define FILEFILTERDIALOG_H

#include <QDialog>
#include <filesystem>
#include <map>
#include <QMessageBox>

namespace Ui {
    class FileFilterDialog;
}

using FileType = std::filesystem::file_type;

struct FilterConfig
{
    bool sizeChk, dateChk, typeChk, reChk;
    int minSize = -1, maxSize = 0x3f3f3f3f;
    std::string min_date, max_date;
    std::unordered_set<FileType> types;  // types to backup
    std::vector<std::string> re_list; // regular expressions
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
    void sentFilterConfig(FilterConfig* config);


private:
    Ui::FileFilterDialog *ui;
    bool sizeChk, dateChk, reChk, typeChk;  // the state of 4 filter checkBox, false initialy
};

#endif // FILEFILTERDIALOG_H
