#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "XmlRW.h"
#include "ExcelRW.h"
#include "TranslateWorker.h"
#include "appobject.h"

#include <QStandardPaths>
#include <QFileDialog>
#include <QListView>
#include <QSslSocket>
#include <QMessageBox>
#include "helpdialog.h"

QString detectLanguage(const QString &fileName)
{
    QMap<QString, QString> languageMap = AppObject::instance()->languageMap_ts;

    QStringList list = fileName.split("_");
    QString languageCode = fileName.split("_").last().split(".").first();
    if(list.size()>2)
    {
        languageCode = list.at(list.size()-2)+"_"+languageCode;
    }
    qDebug()<< list;
    qDebug()<<languageCode;

    return languageMap.value(languageCode, "其他");

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_toLanguage = u8"en";
    m_pXmlWorker = new XmlRW(this);
    m_pExcelWorker = new ExcelRW(1, 2, 3, this);
    m_pTranslateWorker = new TranslateWorker(m_transList, this);
    connect(m_pTranslateWorker,&TranslateWorker::sigEnd,this,&MainWindow::onTransEnd);

    ui->youdaoTipLabel->setVisible(false);
    ui->comboBox->setView(new QListView());
    QMap<QString, QString> languageMap_baidu = AppObject::instance()->languageMap_baidu;

    // 开始往表格中填充数据，遍历QMap
    QMap<QString, QString>::const_iterator it;
    for (it = languageMap_baidu.constBegin(); it!= languageMap_baidu.constEnd(); ++it)
    {
        ui->comboBox->addItem(it.key(), it.value());
    }

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxChanged);
    connect(m_pExcelWorker, &ExcelRW::error, this, &MainWindow::onReceiveMsg);
    connect(m_pTranslateWorker, &TranslateWorker::error, this, &MainWindow::onReceiveMsg);

    readConfig();

    connect(app,&AppObject::sigDebug,this,&MainWindow::slotDebug);

    setWindowTitle(u8"星火Qt翻译工具");

    ui->generateBtn_2->setVisible(false);
    ui->tsUpdateBtn_2->setVisible(false);
}

MainWindow::~MainWindow()
{
    saveConfig();

    delete ui;
}

void MainWindow::TranslatFunction()
{
    bool re;

    m_pExcelWorker->SetTransColumn(ui->transSpinBox->value());

    //import excel file
    if(ui->excelPathEdit->text().isEmpty()) {
        on_excelLookBtn_clicked();
    }

    re = m_pExcelWorker->ImportFromXlsx(m_transList, ui->excelPathEdit->text());
    if(re) {
        onReceiveMsg("import excel file success,Translation Ing ...... ");
    }
    else {
        onReceiveMsg("import excel file failed");
    }


    //translate excel file
    m_pTranslateWorker->SetIdKey(ui->youdaoAppIdlineEdit->text(), ui->youdaoKeylineEdit->text());
    re = m_pTranslateWorker->Translate("auto", m_toLanguage);
    if(re) {
        onReceiveMsg("translate excel file success");
        ui->youdaoTipLabel->setVisible(true);
    }
    else {
        onReceiveMsg("translate excel file failed");
    }
}

void MainWindow::on_tsLookBtn_clicked()
{
    //const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("select .ts file"), nullptr, "Files (*.ts)");

    if(fileName.isEmpty()){
        return;
    }

    ui->tsPathEdit->setText(fileName);
    on_tsImportBtn_clicked();
}

void MainWindow::on_excelLookBtn_clicked()
{
    //const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("select excel file"), nullptr, "Files (*.xlsx)");

    if(fileName.isEmpty()){
        return;
    }
    else{
        QFileInfo info(fileName);
        if ("xlsx" != info.suffix()){
            onReceiveMsg("File type is not supported");
            return;
        }
    }

    ui->excelPathEdit->setText(fileName);
}

