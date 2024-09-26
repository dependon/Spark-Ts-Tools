#include "MainWindow.h"
#include <QApplication>
#include <QSslSocket>

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include "appobject.h"
#define INSTANCE_LOCK_PATH ".cache/ts_file"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    QString timestampS = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        txt = timestampS + QString(" Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = timestampS + QString(" Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = timestampS + QString(" Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = timestampS + QString(" Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = timestampS + QString(" Fatal: %1").arg(msg);
        break;
    }
    emit app->sigDebug(txt);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd");
#ifdef Q_OS_LINUX
    QString logFolder = QDir::homePath() + "/"+INSTANCE_LOCK_PATH +"/log";
    QDir().mkpath(logFolder); // 创建log文件夹

    QString logFileName = QString("%1/log/log_%2.txt").arg(QDir::homePath() + "/"+INSTANCE_LOCK_PATH ).arg(timestamp);
#else
    QString logFolder = "log";
    QDir().mkpath(logFolder); // 创建log文件夹

    QString logFileName = QString("%1/log/log_%2.txt").arg(QCoreApplication::applicationDirPath()).arg(timestamp);
#endif
    QFile outFile(logFileName);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outFile);
    textStream << txt << endl;

    // 删除10天前的日志文件
    QDir logDir(logFolder);
    QStringList filters;
    filters << "log_*.txt";
    logDir.setNameFilters(filters);

    QFileInfoList fileList = logDir.entryInfoList();
    for (QFileInfo fileInfo: fileList) {
        QDateTime fileTime = fileInfo.created();
        if (fileTime.daysTo(QDateTime::currentDateTime()) > 10) {
            QFile::remove(fileInfo.absoluteFilePath());
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(customMessageHandler);
    MainWindow w;
    w.show();
    qDebug() << u8"OpenSSL sslLibraryVersionNumber:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << u8"OpenSSL支持情况:" << QSslSocket::supportsSsl();
    return a.exec();
}
