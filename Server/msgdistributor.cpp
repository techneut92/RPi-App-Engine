#include "msgdistributor.h"

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
}

/* function is a slot connected with the signal from Client::handshake()
 function is called after a succesful handshake was done. */
void MsgDistributor::connectApp(Client *c)
{
    // remove c from u_clients

    // add c to c_clients
    //c_clients << c;

    // connect c to the msgdistributor.
    //connect(c, &Client::textMessageReceived, this, &MsgDistributor::processTextMessages);

    qDebug() << c->getId() << c->appType() << "Ready to process data";
    Q_UNUSED(c);
}

void MsgDistributor::processTextMessages(QString message, QString id, AppType apptype)
{
    qDebug() << message << id << apptype;
}

