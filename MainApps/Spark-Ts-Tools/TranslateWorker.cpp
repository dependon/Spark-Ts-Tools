#include "TranslateWorker.h"
#include <NetWorker.h>
#include <QCryptographicHash>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

TranslateWorker::TranslateWorker(QList<TranslateModel> &list, QObject *parent) : QObject(parent),
    m_list(list)
{
    m_pNetWorker = NetWorker::instance();

    connect(this, &TranslateWorker::translateResult, this, &TranslateWorker::onTranslateResult);
    connect(this, &TranslateWorker::sigNext,this, &TranslateWorker::onNext);
}

TranslateWorker::~TranslateWorker()
{

}

void TranslateWorker::SetIdKey(const QString &id, const QString &key)
{
    m_appId = id;
    m_appKey = key;
}

bool TranslateWorker::Translate(const QString &from, const QString &to)
{
    if(m_list.count() <=0) {
        emit error("translate list is empty");
        return false;
    }

    m_fromLang = from;
    m_toLang = to;
    m_current =0 ;
    m_max = m_list.count();
    emit sigNext();

    return true;
}

void TranslateWorker::YoudaoTranslate(int index, const QString &source)
{
    QString baseUrl = QString("https://openapi.youdao.com/api");

    QString uuid = QUuid::createUuid().toString();
    uuid.remove("{").remove("}").remove("-");
    QString timestamp = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());

    QUrlQuery query;
    qDebug()<<source.toUtf8().toPercentEncoding();
    query.addQueryItem("q", source.toUtf8().toPercentEncoding());
    query.addQueryItem("from", m_fromLang);
    query.addQueryItem("to", m_toLang);
    query.addQueryItem("appKey", m_appId);
    query.addQueryItem("salt", uuid);
    query.addQueryItem("sign", GetYoudaoSign(source, uuid, timestamp));
    query.addQueryItem("signType", "v3");
    query.addQueryItem("curtime", timestamp);

    QNetworkReply *pReply = (m_pNetWorker->get(baseUrl, query));

    connect(pReply, &QNetworkReply::finished, this, [=](){
        if (pReply->error() != QNetworkReply::NoError) {
            emit error("Http error: " + pReply->errorString());
        } else {
//            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//            int nStatusCode = variant.toInt();
//            qDebug() << "Status Code :" << nStatusCode;

            QByteArray replyData = pReply->readAll();

            qDebug() <<"73:" <<replyData;

            QJsonParseError jsonError;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(replyData, &jsonError);
            if (jsonError.error == QJsonParseError::NoError) {
                if (!(jsonDocument.isNull() || jsonDocument.isEmpty()) && jsonDocument.isObject()) {
                    QVariantMap data = jsonDocument.toVariant().toMap();
//                    qDebug() <<"88:"<< data;
                    int errorcode = data[QLatin1String("errorCode")].toInt();

                    if(0 == errorcode){
                        QVariantList detailList = data[QLatin1String("translation")].toList();
                        QString str = detailList.first().toString();

                        emit translateResult(index, str);
                    }

                }
            } else {
                emit error(jsonError.errorString());
            }
        }

        pReply->deleteLater();
        if(m_current!=m_list.count())
        {
            m_current++;
            emit sigNext();
        }
        else
        {
            emit sigEnd();
        }

    });
}

