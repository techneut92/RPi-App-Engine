#ifndef WEBCLIENTCONNECTOR_H
#define WEBCLIENTCONNECTOR_H

#include <QObject>
#include <QWebSocket>
#include "client.h"

class WebClientConnector : public Client
{
    Q_OBJECT
public:
    explicit WebClientConnector(QWebSocket* connector, int uid, QString appID, QString location, AppType appType, QObject *parent = nullptr);
    ~WebClientConnector() override;

    void sendTextMessage(QString message) override;          // Function to send a text message over the websocket
    void sendBinaryMessage(QByteArray message) override;     // Send a binary message over the websocket
    void connectSL(ClientManager *cm, MsgDistributor *md) override;
    QWebSocket *getSocket();

private:
    QWebSocket* socket;

private Q_SLOTS:
    //void onTextMessage(QString message) override;
    //void onBinaryMessage(QByteArray message) override;

signals:
    void disconnected(Client *c);
    void textMessageReceived(QString message, Client *c);
    void binaryMessageReceived(QByteArray message, Client *c);

public slots:
};

#endif // WEBCLIENTCONNECTOR_H
