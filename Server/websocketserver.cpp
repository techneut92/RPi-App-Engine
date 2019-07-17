#include "websocketserver.h"
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QSettings>
#include <QDebug>
#include "clientmanager.h"
#include "jsonhandler.h"
#include "wc_connector.h"
#include "serverapp.h"
#include "webapp.h"

WebsocketServer::WebsocketServer(ClientManager *cm, QObject *parent) :
    QObject(parent),
    websocketServer(nullptr)
{
    this->cm = cm;
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
            qDebug() << "Websocket server running on port" << port;
        }
        else if (this->websocketServer->secureMode() == QWebSocketServer::SecureMode)
            qDebug() << "Websocket server running on port" << port << "With SSL Enabled.";
            connect(websocketServer, &QWebSocketServer::sslErrors,
                    this, &WebsocketServer::onSslErrors);
        }
}

WebsocketServer::~WebsocketServer()
{
    qDebug() << "Closing and cleaning up the websocketserver...";
    websocketServer->close();
    //this->msg_dist = nullptr;
    delete this->cm;
}

void WebsocketServer::onNewConnection()
{
    QWebSocket *pSocket = websocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerAddress().toString() << pSocket->origin();

    this->appendClient(pSocket);

    //this->cm->appendClient(new ServerApp(pSocket));
}

void WebsocketServer::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "SSL error occured";
    Q_UNUSED(errors)
}

void WebsocketServer::identify(QString message)
{
    // convert json to qvariantmap
    if (jsonHandler::isValidJson(message)) {
        QVariantMap jsonMap = jsonHandler::jsonStringToQMap(message);
        if (!jsonMap.isEmpty() && !jsonMap["appID"].isNull() && !jsonMap["uid"].isNull()
                && !jsonMap["appType"].isNull()){// process handshake data
            int uid = jsonMap["uid"].toInt();
            QString id = jsonMap["id"].toString();
            QString location = jsonMap["location"].toString();
            AppType app_type;
            QWebSocket *socket = this->unidentified_clients.value(uid);
            qDebug() << "Handshake started for uid:" << uid;
            Client *app;
            if (jsonMap["appType"].toString() == "clientApp") {
                app_type = AppType::WebClient;
                app = new WebApp(this->wc_connector, uid, id, location, app_type);
                this->cm->appendClient(app);
            }
            else if (jsonMap["appType"].toString() == "serverApp") {
                app_type = AppType::Server;
                app = new ServerApp(socket, uid, id, location, app_type);
                this->cm->appendClient(app);
            }
            else if (jsonMap["appType"].toString() == "webClientConnector") {
                app_type = AppType::WCConnector;
                WebClientConnector *app = new WebClientConnector(socket, uid, id, location, app_type);
                this->wc_connector = app;
                this->cm->appendClient(app);
            }
        }else{
            //this->ws_client->sendTextMessage("HANDSHAKE_FAILURE: incorrect data: " + message);
        }
    }else{
        //this->ws_client->sendTextMessage("HANDSHAKE_FAILURE: incorrect data: " + message); // TODO add error
    }
}

void WebsocketServer::falseIdentify(QByteArray message)
{
    this->identify(QString::fromStdString(message.toStdString()));
}

int WebsocketServer::getNewUid()
{
    int counter = 0;
    while (this->uidTaken(this->uidCounter)){
        this->uidCounter++;
        if (this->uidCounter > MAX_CLIENTS) this->uidCounter = 0;
        if (counter >= MAX_CLIENTS){
            qDebug() << "FATAL ERROR!: more then" << MAX_CLIENTS << "clients are connected already. exiting";
        }
    }
    int uid = this->uidCounter;
    this->uidCounter++;
    return uid;
}

bool WebsocketServer::uidTaken(int uid)
{
    return this->cm->uidTaken(uid) || this->unidentified_clients.contains(uid);
}

void WebsocketServer::appendClient(QWebSocket *socket)
{
    connect(socket, &QWebSocket::textMessageReceived, this, &WebsocketServer::identify);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &WebsocketServer::falseIdentify);
    int uid = this->getNewUid();
    this->unidentified_clients[uid] = socket;
    socket->sendTextMessage("HANDSHAKE " + QString::number(uid));
}
