#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketServer(QObject *parent = nullptr);
    ~WebsocketServer() override;

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *websocketServer;
    QList<QWebSocket *> clients;

signals:

public slots:
};

#endif // WEBSOCKETSERVER_H
