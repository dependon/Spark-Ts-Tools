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

    return languageMap.value(languageCode, QObject::tr(u8"其他"));

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

    setWindowTitle(tr(u8"星火Qt翻译工具"));

    ui->generateBtn_2->setVisible(false);
    ui->tsUpdateBtn_2->setVisible(false);

    on_btn_piliang_clicked();
    ui->hideBoxId->setCheckState(Qt::Checked);
    ui->hideBoxCode->setCheckState(Qt::Checked);
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
        onReceiveMsg(tr(u8"导入 excel 文件成功,正在翻译...... "));
    }
    else {
        onReceiveMsg(tr(u8"导入 excel 文件失败"));
    }


    //translate excel file
    m_pTranslateWorker->SetIdKey(ui->youdaoAppIdlineEdit->text(), ui->youdaoKeylineEdit->text());
    re = m_pTranslateWorker->Translate("auto", m_toLanguage);
    if(re) {
        onReceiveMsg(tr(u8"翻译 excel 文件成功"));
        ui->youdaoTipLabel->setVisible(true);
    }
    else {
        onReceiveMsg("翻译 excel 文件失败");
    }
}

void MainWindow::on_tsLookBtn_clicked()
{
    //const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr(u8"select .ts file"), nullptr, "Files (*.ts)");

    if(fileName.isEmpty()){
        return;
    }

    ui->tsPathEdit->setText(fileName);
    on_tsImportBtn_clicked();
}

