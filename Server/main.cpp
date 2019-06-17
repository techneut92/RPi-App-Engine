#include <QCoreApplication>
#include "websocketserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    WebsocketServer *server = new WebsocketServer();

    Q_UNUSED(server);
    return a.exec();
}
