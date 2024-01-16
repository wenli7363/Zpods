#include "remotefiledialog.h"
#include "ui_remotefiledialog.h"

RemoteFileDialog::RemoteFileDialog(
    QWidget* parent, const QList<QPair<QString, QString>>& fileList)
    : QDialog(parent), ui(new Ui::RemoteFileDialog)
{
    this->fileList = fileList;
    ui->setupUi(this);

    connect(ui->confirmBtn, &QPushButton::clicked, this, [this]() {
        selectedDLFiles = QStringList();
        // 获取选中的文件名并保存到QStringList中
        for (int row = 0; row < ui->remoteSrcTable->rowCount(); ++row)
        {
            QTableWidgetItem* item = ui->remoteSrcTable->item(row, 0);
            if (item && item->isSelected())
            {
                this->selectedDLFiles << item->text();
            }
        }

        // 打印选中的文件名（仅作为示例，实际中你可以根据需要使用这个QStringList）
        qDebug() << "Selected Files: " << this->selectedDLFiles;
        close();
    });

    ui->remoteSrcTable->setColumnCount(1);
    ui->remoteSrcTable->setRowCount(fileList.size());
    ui->remoteSrcTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    QStringList headerLabels;
    headerLabels << "网盘上的POD文件";
    ui->remoteSrcTable->setHorizontalHeaderLabels(headerLabels);
    for (int row = 0; row < fileList.size(); ++row)
    {
        QString fileName =
            fileList.at(row).first + "/" + fileList.at(row).second;

        // 创建新的 QTableWidgetItem，并将 fileName 设置为其文本
        QTableWidgetItem* item = new QTableWidgetItem(fileName);

        // 将 item 设置到指定的行和列
        ui->remoteSrcTable->setItem(row, 0, item);
    }
}

RemoteFileDialog::~RemoteFileDialog()
{
    delete ui;
}
