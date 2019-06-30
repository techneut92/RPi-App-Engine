#ifndef MSGDISTRIBUTOR_H
#define MSGDISTRIBUTOR_H

#include <QObject>
#include "client.h"

#define MAX_CLIENTS 100000 // max value of an int should be 2147483647. so this is fine hopefully.

class MsgDistributor : public QObject
{
    Q_OBJECT
public:
    explicit MsgDistributor(QObject *parent = nullptr);
    ~MsgDistributor() override;
    void AppendClient(Client *c);               // append newly connected client

private:
    QList<Client *> u_clients;                  // clients awaiting handshake
    QList<Client *> c_clients;                  // clients connected
    QMap<QString, QList<Client*>> cc_clients;   // clients connected in a qmap for easy access
    int uid_counter = 0;
    QList<int> uid_taken;

    int getNewUid();
    void relayMessage(QString message, Client* origin, QVariantMap jmap);
    void relayMessage(QString message, Client* origin);

private Q_SLOTS:
    void connectApp(Client *c);                 // connects app to
    void processTextMessages(QString message, Client* origin);
    void onDisconnect(Client *c);

signals:

public slots:
};

#endif // MSGDISTRIBUTOR_H
