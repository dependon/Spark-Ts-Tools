#include "XmlRW.h"

#include <QDebug>
#include <QDomNodeList>

#define ROOT_ELEMENT        "TS"
#define CONTEXT_ELEMENT     "context"
#define MESSAGE_ELEMENT     "message"
#define LOCATION_ELEMENT    "location"
#define SOURCE_ELEMENT      "source"
#define TRANSLATION_ELEMENT "translation"

XmlRW::XmlRW(QObject *parent) : QObject(parent)
{

}

void XmlRW::UpdateTranslateMap(QList<TranslateModel>& list)
{
    m_translateMap.clear();

    foreach (TranslateModel model, list) {
       m_translateMap[model.GetKey()] = model.GetTranslate();
    }
}

bool XmlRW::ImportFromTS(QList<TranslateModel>& list, QString strPath)
{
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    else {
        xml.setDevice(&file);
//        m_translateMap.clear();

        if (xml.readNextStartElement()) {
            QString strName = xml.name().toString();
            if (strName== ROOT_ELEMENT) {
                QXmlStreamAttributes attributes = xml.attributes();
                if (attributes.hasAttribute("version")) {
                    QString strVersion = attributes.value("version").toString();
                    qDebug() << "version : " << strVersion;
                }
                if (attributes.hasAttribute("language")) {
                    QString strLanguage = attributes.value("language").toString();
                    qDebug() << "language : " << strLanguage;
                }

                ReadXBEL();
            } else {
                xml.raiseError("XML file format error.");
            }
        }

        file.close();
        list.clear();

        for (auto i = m_translateMap.constBegin(); i != m_translateMap.constEnd(); i++) {
            TranslateModel model;
            model.SetKey(i.key());
            model.SetSource(i.value());
            model.SetTranslate(i.value());
            list.append(model);
        }

        return true;
    }
}

bool XmlRW::ExportToTS(QList<TranslateModel>& list, QString strPath)
{
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    else {
        if(list.count() <=0) {
            qDebug() << "translate list is empty";
            return false;
        }

        UpdateTranslateMap(list);

        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            qDebug() << "xml parsing error";
            return false;
        }
        file.close();

        QDomElement root=doc.documentElement();
        QDomNodeList list=root.elementsByTagName("message");

        QDomNode node;
        for(int i=0; i < list.count(); i++) {
            node = list.at(i);
            QDomNodeList childList = node.childNodes();
            QString strKey = childList.at(childList.count()-2).toElement().text();
            QString strTranslation = node.lastChild().toElement().text();
            QString strValue = m_translateMap.value(strKey);

            qDebug() << i << "\ttranslatation:" << strTranslation << "\t\tkey:" << strKey << "\t\tvalue:" << strValue;

            if(!strValue.isEmpty() && strTranslation != strValue) {
                QDomNode oldNode = node.lastChild();

                QDomElement newElement = doc.createElement("translation");
                QDomText text = doc.createTextNode(strValue);
                newElement.appendChild(text);
                node.replaceChild(newElement, oldNode);
            }
        }

        if(!file.open(QFile::WriteOnly|QFile::Truncate)) {
            return false;
        }

        QTextStream outStream(&file);
        doc.save(outStream, 4);
        file.close();

        return true;
    }
}

QString XmlRW::ErrorString() const
{
    return QString("Error:%1  Line:%2  Column:%3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}

void XmlRW::ReadXBEL()
{
    Q_ASSERT(xml.isStartElement() && xml.name().toString() == ROOT_ELEMENT);

    while (xml.readNextStartElement()) {
        if (xml.name().toString() == CONTEXT_ELEMENT) {
            ReadContext();
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XmlRW::ReadContext()
{
    Q_ASSERT(xml.isStartElement() && xml.name().toString() == CONTEXT_ELEMENT);

    while (xml.readNextStartElement()) {
        if (xml.name().toString() == MESSAGE_ELEMENT) {
            ReadMessage();
        }
        else {
            xml.skipCurrentElement();
        }
    }
}

void XmlRW::ReadMessage()
{
    Q_ASSERT(xml.isStartElement() && xml.name().toString() == MESSAGE_ELEMENT);

    QString strSource, strTranslation, strLoaction;

    while (xml.readNextStartElement()) {
        if (xml.name().toString() == SOURCE_ELEMENT) {
            strSource = xml.readElementText();
        } else if (xml.name().toString() == TRANSLATION_ELEMENT) {
            strTranslation = xml.readElementText();
        } else if (xml.name().toString() == LOCATION_ELEMENT) {
            strLoaction.clear();

            QXmlStreamAttributes attributes = xml.attributes();
            if (attributes.hasAttribute("filename")) {
                QString strFileName = attributes.value("filename").toString();
                strLoaction.append(QString("fileName: %1; ").arg(strFileName));
            }
            if (attributes.hasAttribute("line")) {
                QString strLine = attributes.value("line").toString();
                strLoaction.append(QString("line: %1; ").arg(strLine));
            }

            xml.skipCurrentElement();
        } else {
            xml.skipCurrentElement();
        }
    }

    qDebug() << "key:" << strSource << "\ttranslation:" << strTranslation;

    if(m_translateMap.contains(strSource)) {
        qDebug() << "repeat key: " << strSource << "translation:" << strLoaction;
    }
    m_translateMap.insert(strSource, strTranslation);
}
