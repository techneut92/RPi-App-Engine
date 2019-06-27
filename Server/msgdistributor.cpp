#include "msgdistributor.h"

MsgDistributor::MsgDistributor(QObject *parent) : QObject(parent)
{

}

MsgDistributor::~MsgDistributor()
{
    qDeleteAll(clients.begin(), clients.end());
}

void MsgDistributor::AppendClient(Client *c)
{
    this->clients << c;
}
