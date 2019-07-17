#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "serverapp.h"
#include "webapp.h"
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
    bool uidTaken(int uid);

private Q_SLOTS:


private:
    MsgDistributor *msgDistributor;
    QMap<int, Client*> clients;
    WebClientConnector *wc_connector = nullptr;
    QMap<QString, QList<int>> sorted_uids;

    QString getClientsPackage(QString id, int excluded_uid);
    QString getClientsPackage(QString id);
    void notifyOthersNewClient(Client* new_client);
    void notifyOthersClientDisconnected(Client *c);
    QString genPackage(QString message);
    QJsonObject getClientJsonObject(Client *c);
    QString getClientsPackage(int uid);

signals:

public slots:
    void onDisconnect(Client *c);

};

#endif // CLIENTMANAGER_H
