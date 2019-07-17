#include "webapp.h"
#include "wc_connector.h"

WebApp::WebApp(WebClientConnector *wc_connector, int uid, QString appID, QString location, AppType appType, QObject *parent) :
    Client (wc_connector->getSocket(), uid, appID, location, appType, parent)
{
    this->wc_connector = wc_connector;
    this->connection_type = ConnectionType::WebSocket;
}

WebApp::~WebApp() {}

void WebApp::sendTextMessage(QString message)
{

}

void WebApp::sendBinaryMessage(QByteArray message)
{

}

void WebApp::connectSL(ClientManager *cm, MsgDistributor *md)
{

}
