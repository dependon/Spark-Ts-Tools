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

QString detectLanguage(const QString &fileName)
{
    QMap<QString, QString> languageMap;
    languageMap["en"] = u8"English";
    languageMap["zh_CN"] = u8"中文";
    languageMap["zh_TW"] = u8"中文繁体";
    languageMap["zh_HK"] = u8"粤语";
    languageMap["es"] = u8"西班牙语";
    languageMap["pl"] = u8"波兰语";
    languageMap["ja"] = u8"日语";
    languageMap["de"] = u8"德语";
    languageMap["ko"] = u8"韩语";
    languageMap["it"] = u8"意大利语";
    languageMap["fr"] = u8"法语";
    languageMap["ru"] = u8"俄语";
    languageMap["pt"] = u8"葡萄牙语";
    languageMap["pt_BR"] = u8"葡萄牙语";
    languageMap["fi"] = u8"芬兰语";
    languageMap["vi"] = u8"越南语";
//    languageMap["tr"] = u8"土耳其语";
    languageMap["th"] = u8"泰语";
    languageMap["hu"] = u8"匈牙利语";
    languageMap["sv"] = u8"瑞典语";
    //languageMap["sk"] = u8"斯洛伐克语";
    languageMap["ro"] = u8"罗马尼亚语";
//    languageMap["ms"] = u8"马来语";

    languageMap["ar_sa"] = u8"阿拉伯语";
    languageMap["ar_iq"] = u8"阿拉伯语";
    languageMap["ar_eg"] = u8"阿拉伯语";
    languageMap["ar_ly"] = u8"阿拉伯语";
    languageMap["ar_dz"] = u8"阿拉伯语";
    languageMap["ar_ma"] = u8"阿拉伯语";
    languageMap["ar_tn"] = u8"阿拉伯语";
    languageMap["ar_om"] = u8"阿拉伯语";
    languageMap["ar_ye"] = u8"阿拉伯语";
    languageMap["ar_sy"] = u8"阿拉伯语";
    languageMap["ar_jo"] = u8"阿拉伯语";
    languageMap["ar_lb"] = u8"阿拉伯语";
    languageMap["ar_kw"] = u8"阿拉伯语";
    languageMap["ar_ae"] = u8"阿拉伯语";
    languageMap["ar_bh"] = u8"阿拉伯语";
    languageMap["ar_qa"] = u8"阿拉伯语";

    languageMap["af"] = u8"荷兰语";//南非
    languageMap["nl"] = u8"荷兰语";//标准
    languageMap["nl_be"] = u8"荷兰语";//比利时

    languageMap["et"] = u8"爱沙尼亚语";
    languageMap["bg"] = u8"保加利亚语";
    languageMap["cs"] = u8"捷克语";
    languageMap["da"] = u8"丹麦语";
    languageMap["el"] = u8"希腊语";



    QStringList list = fileName.split("_");
    QString languageCode = fileName.split("_").last().split(".").first();
    if(list.size()>2)
    {
        languageCode = list.at(list.size()-2)+"_"+languageCode;
    }

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

    ui->comboBox->addItem(u8"English", "en");
    ui->comboBox->addItem(u8"中文", "zh");
    ui->comboBox->addItem(u8"粤语", "yue");
    ui->comboBox->addItem(u8"文言文", "wyw");
    ui->comboBox->addItem(u8"日语", "jp");
    ui->comboBox->addItem(u8"韩语", "kor");
    ui->comboBox->addItem(u8"法语", "fra");
    ui->comboBox->addItem(u8"西班牙语", "spa");
    ui->comboBox->addItem(u8"泰语", "th");
    ui->comboBox->addItem(u8"阿拉伯语", "ara");
    ui->comboBox->addItem(u8"俄语", "ru");
    ui->comboBox->addItem(u8"葡萄牙语", "pt");
    ui->comboBox->addItem(u8"德语", "de");
    ui->comboBox->addItem(u8"意大利语", "it");
    ui->comboBox->addItem(u8"希腊语", "el");
    ui->comboBox->addItem(u8"荷兰语", "nl");
    ui->comboBox->addItem(u8"波兰语", "pl");
    ui->comboBox->addItem(u8"保加利亚语", "bul");
    ui->comboBox->addItem(u8"爱沙尼亚语", "est");
    ui->comboBox->addItem(u8"丹麦语", "dan");
    ui->comboBox->addItem(u8"芬兰语", "fin");
    ui->comboBox->addItem(u8"捷克语", "cs");
    ui->comboBox->addItem(u8"罗马尼亚语", "rom");
    ui->comboBox->addItem(u8"斯洛文尼亚语", "slo");
    ui->comboBox->addItem(u8"瑞典语", "swe");
    ui->comboBox->addItem(u8"匈牙利语", "hu");
    ui->comboBox->addItem(u8"中文繁体", "cht");
    ui->comboBox->addItem(u8"越南语", "vie");
    ui->comboBox->addItem(u8"其他", "other");

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxChanged);
    connect(m_pExcelWorker, &ExcelRW::error, this, &MainWindow::onReceiveMsg);
    connect(m_pTranslateWorker, &TranslateWorker::error, this, &MainWindow::onReceiveMsg);

    readConfig();

    connect(app,&AppObject::sigDebug,this,&MainWindow::slotDebug);

    setWindowTitle(u8"星火Qt翻译工具");
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
