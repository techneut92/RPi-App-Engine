#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>

enum ConnectionType{
    WebSocket, TcpSocket
};

enum AppType{
    Server, WebClient, UnkownType
};

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QWebSocket *cl, QObject *parent = nullptr);
    //explicit Client(QTcpSocket *cl, QObject *parent);
    ~Client() override;
    ConnectionType con_type;
    AppType app_type = UnkownType;

    QString getId() const;

private:
    QString id;
    QWebSocket *ws_client;
    //QTcpSocket *tcp_client;

    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void handshake(QString message);
    void falseHandshake(QByteArray message);

signals:

public slots:
};

#endif // WEBSOCKET_H
