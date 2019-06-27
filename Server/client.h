#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>

enum ClientType{
    WebSocket, TcpSocket
};

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QWebSocket *cl, QObject *parent = nullptr);
    //explicit Client(QTcpSocket *cl, QObject *parent);
    ~Client() override;
    ClientType type;

private:
    QWebSocket *ws_client;
    QTcpSocket *tcp_client;

    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

signals:

public slots:
};

#endif // WEBSOCKET_H
