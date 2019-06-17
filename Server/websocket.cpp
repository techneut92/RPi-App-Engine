#include "websocket.h"

Websocket::Websocket(QWebSocket *cl, QObject *parent) : QObject(parent)
{
    this->client = cl;
}
