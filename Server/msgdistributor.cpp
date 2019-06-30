#include "msgdistributor.h"
#include "jsonhandler.h"
#include <QMap>

MsgDistributor::MsgDistributor(QObject *parent) : QObject(parent)
{

}

MsgDistributor::~MsgDistributor()
{
    qDeleteAll(u_clients.begin(), u_clients.end());
    qDeleteAll(c_clients.begin(), c_clients.end());
}

/* Appends a new connected client and connects the signal to wait for a succesfull handshake */
void MsgDistributor::AppendClient(Client *c)
{
    this->u_clients << c;
    connect(c, &Client::handshake_succesful, this, &MsgDistributor::connectApp);
    connect(c, &Client::disconnected, this, &MsgDistributor::onDisconnect);
}

/* function is a slot connected with the signal from Client::handshake()
 function is called after a succesful handshake was done. */
void MsgDistributor::connectApp(Client *c)
{
    // give an unique id
    c->uid = this->uid_counter;
    this->uid_counter++;
    if (this->uid_counter >= 10000) this->uid_counter = 0;

    // remove c from u_clients
    this->u_clients.removeOne(c);

    // add c to cc_clients
    this->cc_clients[c->getId()].append(c);

    // connect c to the msgdistributor.
    connect(c, &Client::textMessageReceived, this, &MsgDistributor::processTextMessages);

    qDebug() << "MsgDistributor::connectApp" << c->getId() << c->appType() << "Ready to process data";
    c->sendTextMessage("READY");
}

void MsgDistributor::processTextMessages(QString message, Client* origin)
{
    qDebug() << "MsgDistributor::processTextMessages: " << message << origin->getId() << origin->appType() << origin->uid;
    if (jsonHandler::isValidJson(message)){
        QVariantMap jmap = jsonHandler::jsonStringToQMap(message);
        if (!jmap["serverTarget"].isNull()){
            if (jmap["serverTarget"].toString() == "all"){
                // iterate through all clients with the same id and send a message to all except the origin
                foreach( Client* cc, this->cc_clients[origin->getId()]){
                    if (cc->uid != origin->uid) cc->sendTextMessage(jmap["msgData"].toString());
                }
            }
            else if(jmap["serverTarget"].toString() == "server"){
                // iterate through all clients with the same id and send a message to all server apps except the origin
                foreach( Client* cc, this->cc_clients[origin->getId()]){
                    if (cc->uid != origin->uid && cc->appType() == AppType::Server) cc->sendTextMessage(jmap["msgData"].toString());
                }
            }
            else if(jmap["serverTarget"].toString() == "client"){
                qDebug() << "MsgDistributor::processTextMessages: trying to send msg to all clients";
                // iterate through all clients with the same id and send a message to all client apps except the origin
                foreach( Client* cc, this->cc_clients[origin->getId()]){
                    qDebug() << "MsgDistributor::processTextMessages: iterating client:" << cc->getId() << cc->uid << cc->appType();
                    qDebug() << "is apptype webclient" << (cc->appType() == AppType::WebClient);
                    qDebug() << "is apptype server" << (cc->appType() == AppType::Server);
                    qDebug() << "is apptype unkowntype" << (cc->appType() == AppType::UnkownType);
                    if (cc->uid != origin->uid && cc->appType() == AppType::WebClient) {
                        qDebug() << "trying to send msg to client:" << cc->uid;
                        cc->sendTextMessage(jmap["msgData"].toString());
                    }
                }
            }
        }
    }else{
        // iterate through clients with same id and send messages to all different apptypes
        foreach( Client* cc, this->cc_clients[origin->getId()]){
            if (cc != origin && cc->appType() == origin->appType()) cc->sendTextMessage(message);
        }
    }
}

// TODO check FIX
void MsgDistributor::onDisconnect(Client *c)
{
    if (c->awaiting_handshake()){
        // remove client from u_clients
        this->u_clients.removeOne(c);
        qDebug() << "Client without handshake disconnected"; // TODO MOVE TO CLIENT
    }else{
        // remove from cc_clients
        this->cc_clients[c->getId()].removeOne(c);
        qDebug() << "Client Disconnected" << c->getId(); // TODO MOVE TO CLIENT
    }
    delete c;

}

