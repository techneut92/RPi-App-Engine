#ifndef WEBAPP_H
#define WEBAPP_H

#include <QObject>
#include "client.h"
#include "wc_connector.h"

class WebApp : public Client
{
    Q_OBJECT
public:
    explicit WebApp(WebClientConnector *wc_connector, int uid, QString appID, QString location, AppType appType, QObject *parent = nullptr);
    ~WebApp() override;

    void sendTextMessage(QString message) override;
    void sendBinaryMessage(QByteArray message) override;
    void connectSL(ClientManager *cm, MsgDistributor *md) override;

private:
    WebClientConnector *wc_connector;

private Q_SLOTS:
    //void onTextMessage(QString message) override;
    //void onBinaryMessage(QByteArray message) override;

signals:
    void disconnected(Client *c);
    void textMessageReceived(QString message, Client *c);
    void binaryMessageReceived(QByteArray message, Client *c);

public slots:
};

#endif // WEBAPP_H
