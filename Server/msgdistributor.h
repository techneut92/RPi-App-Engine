#ifndef MSGDISTRIBUTOR_H
#define MSGDISTRIBUTOR_H

#include <QObject>
#include "client.h"

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

private Q_SLOTS:
    void connectApp(Client *c);                 // connects app to
    void processTextMessages(QString message, Client* origin);
    void onDisconnect(Client *c);

signals:

public slots:
};

#endif // MSGDISTRIBUTOR_H
