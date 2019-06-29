#include <QCoreApplication>
#include "websocketserver.h"
#include "msgdistributor.h"

int main(int argc, char *argv[])
{
    qDebug() << "Starting Raspberry Pi App Engine...";
    QCoreApplication a(argc, argv);
    MsgDistributor *msgdist = new MsgDistributor();
    WebsocketServer *server = new WebsocketServer(msgdist);

    Q_UNUSED(server);
    return a.exec();
}
