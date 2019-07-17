#include "wc_connector.h"

WebClientConnector::WebClientConnector(QWebSocket* connector, int uid, QString appID, QString location, AppType appType, QObject *parent) :
    Client(connector, uid, appID, location, appType, parent)
{
    this->socket = connector;
    this->connection_type = ConnectionType::WebSocket;
}

WebClientConnector::~WebClientConnector() {
    if (this->socket->isValid())
        this->socket->close();
}

void WebClientConnector::sendTextMessage(QString message)
{

}

void WebClientConnector::sendBinaryMessage(QByteArray message)
{

}

void WebClientConnector::connectSL(ClientManager *cm, MsgDistributor *md)
{

}

QWebSocket* WebClientConnector::getSocket() { return this->socket; }
