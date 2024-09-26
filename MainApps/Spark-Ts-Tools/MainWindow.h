#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include "DataModel/TranslateModel.h"

namespace Ui {
class MainWindow;
}

class XmlRW;
class ExcelRW;
class TranslateWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void TranslatFunction();
private slots:
    void on_tsLookBtn_clicked();
    void on_excelLookBtn_clicked();
    void on_generateBtn_clicked();
    void on_tsUpdateBtn_clicked();
    void on_translateBtn_clicked();
    void on_tsImportBtn_clicked();

    void onComboBoxChanged(int);
    void onReceiveMsg(const QString& msg);

    void on_tsDirLookBtn_clicked();
    void on_excelDirBtn_clicked();
    void on_generateBtn_2_clicked();
    void on_tsUpdateBtn_2_clicked();

    void on_otherLineEdit_textChanged(const QString &arg1);

    void on_tsPathEdit_textChanged(const QString &arg1);

    void on_plTrans_clicked();

    void onTransEnd();

private slots:
    void slotDebug(const QString &info);
    void on_clearLog_clicked();

    void on_clearPl_clicked();

private:
    Ui::MainWindow*         ui;

    QList<TranslateModel>  m_transList;

    QString                 m_toLanguage;

    XmlRW*                  m_pXmlWorker;
    ExcelRW*                m_pExcelWorker;
    TranslateWorker*        m_pTranslateWorker;

    QMap<QString, int>      m_tsColumnMap;

    QMap <QString,QString >      m_waitTs;
    void readConfig();
    void saveConfig();

    bool m_isPL = false;
};

#endif // MAINWINDOW_H