void TranslateWorker::baiduTranslate(int index, const QString &source)
{
    if(!m_Manager)
    {
         m_Manager=new QNetworkAccessManager(this);
         connect(m_Manager,&QNetworkAccessManager::finished,this,&TranslateWorker::replyFinished,Qt::DirectConnection);
    }
    m_index = index;

    QString from = m_fromLang;
    QString to = m_toLang;
    QString MD5;
    char salt[60];
    sprintf(salt,"%d",rand());  //获取随机数
    QString cText = source; //获取代翻译的文本
    QString sign=QString("%1%2%3%4").arg(m_appId).arg(cText).arg(salt).arg(m_appKey);//连接加密文件
    // qDebug ()<<sign;
    QByteArray str = QCryptographicHash::hash(sign.toUtf8(),QCryptographicHash::Md5);
    MD5.append(str.toHex());//生成md5加密文件
    // qDebug()<<MD5;
    QString myurl=QString("http://api.fanyi.baidu.com/api/trans/vip/translate?"
                   "q=%1&from=%2&to=%3&appid=%4"
                   "&salt=%5&sign=%6").arg(cText).arg(from).arg(to).arg(m_appId).arg(salt).arg(MD5);//连接上传文本
    //qDebug()<<myurl;
    m_Manager->get(QNetworkRequest(QUrl(myurl)));//发送上传；
}

QByteArray TranslateWorker::GetYoudaoSign(const QString &source, const QString &uuid, const QString &timestamp)
{
    QByteArray sign;

    QString str = m_appId + source + uuid + timestamp + m_appKey;

//    qDebug() << "str: " << str;

    sign = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256).toHex().toUpper();

//    qDebug() << sign;

    return sign;
}

void TranslateWorker::onTranslateResult(int index, const QString &str)
{
    TranslateModel model = m_list.at(index);
    model.SetTranslate(str);
    m_list.replace(index, model);

    qDebug() << "key: " << m_list.at(index).GetKey() << "\tsource: " << m_list.at(index).GetSource() << "\ttranslation: " << m_list.at(index).GetTranslate();
}

void TranslateWorker::onNext()
{
    if(m_current < m_list.count())
    {
        if(m_list.at(m_current).GetTranslate().isNull())
        {
            baiduTranslate(m_current, m_list.at(m_current).GetKey());
        }
        else
        {
            qDebug()<< "cun zai fan yi" <<m_list.at(m_current).GetTranslate();
            m_current++;
            onNext();
        }
    }
    else {
        m_current = 0;
        emit sigEnd();
    }
}

int TranslateWorker::replyFinished(QNetworkReply *reply)
{

    QJsonParseError jsonError;
    QByteArray all=reply->readAll();//获得api返回值
   // qDebug()<<all;
    QJsonDocument json = QJsonDocument::fromJson(all, &jsonError);
    QJsonObject object = json.object();//jion转码；
    QString cResult;
    if(object.contains("error_code"))
    {
        int nResult=object.value("error_code").toInt();

        switch (nResult) {
        case 52001:
            cResult ="52001 请求超时 重试";
            break;
        case 52002:
            cResult ="52002 系统错误 重试";
            break;
        case 54000:
            cResult ="54000 必填参数为空";
            break;
        case 54001:
            cResult ="54001 签名错误";
            break;
        case 54003:
            cResult ="54003 速度过快访问频率受限";
            break;
        case 54004:
            cResult ="54004 账户余额不足";
            break;
        case 54005:
            cResult ="54005 请求频繁";
            break;
        case 58002:
            cResult ="58002 服务关闭";
            break;
        default:
            cResult ="其他错误";
            break;
        }
        qDebug()<< m_index<<": " <<cResult;
    }
    else {
        QJsonArray value = object.value("trans_result").toArray();//一次解码
        /*
         {"from":"en","to":"zh",
           "trans_result":[{"src":"apple","dst":"\u82f9\u679c"}]}
           第一次解码
         */
        QJsonObject object1=value.at(0).toObject();//二次解码开【】括号；
        /*
           {"src":"apple","dst":"\u82f9\u679c"}
           第二次解码
         */
        //from=object.value("from").toString();
        cResult=object1.value("dst").toString();//得到翻译结果
        qDebug()<< m_index<<": " <<cResult;
        emit translateResult(m_index, cResult);
    }


    reply->deleteLater();
    if(m_current!=m_list.count())
    {
        m_current++;
        emit sigNext();
    }
    return 1;
}
