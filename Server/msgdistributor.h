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
    QList<Client *> clients;

signals:

public slots:
};

#endif // MSGDISTRIBUTOR_H