void MainWindow::on_generateBtn_clicked()
{
    bool re;

    m_pExcelWorker->SetTransColumn(ui->transSpinBox->value());

    //generate excel file
    if(ui->excelPathEdit->text().isEmpty()) {
        const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString saveName = documentLocation + "/untitled.xlsx";
        QString fileName = QFileDialog::getSaveFileName(this, "excel file path", saveName, "Files (*.xlsx)");

        if (fileName.isEmpty())
        {
            return;
        }
        else{
            ui->excelPathEdit->setText(fileName);
        }
    }
    re = m_pExcelWorker->ExportToXlsx(m_transList, ui->excelPathEdit->text());
    if(re) {
        onReceiveMsg("export excel file success");
        ui->youdaoTipLabel->setVisible(false);
    } else {
        onReceiveMsg("export excel file failed");
    }
}

void MainWindow::on_tsUpdateBtn_clicked()
{
    bool re;

    m_pExcelWorker->SetTransColumn(ui->transSpinBox->value());

    //import excel file
    if(ui->excelPathEdit->text().isEmpty()) {
        on_excelLookBtn_clicked();
    }

    re = m_pExcelWorker->ImportFromXlsx(m_transList, ui->excelPathEdit->text());
    if(re) {
        onReceiveMsg("import excel file success");
        ui->youdaoTipLabel->setVisible(false);
    } else {
        onReceiveMsg("import excel file failed");
    }

    //update ts file
    if(ui->tsPathEdit->text().isEmpty()) {
        on_tsLookBtn_clicked();
    }

    re = m_pXmlWorker->ExportToTS(m_transList, ui->tsPathEdit->text());

    if(re) {
        onReceiveMsg("update .ts file success");
    } else {
        onReceiveMsg("update .ts file failed");
    }

}

void MainWindow::on_translateBtn_clicked()
{
    if(m_waitTs.count() == 0)
    {
          TranslatFunction();
    }
    else
    {
         QMessageBox::critical(this, "错误", "需要等待当前批量翻译完成!");
    }

}

void MainWindow::onComboBoxChanged(int)
{
    QString langCode = ui->comboBox->currentData().toString();

    if ("other" == langCode) {
        m_toLanguage = ui->otherLineEdit->text();
    } else {
        m_toLanguage = langCode;
    }

}



void MainWindow::on_tsImportBtn_clicked()
{
    bool re;

    //import .ts file
    if(ui->tsPathEdit->text().isEmpty()) {
        on_tsLookBtn_clicked();
    }

    QFileInfo info(ui->tsPathEdit->text());
    if (!info.isFile() || "ts" != info.suffix()){
        onReceiveMsg("File type is not supported");
        return;
    }

    m_transList.clear();
    re = m_pXmlWorker->ImportFromTS(m_transList, ui->tsPathEdit->text());

    if(re) {
        onReceiveMsg("import .ts file success");
    } else {
        onReceiveMsg("import .ts file failed");
    }
}

void MainWindow::onReceiveMsg(const QString &msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_tsDirLookBtn_clicked()
{
    const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString dirName = QFileDialog::getExistingDirectory(this, tr("select .ts dir"), documentLocation);

    if(dirName.isEmpty()){
        return;
    }

    ui->tsDirEdit->setText(dirName);
}

void MainWindow::on_excelDirBtn_clicked()
{
    const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("select excel file"), documentLocation, "Files (*.xlsx)");

    if(fileName.isEmpty()){
        return;
    }
    else{
        QFileInfo info(fileName);
        if ("xlsx" != info.suffix()){
            onReceiveMsg("File type is not supported");
            return;
        }
    }

    ui->excelDirEdit->setText(fileName);
}

