#include "msgdistributor.h"
#include "jsonhandler.h"
#include <QMap>
#include "clientmanager.h"

MsgDistributor::MsgDistributor(QObject *parent) : QObject(parent)
{

}

MsgDistributor::~MsgDistributor()
{

}

void MsgDistributor::setServer(ClientManager *c)
{
    this->cm = c;
}

// Function to relay messages to specific client groups
void MsgDistributor::relayMessage(QString message, Client *origin, QVariantMap jmap)
{
    QMap<int, Client*> cc_clients = this->cm->getClients();
    QMap<QString, QList<int>> sorted_uids = this->cm->getSortedClients();
    if (jmap["serverTarget"].toString() == "all"){
        // iterate through all clients with the same id and send a message to all client types except the origin
        foreach(int uid, sorted_uids[origin->getId()])
            if (uid != origin->uid) cc_clients[uid]->sendTextMessage(this->genPackage(origin, jmap["msgData"].toString()));
    }
    else if(jmap["serverTarget"].toString() == "serverApp"){
        // iterate through all clients with the same id and send a message to all server apps except the origin
        foreach( int uid, sorted_uids[origin->getId()])
            if (uid != origin->uid && cc_clients[uid]->appType() == AppType::Server)
                cc_clients[uid]->sendTextMessage(this->genPackage(origin, jmap["msgData"].toString()));
    }
    else if(jmap["serverTarget"].toString() == "clientApp"){
        // iterate through all clients with the same id and send a message to all client apps except the origin
        foreach( int uid, sorted_uids[origin->getId()])
            if (cc_clients[uid]->uid != origin->uid && cc_clients[uid]->appType() == AppType::WebClient)
                cc_clients[uid]->sendTextMessage(this->genPackage(origin, jmap["msgData"].toString()));
    }
    else if(jmap["serverTarget"].toString() == "uid"){
        bool ok;
        int uid = jmap["uid"].toInt(&ok);
        if (!ok) {
            qDebug() << "from uid:" << origin->uid << "app:" << origin->getId() <<
                        "int conversion of uid failed. message not send. uid:" << jmap["uid"].toString();
        }else
            cc_clients[uid]->sendTextMessage(this->genPackage(origin, jmap["msgData"].toString()));
    }
    else if (jmap["serverTarget"].toString() == "server"){
        //this->serverCommandManager(origin, jmap);
    }
    else relayMessage(message, origin);
}

// iterate through clients with same id and send messages to all different apptypes
void MsgDistributor::relayMessage(QString message, Client *origin)
{
    QMap<int, Client*> cc_clients = this->cm->getClients();
    QMap<QString, QList<int>> sorted_uids = this->cm->getSortedClients();
    foreach( int uid, sorted_uids[origin->getId()])
        if (cc_clients[uid]->uid != origin->uid && cc_clients[uid]->appType() != origin->appType()) cc_clients[uid]->sendTextMessage(this->genPackage(origin, message));
}

QString MsgDistributor::genPackage(Client *origin, QString msg)
{
    QJsonObject mainobject;
    QJsonObject originClient;

    originClient.insert("uid", QJsonValue::fromVariant(origin->uid));
    originClient.insert("appId", QJsonValue::fromVariant(origin->getId()));
    originClient.insert("peerAddress", QJsonValue::fromVariant(origin->getPeerAddress()));
    originClient.insert("peerName", QJsonValue::fromVariant(origin->getPeerName()));
    originClient.insert("peerOrigin", QJsonValue::fromVariant(origin->getOrigin()));
    if (origin->appType() == AppType::WebClient)
        originClient.insert("appType", QJsonValue::fromVariant("webClient"));
    else if (origin->appType() == AppType::Server)
        originClient.insert("appType", QJsonValue::fromVariant("serverClient"));
    else if (origin->appType() == AppType::UnkownType)
        originClient.insert("appType", QJsonValue::fromVariant("unkownType"));

    mainobject.insert("origin", originClient);
    mainobject.insert("msgData", msg);

    QJsonDocument doc(mainobject);
    QString new_msg = doc.toJson();
    return new_msg;
}

// process text messages and send them to the correct targets
void MsgDistributor::processTextMessages(QString message, Client* origin)
{
    qDebug() << "MsgDistributor::processTextMessages: " << message << origin->getId() << origin->appType() << origin->uid;
    if (jsonHandler::isValidJson(message)){
        QVariantMap jmap = jsonHandler::jsonStringToQMap(message);
        if (!jmap["serverTarget"].isNull())
            relayMessage(message, origin, jmap);
        else
            relayMessage(message, origin);
    }
    else
        qDebug() << "invalid message:" << message;
}
