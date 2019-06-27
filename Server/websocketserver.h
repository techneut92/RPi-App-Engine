#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "msgdistributor.h"
#include "client.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketServer(MsgDistributor *ms, QObject *parent = nullptr);
    ~WebsocketServer() override;

private Q_SLOTS:
    void onNewConnection();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *websocketServer;
    MsgDistributor *msg_dist;

signals:

public slots:
};

#endif // WEBSOCKETSERVER_H