void MainWindow::on_generateBtn_2_clicked()
{
    bool re;

    QFileInfo tsDirinfo(ui->tsDirEdit->text());
    if (!tsDirinfo.isDir()){
        onReceiveMsg("ts dir is empty");
        return;
    }

    QFileInfo excelinfo(ui->excelDirEdit->text());
    if (!excelinfo.exists()){
        onReceiveMsg("excel file is empty");
        return;
    }
    qDebug() << excelinfo.filePath() << excelinfo.absoluteDir().path();

    QStringList filters;
    filters << QString("*.ts");
    QDir tsdir(ui->tsDirEdit->text());
    tsdir.setFilter(QDir::Files | QDir::NoSymLinks);
    tsdir.setNameFilters(filters);

    if (tsdir.count() <= 0) {
        onReceiveMsg("ts dir ts file is 0");
        return;
    }

    for (QFileInfo info : tsdir.entryInfoList()) {
        //import ts file
        m_transList.clear();
        re = m_pXmlWorker->ImportFromTS(m_transList, info.absoluteFilePath());

        if(re) {
            onReceiveMsg("import " + info.fileName() + " success");
        } else {
            onReceiveMsg("import " + info.fileName() + " failed");
        }

        //generate excel file
        m_pExcelWorker->SetTransColumn(ui->transSpinBox->value());
        QString excelFileName = excelinfo.absoluteDir().path() + "/" + info.baseName() + ".xlsx";
        re = m_pExcelWorker->ExportToXlsx(m_transList, excelFileName);
        if(re) {
            onReceiveMsg("export " + excelFileName + " success");
            ui->youdaoTipLabel->setVisible(false);
        } else {
            onReceiveMsg("export " + excelFileName + " failed");
        }
    }
}

void MainWindow::on_tsUpdateBtn_2_clicked()
{
    bool re;

    QFileInfo tsDirinfo(ui->tsDirEdit->text());
    if (!tsDirinfo.isDir()){
        onReceiveMsg("ts dir is empty");
        return;
    }

    QFileInfo excelDirinfo(ui->excelDirEdit->text());
    if (!excelDirinfo.exists()){
        onReceiveMsg("excel path is empty");
        return;
    }

    QStringList filters;
    filters << QString("*.ts");
    QDir tsdir(ui->tsDirEdit->text());
    tsdir.setFilter(QDir::Files | QDir::NoSymLinks);
    tsdir.setNameFilters(filters);

    if (tsdir.count() <= 0) {
        onReceiveMsg("ts dir ts file is 0");
        return;
    }

    for (QFileInfo info : tsdir.entryInfoList()) {
        if (!m_tsColumnMap.contains(info.fileName())) {
            continue;
        }

        //import ts file
        m_transList.clear();
        re = m_pXmlWorker->ImportFromTS(m_transList, info.absoluteFilePath());

        if(!re) {
            continue;
        }

        m_pExcelWorker->SetTransColumn(m_tsColumnMap[info.fileName()]);
        re = m_pExcelWorker->ImportFromXlsx(m_transList, ui->excelDirEdit->text());
        if(!re) {
            continue;
        }

        re = m_pXmlWorker->ExportToTS(m_transList, info.absoluteFilePath());

        if(!re) {
            continue;
        }
    }

    onReceiveMsg("all ts file update finish");
}

void MainWindow::readConfig()
{
    QString configPath = QApplication::applicationDirPath();
#if __DEBUG
    configPath.append("/../Config");
#endif
    QSettings settings(configPath + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("path");
    ui->tsPathEdit->setText(settings.value("tsPath").toString());
    ui->tsDirEdit->setText(settings.value("tsDir").toString());

    ui->excelPathEdit->setText(settings.value("excelPath").toString());
    ui->excelDirEdit->setText(settings.value("excelPath2").toString());

    ui->youdaoAppIdlineEdit->setText(settings.value("appid").toString());
    ui->youdaoKeylineEdit->setText(settings.value("key").toString());

    settings.endGroup();

    m_tsColumnMap.clear();
    int size = settings.beginReadArray("languages");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        int column = settings.value("column").toInt();
        QString fileName = settings.value("tsFile").toString();
        m_tsColumnMap[fileName] = column;
    }
    settings.endArray();
}

