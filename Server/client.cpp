#include "client.h"

Client::Client(QWebSocket *cl, QObject *parent) : QObject(parent)
{
    this->ws_client = cl;
    this->type = ClientType::WebSocket;
    // make connections first
    connect(this->ws_client, &QWebSocket::textMessageReceived, this, &Client::processTextMessage);
    connect(this->ws_client, &QWebSocket::binaryMessageReceived,
            this, &Client::processBinaryMessage);
    connect(this->ws_client, &QWebSocket::disconnected, this, &Client::socketDisconnected);

    // ask for the ID
    cl->sendTextMessage(QString("RequestID"));
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
