#include "clientmanager.h"
#include <QJsonArray>

ClientManager::ClientManager(MsgDistributor *md, QObject *parent) : QObject(parent)
{
    this->msgDistributor = md;
    this->msgDistributor->setServer(this);
}

QMap<int, Client *> ClientManager::getClients()
{
    return this->cc_clients;
}

QMap<QString, QList<int> > ClientManager::getSortedClients()
{
    return this->sorted_uids;
}

void ClientManager::appendClient(Client *c)
{
    c->uid = this->getNewUid();
    this->u_clients[c->uid] = c;
    connect(c, &Client::handshake_succesful, this, &ClientManager::connectApp);
    connect(c, &Client::disconnected, this, &ClientManager::onDisconnect);
}

void ClientManager::connectApp(Client *c)
{    
    // add c to cc_clients and sorted uid's
    this->cc_clients[c->uid] = c;
    this->sorted_uids[c->getId()].append(c->uid);

    // remove c from u_clients
    this->u_clients.remove(c->uid);

    // get the clients already connected
    QString clientsString;

    // connect c to the msgdistributor.
    connect(c, &Client::textMessageReceived, this->msgDistributor, &MsgDistributor::processTextMessages);

    qDebug() << "MsgDistributor::connectApp" << c->getId() << c->appType() << "Ready to process data";
    c->sendTextMessage("HANDSHAKE_SUCCESS " + QString::number(c->uid));
    c->sendTextMessage(this->getClientsPackage(c->getId(), c->uid));
    this->notifyOthers(c);
}

void ClientManager::onDisconnect(Client *c)
{
    disconnect(c, &Client::disconnected, this, &ClientManager::onDisconnect);
    if (c->awaiting_handshake()){
        qDebug() << "Client without handshake disconnected" << c->uid;
        // remove from cc_clients
        this->u_clients.remove(c->uid);
    }else{
        qDebug() << "Client Disconnected" << c->getId() << c->uid;
        disconnect(c, &Client::textMessageReceived, this->msgDistributor, &MsgDistributor::processTextMessages);
        this->cc_clients.remove(c->uid); // TODO FIX, segmentation errors??

        QMutableListIterator<int> i(this->sorted_uids[c->getId()]);
        while (i.hasNext()) {
            if (i.next() == c->uid)
                i.remove();
        }
        if (this->sorted_uids[c->getId()].count() == 0)
            this->sorted_uids.remove(c->getId());
        delete c;
    }
}

int ClientManager::getNewUid()
{
    int counter = 0;
    while (this->uidTaken(this->uidCounter)){
        this->uidCounter++;
        if (this->uidCounter > MAX_CLIENTS) this->uidCounter = 0;
        if (counter >= MAX_CLIENTS){
            qDebug() << "FATAL ERROR!: more then" << MAX_CLIENTS << "clients are connected already. exiting";
            exit(255);
        }
    }
    int uid = this->uidCounter;
    this->uidCounter++;
    return uid;
}

bool ClientManager::uidTaken(int uid)
{
    return this->cc_clients.keys().contains(uid) || this->u_clients.contains(uid);
}

QString ClientManager::getClientsPackage(QString id, int ruid)
{
    QJsonObject  mainObject;
    QJsonArray clientsArray;

    mainObject.insert("action", QJsonValue::fromVariant("getClients"));

    foreach(int uid, this->sorted_uids[id])
        if (ruid < 0 || (ruid >= 0 && ruid != uid))
            clientsArray.push_back(this->getClientJsonObject(this->cc_clients[uid]));
    mainObject.insert("clients", clientsArray);

    QJsonDocument  json(mainObject);
    QString jsonString = json.toJson();
    return this->genPackage(jsonString);
}

QString ClientManager::getClientsPackage(QString id)
{
    return this->getClientsPackage(id, -1);
}

void ClientManager::notifyOthers(Client *new_client)
{
    QJsonObject  mainObject;
    QJsonArray clientsArray;

    mainObject.insert("action", QJsonValue::fromVariant("newClient"));
    mainObject.insert("client", this->getClientJsonObject(new_client));

    QJsonDocument json(mainObject);
    QString jsonString = json.toJson();
    QString msg = this->genPackage(jsonString);
    foreach (int target, this->sorted_uids[new_client->getId()])
        if (new_client->uid != target)
            this->cc_clients[target]->sendTextMessage(msg);
}

QString ClientManager::genPackage(QString message)
{
    QJsonObject mainObject;

    mainObject.insert("originName", QJsonValue::fromVariant("server"));
    mainObject.insert("msgData", message);

    QJsonDocument doc(mainObject);
    QString jsonString = doc.toJson();
    //jsonString.remove('\n');
    //jsonString.remove(' ');
    //jsonString.replace("\\\"", "\"");
    return jsonString;
}

QJsonObject ClientManager::getClientJsonObject(Client *c)
{
    QJsonObject t_object;
    t_object.insert("uid", QJsonValue::fromVariant(c->uid));
    t_object.insert("appId", QJsonValue::fromVariant(c->getId()));
    t_object.insert("peerName", QJsonValue::fromVariant(c->getPeerName()));
    t_object.insert("peerAddress", QJsonValue::fromVariant(c->getPeerAddress()));
    t_object.insert("peerOrigin", QJsonValue::fromVariant(c->getOrigin()));
    if (c->appType() == AppType::Server) t_object.insert("appType", QJsonValue::fromVariant("serverApp"));
    else if(c->appType() == AppType::WebClient) t_object.insert("appType", QJsonValue::fromVariant("clientApp"));
    else if(c->appType() == AppType::UnkownType) t_object.insert("appType", QJsonValue::fromVariant("unkownType"));
    return t_object;
}
