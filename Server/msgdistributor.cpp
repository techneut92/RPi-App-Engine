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
    qDebug() << "DEBUGGING: Relay Message in msgdist called. currently contains clients: " << cc_clients.count();
    if (jmap["serverTarget"].toString() == "all"){
        // iterate through all clients with the same id and send a message to all client types except the origin
        //this->sorted_uids[""].append(1);
        foreach(int uid, sorted_uids[origin->getId()])
            if (uid != origin->uid) cc_clients[uid]->sendTextMessage(jmap["msgData"].toString());
    }
    else if(jmap["serverTarget"].toString() == "serverApp"){
        // iterate through all clients with the same id and send a message to all server apps except the origin
        foreach( int uid, sorted_uids[origin->getId()])
            if (uid != origin->uid && cc_clients[uid]->appType() == AppType::Server)
                cc_clients[uid]->sendTextMessage(jmap["msgData"].toString());
    }
    else if(jmap["serverTarget"].toString() == "clientApp"){
        // iterate through all clients with the same id and send a message to all client apps except the origin
        foreach( int uid, sorted_uids[origin->getId()])
            if (cc_clients[uid]->uid != origin->uid && cc_clients[uid]->appType() == AppType::WebClient)
               cc_clients[uid]->sendTextMessage(jmap["msgData"].toString());
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
        if (cc_clients[uid]->uid != origin->uid && cc_clients[uid]->appType() != origin->appType()) cc_clients[uid]->sendTextMessage(message);
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
