#include "client.h"
#include <QWebSocket>

Client::Client(QWebSocket *cl, QObject *parent) : QObject(parent)
{
    this->ws_client = cl;                       // save the connection in this variable
    this->con_type = ConnectionType::WebSocket; // set the connection type

    // make connections first. This is set before the handshake can occur.
    connect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::handshake);
    connect(this->ws_client, &QWebSocket::binaryMessageReceived,
            this, &Client::falseHandshake);
    connect(this->ws_client, &QWebSocket::disconnected, this, &Client::socketDisconnected);

    // ask the client to do an handshake
    cl->sendTextMessage(QString("HANDSHAKE"));
}

// destructor makes sure the connection is closed and deleted.
Client::~Client()
{
    this->ws_client->close();
    this->ws_client = nullptr;
}

// getter for id
QString Client::getId() const
{
    return id;
}

// returns if the handshake has occured as a bool
bool Client::awaiting_handshake()
{
    if (this->handshake_succes) return false;
    else return true;
}

// returns apptype
AppType Client::appType()
{
    return this->app_type;
}

// return connection type
ConnectionType Client::connectionType()
{
    return this->con_type;
}

// Messages are handled in the msg distributor, as such it will be directly emitted.
void Client::processTextMessage(QString message)
{
    qDebug() << message;
    emit textMessageReceived(message, this->id, this->app_type);
}

void Client::processBinaryMessage(QByteArray message)
{
    qDebug() << message;
    emit binaryMessageReceived(message, this->id, this->app_type);
}

// TODO HANDLE DISCONNECTS
void Client::socketDisconnected()
{
    qDebug() << "disconnected";
}

// handshake function catches all first messages from the app.
// the first data should contain the app id and the type of app.
void Client::handshake(QString message)
{

    // process handshake data
    qDebug() << "Handshake started:" << message;
    // decode json or whatever format I decide to use
    // set id and apptype
    // set handshake_succes to true

    if (this->handshake_succes){
        // disconnect previous signals as we don't need to use the handshake anymore.
        disconnect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::handshake);
        disconnect(this->ws_client, &QWebSocket::binaryMessageReceived,
                this, &Client::falseHandshake);
        // reconnect the signals to the functions to handle msg distribution
        connect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::processTextMessage);
        connect(this->ws_client, &QWebSocket::binaryMessageReceived,
                this, &Client::processBinaryMessage);
        this->ws_client->sendTextMessage("HANDSHAKE_SUCCES");
        emit handshake_succesful(this);
    }else{
        this->ws_client->sendTextMessage("HANDSHAKE_FAILURE: incorrect data: " + message); // TODO add error
    }
}

// handshake cannot be processed as binary data.
void Client::falseHandshake(QByteArray message)
{
    qDebug() << "Data received as bytearray while awaiting handshake...";
    qDebug() << message;
    this->ws_client->sendTextMessage("HANDSHAKE_FAILURE: Data received as byte array.");
}
