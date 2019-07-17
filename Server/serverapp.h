#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include "client.h"

/* This class contain the websocket connection and handles those.
 * any message will be send to the msg distributor and will be transfered there
 * to all other clients with the same id.
 */

class ServerApp : public Client
{
    Q_OBJECT
public:
    explicit ServerApp(QWebSocket *cl, int uid, QString appID, QString location, AppType appType, QObject *parent = nullptr);
    ~ServerApp() override;                             // Override function on the destroyer to make sure all connections are closed.

    void sendTextMessage(QString message) override;          // Function to send a text message over the websocket
    void sendBinaryMessage(QByteArray message) override;     // Send a binary message over the websocket
    void connectSL(ClientManager *cm, MsgDistributor *md) override;

private:
    QWebSocket *socket;                          // Contains the websocket connection.

private Q_SLOTS:
    void processTextMessage(QString message);       // Captures text messages from the websocket
    void processBinaryMessage(QByteArray message);  // Captures binary messages from the websocket
    void socketDisconnected();                      // Registers when the connection closes

signals:
    void disconnected(Client *c);
    void textMessageReceived(QString message, Client *c);
    void binaryMessageReceived(QByteArray message, Client *c);

public slots:
};

#endif // WEBSOCKET_H
