#ifndef TRANSLATEWORKER_H
#define TRANSLATEWORKER_H

#include <QObject>
#include "DataModel/TranslateModel.h"
#include <QNetworkReply>
class QNetworkAccessManager;
class NetWorker;

class TranslateWorker : public QObject
{
    Q_OBJECT
public:

    explicit TranslateWorker(QList<TranslateModel> &list, QObject *parent = nullptr);
    ~TranslateWorker();

    void SetIdKey(const QString& id, const QString& key);
    bool Translate(const QString &from = QString("auto"), const QString &to = QString("en"));

signals:
    void translateResult(int index, const QString &str);
    void error(const QString& msg);
    void sigNext();

    // end
    void sigEnd();
private slots:
    void onTranslateResult(int index, const QString &str);

    void onNext();
    int replyFinished(QNetworkReply *reply);//获得翻译返回的结果
private:
    QByteArray GetYoudaoSign(const QString &source, const QString& uuid, const QString& timestamp);
    void YoudaoTranslate(int index, const QString &source);
    void baiduTranslate(int index, const QString &source);

    NetWorker*              m_pNetWorker;

    QString                 m_fromLang;
    QString                 m_toLang;
    QString                 m_appId;
    QString                 m_appKey;
    QList<TranslateModel>   &m_list;
    bool isok = true;
    int m_current = 0;
    int m_max=0;
    QNetworkAccessManager *m_Manager{nullptr};
    int m_index= 0;
};

#endif // TRANSLATEWORKER_H
