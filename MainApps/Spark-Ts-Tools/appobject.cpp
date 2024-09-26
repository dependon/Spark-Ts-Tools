#include "appobject.h"

AppObject *AppObject::m_app = nullptr;
AppObject *AppObject::instance()
{
    if(!m_app)
    {
        m_app = new AppObject();
    }
    return m_app;
}

AppObject::AppObject(QObject *object)
    :QObject(object)
{

}
