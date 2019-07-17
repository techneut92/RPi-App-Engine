#include "clientmanager.h"
#include <QJsonArray>

ClientManager::ClientManager(MsgDistributor *md, QObject *parent) : QObject(parent)
{
    this->msgDistributor = md;
    this->msgDistributor->setServer(this);
}

QMap<int, Client *> ClientManager::getClients() { return this->clients; }
QMap<QString, QList<int> > ClientManager::getSortedClients() { return this->sorted_uids; }
bool ClientManager::uidTaken(int uid) { return this->clients.keys().contains(uid); }

void ClientManager::appendClient(Client *c)
{
    this->clients.insert(c->getUID(), c);
    this->sorted_uids[c->getAppID()].append(c->getUID());
    c->connectSL(this, this->msgDistributor);

    QString clientsString;

    qDebug() << "app" << c->getAppID() << "uid:" << c->getUID() << "Ready to process data";
    c->sendTextMessage("HANDSHAKE_SUCCESS " +
                       this->getClientsPackage(c->getUID()).replace(" ", "") +
                       " " +
                       this->getClientsPackage(c->getAppID(), c->getUID()).replace(" ", ""));
    this->notifyOthersNewClient(c);
}

void ClientManager::onDisconnect(Client *c)
{
    //disconnect(c, &Client::disconnected, this, &ClientManager::onDisconnect);

    qDebug() << "Client Disconnected" << c->getAppID() << c->getUID();
    this->notifyOthersClientDisconnected(c);
    //disconnect(c, &Client::textMessageReceived, this->msgDistributor, &MsgDistributor::processTextMessages);
    this->clients.remove(c->getUID()); // TODO FIX, segmentation errors??

    QMutableListIterator<int> i(this->sorted_uids[c->getAppID()]);
    while (i.hasNext()) {
        if (i.next() == c->getUID())
            i.remove();
    }
    if (this->sorted_uids[c->getAppID()].count() == 0)
        this->sorted_uids.remove(c->getAppID());
    delete c;
}


QString ClientManager::getClientsPackage(QString id, int excluded_uid)
{
    QJsonObject  mainObject;
    QJsonArray clientsArray;

    mainObject.insert("action", QJsonValue::fromVariant("getClients"));

    foreach(int uid, this->sorted_uids[id])
        if (excluded_uid < 0 || (excluded_uid >= 0 && excluded_uid != uid))
            clientsArray.push_back(this->getClientJsonObject(this->clients[uid]));
    mainObject.insert("clients", clientsArray);

    QJsonDocument  json(mainObject);
    QString jsonString = json.toJson();
    return this->genPackage(jsonString);
}

QString ClientManager::getClientsPackage(int uid)
{
    QJsonObject  mainObject;

    mainObject.insert("action", QJsonValue::fromVariant("initSelf"));
    mainObject.insert("self", this->getClientJsonObject(this->clients[uid]));

    QJsonDocument  json(mainObject);
    QString jsonString = json.toJson();
    return this->genPackage(jsonString);
}

QString ClientManager::getClientsPackage(QString id)
{
    return this->getClientsPackage(id, -1);
}

void ClientManager::notifyOthersNewClient(Client *new_client)
{
    QJsonObject  mainObject;

    mainObject.insert("action", QJsonValue::fromVariant("newClient"));
    mainObject.insert("client", this->getClientJsonObject(new_client));

    QJsonDocument json(mainObject);
    QString jsonString = json.toJson();
    QString msg = this->genPackage(jsonString);
    foreach (int target, this->sorted_uids[new_client->getAppID()])
        if (new_client->getUID() != target)
            this->clients[target]->sendTextMessage(msg);
}

void ClientManager::notifyOthersClientDisconnected(Client *c)
{
    QJsonObject  mainObject;
    QJsonArray clientsArray;

    mainObject.insert("action", QJsonValue::fromVariant("clientDisconnected"));
    mainObject.insert("clientUid", c->getUID());

    QJsonDocument json(mainObject);
    QString jsonString = json.toJson();
    QString msg = this->genPackage(jsonString);
    foreach (int target, this->sorted_uids[c->getAppID()])
        if (c->getUID() != target)
            this->clients[target]->sendTextMessage(msg);
}

QString ClientManager::genPackage(QString message)
{
    QJsonObject mainObject;

    mainObject.insert("originName", QJsonValue::fromVariant("server"));
    mainObject.insert("msgData", message);

    QJsonDocument doc(mainObject);
    QString jsonString = doc.toJson();
    return jsonString;
}

QJsonObject ClientManager::getClientJsonObject(Client *c)
{
    QJsonObject t_object;
    t_object.insert("uid", QJsonValue::fromVariant(c->getUID()));
    t_object.insert("appId", QJsonValue::fromVariant(c->getAppID()));
    t_object.insert("peerName", QJsonValue::fromVariant(c->getPeerName()));
    t_object.insert("peerAddress", QJsonValue::fromVariant(c->getPeerAddress()));
    t_object.insert("peerOrigin", QJsonValue::fromVariant(c->getOrigin()));
    if (c->getAppType() == AppType::Server) t_object.insert("appType", QJsonValue::fromVariant("serverApp"));
    else if(c->getAppType() == AppType::WebClient) t_object.insert("appType", QJsonValue::fromVariant("clientApp"));
    else if(c->getAppType() == AppType::UnkownType) t_object.insert("appType", QJsonValue::fromVariant("unkownType"));
    return t_object;
}
