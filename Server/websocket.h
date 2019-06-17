#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>

class Websocket : public QObject
{
    Q_OBJECT
public:
    explicit Websocket(QWebSocket *cl, QObject *parent = nullptr);

private:
    QWebSocket *client;

signals:

public slots:
};

#endif // WEBSOCKET_H
