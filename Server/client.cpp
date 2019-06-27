#include "client.h"
#include <QWebSocket>

Client::Client(QWebSocket *cl, QObject *parent) : QObject(parent)
{
    this->ws_client = cl;
    this->con_type = ConnectionType::WebSocket;
    // make connections first
    connect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::handshake);
    connect(this->ws_client, &QWebSocket::binaryMessageReceived,
            this, &Client::falseHandshake);
    connect(this->ws_client, &QWebSocket::disconnected, this, &Client::socketDisconnected);

    // ask for the ID
    cl->sendTextMessage(QString("HANDSHAKE"));
}

//Client::Client(QTcpSocket *cl, QObject *parent) : QObject(parent)
//{
    //this->tcp_client = cl;
    //this->Type = ClientType::TcpSocket;
//}

Client::~Client()
{
    this->ws_client = nullptr;
}

QString Client::getId() const
{
    return id;
}

void Client::processTextMessage(QString message)
{
    qDebug() << message;
}

void Client::processBinaryMessage(QByteArray message)
{
    qDebug() << message;
}

void Client::socketDisconnected()
{
    qDebug() << "disconnected";
}

void Client::handshake(QString message)
{
    bool handshake_succes = false;
    // process handshake data
    qDebug() << message;

    if (handshake_succes){
        // disconnect previous signals
        disconnect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::handshake);
        disconnect(this->ws_client, &QWebSocket::binaryMessageReceived,
                this, &Client::falseHandshake);
        // reconnect the signals
        connect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::processTextMessage);
        connect(this->ws_client, &QWebSocket::binaryMessageReceived,
                this, &Client::processBinaryMessage);
        this->ws_client->sendTextMessage("HANDSHAKE_SUCCES");
    }else{
        this->ws_client->sendTextMessage("HANDSHAKE_FAILURE");
    }
}

void Client::falseHandshake(QByteArray message)
{
    qDebug() << "Data received as bytearray while awaiting handshake...";
    qDebug() << message;
    this->ws_client->sendTextMessage("HANDSHAKE_FAILURE");
}
