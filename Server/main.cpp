#include <QCoreApplication>
#include "websocketserver.h"
#include "msgdistributor.h"
#include "clientmanager.h"

int main(int argc, char *argv[])
{
    qDebug() << "Starting Raspberry Pi App Engine...";
    QCoreApplication a(argc, argv);
    MsgDistributor *msgdist = new MsgDistributor();
    ClientManager *c_manager = new ClientManager(msgdist);
    WebsocketServer *server = new WebsocketServer(c_manager);

    Q_UNUSED(server);
    Q_UNUSED(c_manager);
    return a.exec();
}
