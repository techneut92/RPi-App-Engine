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

private:
    MsgDistributor *msgDistributor;
    QMap<int, Client*> u_clients;
    //QMap<QString, QMap<int, Client*>> cc_clients;   // clients connected in a qmap for easy access
    QMap<int, Client*> cc_clients;
    QMap<QString, QList<int>> sorted_uids;
    int uidCounter = 0;
    //QList<int> uidTaken;

    void connectApp(Client *c);
    void onDisconnect(Client *c);
    int getNewUid();
    bool uidTaken(int uid);
    QString getClientsPackage(QString id, int excluded_uid);
    QString getClientsPackage(QString id);
    void notifyOthers(Client* new_client);
    QString genPackage(QString message);
    QJsonObject getClientJsonObject(Client *c);

signals:

public slots:
};

#endif // CLIENTMANAGER_H
