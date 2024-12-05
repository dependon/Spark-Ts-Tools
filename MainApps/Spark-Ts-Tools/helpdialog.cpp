#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "appobject.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    setWindowTitle(u8"对照帮助");
    QMap<QString, QString> languageMap_ts = AppObject::instance()->languageMap_ts;
    QMap<QString, QString> languageMap_baidu = AppObject::instance()->languageMap_baidu;

    // 设置表格的列数为2，分别用于存放键和值
    ui->tableTs->setColumnCount(2);

    {
        // 开始往表格中填充数据，遍历QMap
        QMap<QString, QString>::const_iterator it;
        int row = 0;
        for (it = languageMap_ts.constBegin(); it!= languageMap_ts.constEnd(); ++it) {
            // 在表格中插入新行
            ui->tableTs->insertRow(row);

            // 将键（语言名称）放入第一列
            QTableWidgetItem* itemKey = new QTableWidgetItem(it.key());
            ui->tableTs->setItem(row, 0, itemKey);

            // 将值（语言代码）放入第二列
            QTableWidgetItem* itemValue = new QTableWidgetItem(it.value());
            ui->tableTs->setItem(row, 1, itemValue);

            row++;
        }
    }

    {
        // 开始往表格中填充数据，遍历QMap
        QMap<QString, QString>::const_iterator it;
        int row = 0;
        for (it = languageMap_baidu.constBegin(); it!= languageMap_baidu.constEnd(); ++it) {
            // 在表格中插入新行
            ui->tableBaidu->insertRow(row);

            // 将键（语言名称）放入第一列
            QTableWidgetItem* itemKey = new QTableWidgetItem(it.key());
            ui->tableBaidu->setItem(row, 0, itemKey);

            // 将值（语言代码）放入第二列
            QTableWidgetItem* itemValue = new QTableWidgetItem(it.value());
            ui->tableBaidu->setItem(row, 1, itemValue);

            row++;
        }
    }
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_close_clicked()
{
    this->close();
}
