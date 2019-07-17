#include "client.h"

Client::Client(QWebSocket *sock, int uid, QString appID, QString location, AppType appType, QObject *parent):
    QObject (parent)
{
    this->uid = uid;
    this->app_id = appID;
    this->location = location;
    this->app_type = appType;
    this->peerAddress = sock->peerAddress().toString();
    this->peerName = sock->peerName();
    this->origin = sock->origin();
    this->connection_type = ConnectionType::WebSocket;
}

Client::~Client(){}

int Client::getUID() { return this->uid; }
QString Client::getAppID() { return this->app_id; }
QString Client::getLocation() { return this->location; }
AppType Client::getAppType() { return this->app_type; }
ConnectionType Client::getConnectionType() { return this->connection_type; }
QString Client::getPeerAddress() { return this->peerAddress; }
QString Client::getOrigin() { return this->origin; }
QString Client::getPeerName() { return this->peerName; }

