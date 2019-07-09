#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "client.h"
#include <QMap>
#include <msgdistributor.h>

#define MAX_CLIENTS 100000 // max value of an int should be 2147483647. so this is fine hopefully.

class ClientManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientManager(MsgDistributor *md, QObject *parent = nullptr);
    QMap<int, Client*> getClients();
    QMap<QString, QList<int>> getSortedClients();
    void appendClient(Client* c);

private Q_SLOTS:
    void connectApp(Client *c);
    void onDisconnect(Client *c);

private:
    MsgDistributor *msgDistributor;
    QMap<int, Client*> u_clients;
    QMap<int, Client*> cc_clients;
    QMap<QString, QList<int>> sorted_uids;
    int uidCounter = 0;

    int getNewUid();
    bool uidTaken(int uid);
    QString getClientsPackage(QString id, int excluded_uid);
    QString getClientsPackage(QString id);
    void notifyOthersNewClient(Client* new_client);
    void notifyOthersClientDisconnected(Client *c);
    QString genPackage(QString message);
    QJsonObject getClientJsonObject(Client *c);
    QString getClientsPackage(int uid);

signals:

public slots:
};

#endif // CLIENTMANAGER_H
