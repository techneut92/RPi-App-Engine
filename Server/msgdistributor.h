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
    void AppendClient(Client *c);

private:
    QList<Client *> u_clients; // clients awaiting handshake
    QList<Client *> c_clients; // clients connected

private Q_SLOTS:
    void connectApp(Client *c);
    void processTextMessages(QString message, QString id, AppType apptype);

signals:

public slots:
};

#endif // MSGDISTRIBUTOR_H
