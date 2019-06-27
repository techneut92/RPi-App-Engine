#include "websocketserver.h"
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QSettings>
#include <QDebug>

WebsocketServer::WebsocketServer(MsgDistributor *ms, QObject *parent) :
    QObject(parent),
    websocketServer(nullptr)
{
    this->msg_dist = ms;
    QSettings settings("/etc/rpae/server/server.ini", QSettings::IniFormat);
    quint16 port = settings.value("websocket_server/port", "").toString().toUShort(); //TODO REPLACE
    if (settings.value("websocket_server/ssl_enabled", "").toString() == "true"){
        this->websocketServer = new QWebSocketServer(QStringLiteral("Raspberry pi App Engine message distributor"),
                                                     QWebSocketServer::SecureMode,
                                                     this);
        QSslConfiguration sslConfiguration;
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
    }
    else {
        this->websocketServer = new QWebSocketServer(QStringLiteral("Raspberry pi App Engine message distributor"),
                                                 QWebSocketServer::NonSecureMode,
                                                 this);
    }

    if (websocketServer->listen(QHostAddress::Any, port))
        {
        connect(websocketServer, &QWebSocketServer::newConnection,
                 this, &WebsocketServer::onNewConnection);

        if(this->websocketServer->secureMode() == QWebSocketServer::NonSecureMode){
            qDebug() << "Raspberry Pi App Engine server running on port" << port;
        }
        else if (this->websocketServer->secureMode() == QWebSocketServer::SecureMode)
            qDebug() << "Raspberry Pi App Engine server running on port" << port << "With SSL Enabled.";
            connect(websocketServer, &QWebSocketServer::sslErrors,
                    this, &WebsocketServer::onSslErrors);
        }
}

WebsocketServer::~WebsocketServer()
{
    qDebug() << "Closing and cleaning up the websocketserver...";
    websocketServer->close();
    this->msg_dist = nullptr;
}

void WebsocketServer::onNewConnection()
{
    QWebSocket *pSocket = websocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    this->msg_dist->AppendClient(new Client(pSocket));
}

void WebsocketServer::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "SSL error occured";
    Q_UNUSED(errors)
}
