#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "msgdistributor.h"
#include "client.h"
#include "clientmanager.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketServer(ClientManager *cm, QObject *parent = nullptr);
    ~WebsocketServer() override;

private Q_SLOTS:
    void onNewConnection();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *websocketServer;
    //MsgDistributor *msg_dist;
    ClientManager *cm;

signals:

public slots:
};

#endif // WEBSOCKETSERVER_H
