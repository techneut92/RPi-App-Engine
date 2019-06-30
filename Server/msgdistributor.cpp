#include "msgdistributor.h"
#include "jsonhandler.h"

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
    // remove c from u_clients
    qDebug() << "MsgDistributor::connectApp: Attempting to remove client from u_clients" << this->u_clients.removeOne(c);

    // add c to cc_clients
    //c_clients << c;
    this->cc_clients[c->getId()].append(c);

    // connect c to the msgdistributor.
    connect(c, &Client::textMessageReceived, this, &MsgDistributor::processTextMessages);

    qDebug() << "MsgDistributor::connectApp" << c->getId() << c->appType() << "Ready to process data";
    c->sendTextMessage("READY");
}

void MsgDistributor::processTextMessages(QString message, QString id, AppType apptype)
{
    qDebug() << "MsgDistributor::processTextMessages: " << message << id << apptype;
    if (jsonHandler::isValidJson(message)){
        QVariantMap jmap = jsonHandler::jsonStringToQMap(message);
        if (!jmap["serverTarget"].isNull()){
            if (jmap["serverTarget"].toString() == "all"){
                // iterate through all clients with the same id and send a message to all except the origin
            }
            else if(jmap["serverTarget"].toString() == "server"){
                // iterate through all clients with the same id and send a message to all server apps except the origin
            }
            else if(jmap["serverTarget"].toString() == "client"){
                // iterate through all clients with the same id and send a message to all client apps except the origin
            }
        }
    }else{
        // iterate through clients with same id and send messages to all different apptypes
    }
}

// TODO FIX
void MsgDistributor::onDisconnect(Client *c)
{
    if (c->awaiting_handshake()){
        // TODO REMOVE client from u_clients
    }else{
        // TODO REMOVE CLIENT FROM CC_CLIENTS
    }
    delete c;
    qDebug() << "not handling the disconnect atm...";
}

