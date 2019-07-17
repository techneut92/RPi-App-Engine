#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QWebSocket>

/* Enum declerations for apptype and connection type. Don't know if this is conventional but i needed them so...
*/
enum ConnectionType{
    WebSocket, TcpSocket, TypeNone
};

enum AppType{
    WCConnector, Server, WebClient, UnkownType
};

// pre declarations
class ClientManager;
class MsgDistributor;

class Client : public QObject
{
public:
    explicit Client(QWebSocket *sock, int uid, QString appID, QString location, AppType appType, QObject *parent = nullptr);
    virtual ~Client();

    int getUID();
    QString getAppID();
    QString getLocation();
    AppType getAppType();
    ConnectionType getConnectionType();
    virtual QString getPeerAddress();
    virtual QString getOrigin();
    virtual QString getPeerName();
    virtual void connectSL(ClientManager *cm, MsgDistributor *md) = 0;
    //virtual void disconnectSL() = 0;

    virtual void sendTextMessage(QString message) = 0;
    virtual void sendBinaryMessage(QByteArray message) = 0;

protected:
    int uid;
    QString app_id;
    QString location;
    AppType app_type = AppType::UnkownType;
    ConnectionType connection_type = ConnectionType::TypeNone;
    QString peerAddress;
    QString origin;
    QString peerName;
    //virtual void onTextMessage(QString message) = 0;
    //virtual void onBinaryMessage(QByteArray message) = 0;

};

#endif // CLIENT_H
