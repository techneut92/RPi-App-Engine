#include "serverapp.h"
#include <QWebSocket>
#include "jsonhandler.h"
#include "client.h"
#include "clientmanager.h"

ServerApp::ServerApp(QWebSocket *cl, int uid, QString appID, QString location, AppType appType, QObject *parent):
    Client (cl, uid, appID, location, appType, parent)
{
    this->socket = cl;
    connect(this->socket, &QWebSocket::disconnected, this, &ServerApp::socketDisconnected);
}

ServerApp::~ServerApp()
{
    if (this->socket->isValid())
        this->socket->close();
}

void ServerApp::sendBinaryMessage(QByteArray message)
{
    this->socket->sendBinaryMessage(message);
}

void ServerApp::connectSL(ClientManager *cm, MsgDistributor *md)
{
    connect(this, &ServerApp::disconnected, cm, &ClientManager::onDisconnect);
    connect(this, &ServerApp::textMessageReceived, md, &MsgDistributor::processTextMessages);
    connect(this, &ServerApp::binaryMessageReceived, md, &MsgDistributor::processBinaryMessages);
}

void ServerApp::sendTextMessage(QString message)
{
    this->socket->sendTextMessage(message);
}

void ServerApp::processTextMessage(QString message) { emit textMessageReceived(message, this); }

void ServerApp::processBinaryMessage(QByteArray message) { emit binaryMessageReceived(message, this); }

void ServerApp::socketDisconnected() { emit disconnected(this); }
