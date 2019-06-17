#include "websocketserver.h"
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QSettings>
#include <QDebug>

WebsocketServer::WebsocketServer(QObject *parent) :
    QObject(parent),
    websocketServer(nullptr)
{
    websocketServer = new QWebSocketServer(QStringLiteral("Raspberry pi App Engine message distributor"),
                                                  QWebSocketServer::SecureMode,
                                                  this);
    QSettings settings(QString("server.ini"), QSettings::IniFormat);
    QSslConfiguration sslConfiguration;
    quint16 port = quint16(settings.value("websocket server/ssl_key", "").toInt());
    QFile certFile(settings.value("websocket server/ssl_cert", "").toString());
    QFile keyFile(settings.value("websocket server/ssl_key", "").toString());
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
            qDebug() << "Raspberry Pi App Engine server running on port " << port;
            connect(websocketServer, &QWebSocketServer::newConnection,
                    this, &WebsocketServer::onNewConnection);
            connect(websocketServer, &QWebSocketServer::sslErrors,
                    this, &WebsocketServer::onSslErrors);
        }
}

WebsocketServer::~WebsocketServer()
{
    websocketServer->close();
    qDeleteAll(clients.begin(), clients.end());
}

void WebsocketServer::onNewConnection()
{

}

void WebsocketServer::processTextMessage(QString message)
{

}

void WebsocketServer::processBinaryMessage(QByteArray message)
{

}

void WebsocketServer::socketDisconnected()
{

}

void WebsocketServer::onSslErrors(const QList<QSslError> &errors)
{

}
