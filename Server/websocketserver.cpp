#include "websocketserver.h"
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QSettings>
#include <QDebug>
#include "websocket.h"

WebsocketServer::WebsocketServer(QObject *parent) :
    QObject(parent),
    websocketServer(nullptr)
{
    websocketServer = new QWebSocketServer(QStringLiteral("Raspberry pi App Engine message distributor"),
                                                  QWebSocketServer::SecureMode,
                                                  this);
    QSettings settings("/etc/rpae/server.ini", QSettings::IniFormat);  // TODO CHANGE TO /etc/rpae
    QSslConfiguration sslConfiguration;
    quint16 port = settings.value("websocket_server/port", "").toString().toUShort();
    QFile certFile(settings.value("websocket_server/ssl_cert", "").toString());
    QFile keyFile(settings.value("websocket_server/ssl_key", "").toString());
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
    websocketServer->setSslConfiguration(sslConfiguration);

    if (websocketServer->listen(QHostAddress::Any, port))
        {
            qDebug() << "Raspberry Pi App Engine server running on port" << port;
            connect(websocketServer, &QWebSocketServer::newConnection,
                    this, &WebsocketServer::onNewConnection);
            connect(websocketServer, &QWebSocketServer::sslErrors,
                    this, &WebsocketServer::onSslErrors);
        }
}

WebsocketServer::~WebsocketServer()
{
    websocketServer->close();
    qDeleteAll(u_clients.begin(), u_clients.end());
}

void WebsocketServer::onNewConnection()
{
    QWebSocket *pSocket = websocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebsocketServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &WebsocketServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebsocketServer::socketDisconnected);

    u_clients << pSocket;

}

void WebsocketServer::processTextMessage(QString message)
{
    qDebug() << message;
}

void WebsocketServer::processBinaryMessage(QByteArray message)
{
    qDebug() << message;
}

void WebsocketServer::socketDisconnected()
{
    qDebug() << "disconnected";
}

void WebsocketServer::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "SSL error occured";
    Q_UNUSED(errors)
}
