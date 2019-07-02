#ifndef MSGDISTRIBUTOR_H
#define MSGDISTRIBUTOR_H

#include <QObject>
#include "client.h"

#define MAX_CLIENTS 100000 // max value of an int should be 2147483647. so this is fine hopefully.

class ClientManager;

class MsgDistributor : public QObject
{
    Q_OBJECT
public:
    explicit MsgDistributor(QObject *parent = nullptr);
    ~MsgDistributor() override;
    void setServer(ClientManager *c);

private:
    void relayMessage(QString message, Client* origin, QVariantMap jmap);
    void relayMessage(QString message, Client* origin);
    ClientManager *cm;

private Q_SLOTS:

signals:
    void msgToSend(QString target, QString message);

public slots:
    void processTextMessages(QString message, Client* origin);
};

#endif // MSGDISTRIBUTOR_H
