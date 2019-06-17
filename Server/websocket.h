#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>

class Websocket : public QWebSocket
{
    Q_OBJECT
public:
    explicit Websocket(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WEBSOCKET_H
