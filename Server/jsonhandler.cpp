#include "jsonhandler.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

jsonHandler::jsonHandler(QObject *parent) : QObject(parent)
{

}

QVariantMap jsonHandler::jsonStringToQMap(QString data)
{
    if (isValidJson(data)){
        QByteArray jsondata;
        jsondata.append(data);
        QJsonDocument doc = QJsonDocument::fromJson(jsondata);
        QJsonObject jsonObject = doc.object();
        QVariantMap jsonMap = jsonObject.toVariantMap();
        return jsonMap;
    }
    else {
        QVariantMap rt;
        return rt;
    }
}

bool jsonHandler::isValidJson(QString data)
{
    QByteArray jsondata;
    jsondata.append(data);
    QJsonDocument doc = QJsonDocument::fromJson(jsondata);
    return !doc.isNull();
}