void MainWindow::saveConfig()
{
    QString configPath = QApplication::applicationDirPath();
#if __DEBUG
    configPath.append("/../Config");
#endif
    QSettings settings(configPath + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("path");
    settings.setValue("tsPath", ui->tsPathEdit->text());
    settings.setValue("tsDir", ui->tsDirEdit->text());
    settings.setValue("excelPath", ui->excelPathEdit->text());
    settings.setValue("excelPath2", ui->excelDirEdit->text());
    settings.setValue("appid",ui->youdaoAppIdlineEdit->text());
    settings.setValue("key",ui->youdaoKeylineEdit->text());

    settings.endGroup();
}

void MainWindow::on_otherLineEdit_textChanged(const QString &arg1)
{
    QString langCode = ui->comboBox->currentData().toString();
    if ("other" == langCode) {
         m_toLanguage = arg1;
    }
}

void MainWindow::on_tsPathEdit_textChanged(const QString &arg1)
{
    QString strName = detectLanguage(arg1);
    if(ui->comboBox->findText(strName))
    {
        ui->comboBox->setCurrentText(strName);
    }
    if(ui->comboBox->currentText() == u8"其他")
    {
        ui->otherLineEdit->setText("");
    }
}


void MainWindow::on_plTrans_clicked()
{
    m_isPL = true;
    m_waitTs.clear();
    QString path = ui->excelDirEdit->text();
    QDir directory(path);
    QStringList filters;
    filters << "*.ts";
    directory.setNameFilters(filters);
    QStringList tsFiles = directory.entryList();

    for (const QString &file : tsFiles) {
        QString xlsx = directory.path()+"/"+file;
        xlsx = xlsx.left(xlsx.lastIndexOf(".ts"));
        xlsx += ".xlsx";
        m_waitTs .insert(directory.path()+"/"+file,xlsx);
    }
    if (!m_waitTs.isEmpty()) {
        auto firstKey = m_waitTs.begin().key();
        auto firstValue = m_waitTs.begin().value();
        qDebug() << "First key-value pair: " << firstKey << " : " << firstValue;
        m_waitTs.erase(m_waitTs.begin());
        ui->tsPathEdit->setText(firstKey);
        ui->excelPathEdit->setText(firstValue);
        on_tsImportBtn_clicked();
        on_generateBtn_clicked();
        TranslatFunction();
    } else {
        qDebug() << "不存在可翻译的文件.";
    }

    ui->lbl_fileCount->setText(QString::number(m_waitTs.count()));
}

void MainWindow::onTransEnd()
{
    onReceiveMsg("Translation OK ...... ");
    on_generateBtn_clicked();
    on_tsUpdateBtn_clicked();

    ui->lbl_fileCount->setText(QString::number(m_waitTs.count()));

    if (!m_waitTs.isEmpty()) {
        auto firstKey = m_waitTs.begin().key();
        auto firstValue = m_waitTs.begin().value();
        qDebug() << "First key-value pair: " << firstKey << " : " << firstValue;
        m_waitTs.erase(m_waitTs.begin());
        ui->tsPathEdit->setText(firstKey);
        ui->excelPathEdit->setText(firstValue);
        on_tsImportBtn_clicked();
        on_generateBtn_clicked();
        TranslatFunction();
    } else {
        qDebug() << "翻译结束.";
    }
    ui->lbl_fileCount->setText(QString::number(m_waitTs.count()));
}

void MainWindow::slotDebug(const QString &info)
{
    ui->textEdit_log->append(info);
}


void MainWindow::on_clearLog_clicked()
{
    ui->textEdit_log->clear();
}

void MainWindow::on_clearPl_clicked()
{
    m_waitTs.clear();
    ui->lbl_fileCount->setText(QString::number(m_waitTs.count()));
}

void MainWindow::on_tsKBtn_clicked()
{
    on_translateBtn_clicked();
}

void MainWindow::on_tsHelpBtn_clicked()
{
    HelpDialog dialog;
    dialog.exec();
}
