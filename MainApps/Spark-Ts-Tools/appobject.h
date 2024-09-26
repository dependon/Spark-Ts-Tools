#ifndef APPOBJECT_H
#define APPOBJECT_H

#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QObject>
#include <QThread>

#include <QRect>
#include <QMutex>
#include <QVector>
#include <QDir>

#define app AppObject::instance()

class AppObject :public QObject
{
    Q_OBJECT
public:
    static AppObject *instance();
    AppObject(QObject * object = nullptr);
signals:
    void sigDebug(const QString & info);
private:
    static AppObject* m_app;
};

#endif // APPOBJECT_H