void MainWindow::on_excelLookBtn_clicked()
{
    //const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr(u8"select excel file"), nullptr, "Files (*.xlsx)");

    if(fileName.isEmpty()){
        return;
    }
    else{
        QFileInfo info(fileName);
        if ("xlsx" != info.suffix()){
            onReceiveMsg(tr(u8"文件类型不支持"));
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
        onReceiveMsg(tr(u8"导出 excel 文件成功"));
        ui->youdaoTipLabel->setVisible(false);
    } else {
        onReceiveMsg(tr(u8"导出 excel 文件失败"));
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
        onReceiveMsg(tr(u8"导入 excel 文件成功"));
        ui->youdaoTipLabel->setVisible(false);
    } else {
        onReceiveMsg(tr(u8"导入 excel 文件失败"));
    }

    //update ts file
    if(ui->tsPathEdit->text().isEmpty()) {
        on_tsLookBtn_clicked();
    }

    re = m_pXmlWorker->ExportToTS(m_transList, ui->tsPathEdit->text());

    if(re) {
        onReceiveMsg(tr(u8"更新 .ts 文件 成功"));
    } else {
        onReceiveMsg(tr(u8"更新 .ts 文件 失败"));
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
        onReceiveMsg(tr(u8"文件类型不支持"));
        return;
    }

    m_transList.clear();
    re = m_pXmlWorker->ImportFromTS(m_transList, ui->tsPathEdit->text());

    if(re) {
        onReceiveMsg(tr(u8"导入 .ts 文件成功"));
    } else {
        onReceiveMsg(tr(u8"导入 .ts 文件失败"));
    }
}

void MainWindow::onReceiveMsg(const QString &msg)
{
    qDebug()<< msg;
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_tsDirLookBtn_clicked()
{
    const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString dirName = QFileDialog::getExistingDirectory(this, tr(u8"选择 .ts 目录"), documentLocation);

    if(dirName.isEmpty()){
        return;
    }

    ui->tsDirEdit->setText(dirName);
}

void MainWindow::on_excelDirBtn_clicked()
{
    const QString documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr(u8"选择Excel"), documentLocation, "Files (*.xlsx)");

    if(fileName.isEmpty()){
        return;
    }
    else{
        QFileInfo info(fileName);
        if ("xlsx" != info.suffix()){
            onReceiveMsg(tr(u8"文件类型不支持"));
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
        onReceiveMsg(tr(u8"ts 目录为空"));
        return;
    }

    QFileInfo excelinfo(ui->excelDirEdit->text());
    if (!excelinfo.exists()){
        onReceiveMsg("excel 文件是空的");
        return;
    }
    qDebug() << excelinfo.filePath() << excelinfo.absoluteDir().path();

    QStringList filters;
    filters << QString("*.ts");
    QDir tsdir(ui->tsDirEdit->text());
    tsdir.setFilter(QDir::Files | QDir::NoSymLinks);
    tsdir.setNameFilters(filters);

    if (tsdir.count() <= 0) {
        onReceiveMsg(tr(u8"ts 文件的ts文件是0"));
        return;
    }

    for (QFileInfo info : tsdir.entryInfoList()) {
        //import ts file
        m_transList.clear();
        re = m_pXmlWorker->ImportFromTS(m_transList, info.absoluteFilePath());

        if(re) {
            onReceiveMsg(tr(u8"导入 ") + info.fileName() + tr(u8" 成功"));
        } else {
            onReceiveMsg(tr(u8"导入 ") + info.fileName() + tr(u8" 失败"));
        }

        //generate excel file
        m_pExcelWorker->SetTransColumn(ui->transSpinBox->value());
        QString excelFileName = excelinfo.absoluteDir().path() + "/" + info.baseName() + ".xlsx";
        re = m_pExcelWorker->ExportToXlsx(m_transList, excelFileName);
        if(re) {
            onReceiveMsg(tr(u8"导入 ") + excelFileName + tr(u8" 成功"));
            ui->youdaoTipLabel->setVisible(false);
        } else {
            onReceiveMsg(tr(u8"导入 ") + excelFileName + tr(u8" 失败"));
        }
    }
}

void MainWindow::on_tsUpdateBtn_2_clicked()
{
    bool re;

    QFileInfo tsDirinfo(ui->tsDirEdit->text());
    if (!tsDirinfo.isDir()){
        onReceiveMsg(tr(u8"ts 目录是空的"));
        return;
    }

    QFileInfo excelDirinfo(ui->excelDirEdit->text());
    if (!excelDirinfo.exists()){
        onReceiveMsg(tr(u8"excel 路径是空的"));
        return;
    }

    QStringList filters;
    filters << QString("*.ts");
    QDir tsdir(ui->tsDirEdit->text());
    tsdir.setFilter(QDir::Files | QDir::NoSymLinks);
    tsdir.setNameFilters(filters);

    if (tsdir.count() <= 0) {
        onReceiveMsg(tr(u8"ts 文件的ts文件是0"));
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

    onReceiveMsg(tr(u8"所有ts文件更新完成"));
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
    //if(ui->comboBox->findText(strName))
    {
        ui->comboBox->setCurrentText(strName);
    }
    if(ui->comboBox->currentText() == tr(u8"其他"))
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
        qDebug() << tr(u8"First key-value pair: ") << firstKey << " : " << firstValue;
        m_waitTs.erase(m_waitTs.begin());
        ui->tsPathEdit->setText(firstKey);
        ui->excelPathEdit->setText(firstValue);
        on_tsImportBtn_clicked();
        on_generateBtn_clicked();
        TranslatFunction();
    } else {
        qDebug() << tr(u8"不存在可翻译的文件.");
    }

    ui->lbl_fileCount->setText(QString::number(m_waitTs.count()));
}

void MainWindow::onTransEnd()
{
    onReceiveMsg(tr(u8"翻译 完成 ...... "));
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
        qDebug() << tr(u8"翻译结束.");
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
    if(m_waitTs.count() == 0)
    {
          TranslatFunction();
    }
    else
    {
         QMessageBox::critical(this, tr(u8"错误"), tr(u8"需要等待当前批量翻译完成!"));
    }

}

void MainWindow::on_tsHelpBtn_clicked()
{
    HelpDialog dialog;
    dialog.exec();
}

void MainWindow::on_btn_ts_clicked()
{
    QString sourceTsPath = QFileDialog::getOpenFileName(this, tr(u8"select .ts file"), nullptr, "Files (*.ts)");
    if (sourceTsPath.isEmpty()) {
        onReceiveMsg(tr(u8"请先选择源 ts 文件"));
        return;
    }

    QFileInfo sourceFileInfo(sourceTsPath);
    QString sourceDir = sourceFileInfo.absolutePath();
    QString originalBaseName = sourceFileInfo.baseName(); // Original base name, possibly with lang_COUNTRY code
    QString actualBaseName = originalBaseName; // This will be the base name after stripping known lang codes

    // Define known language codes for stripping from the source filename.
    // These are sorted by length in descending order to ensure longer codes (e.g., "zh_CN") are matched before shorter ones (e.g., "zh").
    QStringList langCodeKeysForStripping;
    langCodeKeysForStripping << "en" << "zh_CN" << "zh_TW" << "zh_HK" << "es" << "pl" << "ja" << "jp" << "de" << "ko" << "kor" << "it"
                             << "fr" << "fra" << "ru" << "pt" << "pt_BR" << "fi" << "fin" << "vi" << "vie" << "th" << "hu" << "sv" << "swe" << "ro" << "rom"
                             << "ar_sa" << "ar_iq" << "ar_eg" << "ar_ly" << "ar_dz" << "ar_ma" << "ar_tn" << "ar_om"
                             << "ar_ye" << "ar_sy" << "ar_jo" << "ar_lb" << "ar_kw" << "ar_ae" << "ar_bh" << "ar_qa" << "ara"
                             << "af" << "nl" << "nl_be" << "et" << "est" << "bg" << "bul" << "cs" << "da" << "dan" << "el"
                             << "zh" << "yue" << "wyw" << "slo" << "cht";
    langCodeKeysForStripping.removeDuplicates();
    std::sort(langCodeKeysForStripping.begin(), langCodeKeysForStripping.end(), [](const QString& a, const QString& b) {
        return a.length() > b.length();
    });

    for (const QString& code : langCodeKeysForStripping) {
        QString suffixToTest = "_" + code;
        if (actualBaseName.endsWith(suffixToTest, Qt::CaseInsensitive)) {
            actualBaseName = actualBaseName.left(actualBaseName.length() - suffixToTest.length());
            break; // Found and stripped the longest matching known suffix
        }
    }
    // If no known language code was stripped, actualBaseName remains originalBaseName.

    QStringList targetLanguages = { // This list defines which languages to generate files for
        "en", "zh_CN", "zh_TW", "zh_HK", "es", "pl", "ja", "de", "ko", "it",
        "fr", "ru", "pt", "pt_BR", "fi", "vi", "th", "hu", "sv", "ro"
    };

    for (const QString &lang : targetLanguages) {
        // The old logic for deriving newBaseName by simply splitting at the last underscore is removed.
        // We now use the 'actualBaseName' which has been properly stripped of known language codes.
        QString targetFileName = QString("%1_%2.ts").arg(actualBaseName).arg(lang);
        QString targetTsPath = sourceDir + "/" + targetFileName;

        // 1. 复制 ts 文件
        if (QFile::copy(sourceTsPath, targetTsPath)) {
            onReceiveMsg(QString("成功复制 %1 到 %2").arg(sourceFileInfo.fileName()).arg(targetFileName));

            // 2. 修改语言属性并清空 translation 标签
            QFile file(targetTsPath);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                QString content = file.readAll();
                file.resize(0); // 清空文件内容以便写入修改后的内容

                QXmlStreamReader xmlReader(content);
                QXmlStreamWriter xmlWriter(&file);
                xmlWriter.setAutoFormatting(true);

                // 写入 XML 声明和 DOCTYPE 声明
                xmlWriter.writeStartDocument("1.0", true);
                xmlWriter.writeDTD("<!DOCTYPE TS>");

                while (!xmlReader.atEnd() && !xmlReader.hasError()) {
                    QXmlStreamReader::TokenType token = xmlReader.readNext();

                    if (token == QXmlStreamReader::StartElement) {
                        QString currentElementName = xmlReader.name().toString();
                        if (currentElementName.toLower() == QLatin1String("ts")) { // Case-insensitive check for "ts" or "TS"
                            xmlWriter.writeStartElement(QLatin1String("TS")); // Standardize to "TS" for output
                            // Copy all attributes from source TS element, except 'language'
                            for (const QXmlStreamAttribute &attr : xmlReader.attributes()) {
                                if (attr.name().toString().toLower() != QLatin1String("language")) {
                                    xmlWriter.writeAttribute(attr.name().toString(), attr.value().toString());
                                }
                            }
                            // Now, explicitly set the new language attribute
                            xmlWriter.writeAttribute(QLatin1String("language"), lang);
                        } else if (currentElementName == QLatin1String("translation")) {
                            xmlWriter.writeStartElement(QLatin1String("translation"));
                            xmlWriter.writeAttribute(QLatin1String("type"), QLatin1String("unfinished"));
                            xmlReader.skipCurrentElement(); // This consumes the original <translation>...</translation>
                            xmlWriter.writeEndElement(); // Closes the new <translation type="unfinished"></translation>
                        } else {
                            xmlWriter.writeStartElement(currentElementName);
                            xmlWriter.writeAttributes(xmlReader.attributes());
                        }
                    } else if (token == QXmlStreamReader::EndElement) {
                        QString currentEndElementName = xmlReader.name().toString();
                        if (currentEndElementName.toLower() == QLatin1String("ts")) {
                            xmlWriter.writeEndElement(); // Close our <TS>
                        } else if (currentEndElementName != QLatin1String("translation")) {
                            // We don't write an end element for "translation" here because
                            // the StartElement logic for "translation" now writes its own EndElement.
                            xmlWriter.writeEndElement();
                        }
                    } else if (token == QXmlStreamReader::Characters) {
                        xmlWriter.writeCharacters(xmlReader.text().toString());
                    } else if (token == QXmlStreamReader::Comment) {
                        xmlWriter.writeComment(xmlReader.text().toString());
                    } else if (token == QXmlStreamReader::ProcessingInstruction) {
                         xmlWriter.writeProcessingInstruction(xmlReader.processingInstructionTarget().toString(), xmlReader.processingInstructionData().toString());
                    }
                }

                if (xmlReader.hasError()) {
                    onReceiveMsg(QString(tr(u8"处理文件 %1 时出现 XML 错误: %2")).arg(targetFileName).arg(xmlReader.errorString()));
                }

                file.close();
                onReceiveMsg(QString(tr(u8"成功处理文件 %1)")).arg(targetFileName));

            } else {
                onReceiveMsg(QString(tr(u8"无法打开文件 %1 进行读写")).arg(targetFileName));
            }

        } else {
            onReceiveMsg(QString(tr(u8"复制文件 %1 到 %2 失败")).arg(sourceFileInfo.fileName()).arg(targetFileName));
        }
    }

    onReceiveMsg(tr(u8"批量生成 ts 文件完成"));
}

void MainWindow::on_btn_only_clicked()
{
    ui->groupBox_piliang->setVisible(false);
    ui->groupBox_only->setVisible(true);
}

void MainWindow::on_btn_piliang_clicked()
{
    ui->groupBox_piliang->setVisible(true);
    ui->groupBox_only->setVisible(false);
}

void MainWindow::on_hideBoxId_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->youdaoAppIdlineEdit->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->youdaoAppIdlineEdit->setEchoMode(QLineEdit::Normal);
    }
}

void MainWindow::on_hideBoxCode_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->youdaoKeylineEdit->setEchoMode(QLineEdit::Password);
    }
    else {
        ui->youdaoKeylineEdit->setEchoMode(QLineEdit::Normal);
    }
}
