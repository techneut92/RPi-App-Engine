#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "msgdistributor.h"
#include "serverapp.h"
#include "clientmanager.h"
#include "wc_connector.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
class WebClientConnector;
class WebsocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketServer(ClientManager *cm, QObject *parent = nullptr);
    ~WebsocketServer() override;

private Q_SLOTS:
    void onNewConnection();
    void onSslErrors(const QList<QSslError> &errors);
    void identify(QString message);
    void falseIdentify(QByteArray message);

private:
    QWebSocketServer *websocketServer;
    QMap<int, QWebSocket*> unidentified_clients;
    ClientManager *cm;
    WebClientConnector *wc_connector;
    int uidCounter = 0;

    int getNewUid();
    bool uidTaken(int uid);
    void appendClient(QWebSocket *client);

signals:

public slots:
};

#endif // WEBSOCKETSERVER_H
