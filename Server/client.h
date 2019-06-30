#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

/* This class contain the websocket connection and handles those.
 * any message will be send to the msg distributor and will be transfered there
 * to all other clients with the same id.
 */

/* Enum declerations for apptype and connection type. Don't know if this is conventional but i needed them so...
*/
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
    ~Client() override;                             // Override function on the destroyer to make sure all connections are closed.

    QString getId() const;                          // Get function for the app id
    bool awaiting_handshake();                      // Returns the opposite of handshake_succes
    AppType appType();                              // Get function for app_type
    ConnectionType connectionType();                // Get function for con_type
    //int uid;                                        // optional unique id
    void sendTextMessage(QString message);          // Function to send a text message over the websocket
    void sendBinaryMessage(QByteArray message);     // Send a binary message over the websocket

private:
    QString id;                                     // Contains the app ID.
    QWebSocket *ws_client;                          // Contains the websocket connection.
    bool handshake_succes = false;                  // Registers if the handshake was done succesfully.
    ConnectionType con_type;                        // currently always set to websocket, is there in case of expansion to tcp connections.
    AppType app_type = UnkownType;                  // Will contain the apptype if it's either a client or server type.

private Q_SLOTS:
    void processTextMessage(QString message);       // Captures text messages from the websocket
    void processBinaryMessage(QByteArray message);  // Captures binary messages from the websocket
    void socketDisconnected();                      // Registers when the connection closes
    void handshake(QString message);                // Function to perform the handshake, captures the first data from clients
    void falseHandshake(QByteArray message);        // Function to capture binary data before the handshake occured

signals:
    void handshake_succesful(Client *c);                                             // Signal to send when the handshake was succesful sending this client as an object
    void textMessageReceived(QString message, Client* c);          // Signal to send any received text messages
    void binaryMessageReceived(QByteArray message, Client* c);     // Signal to send any received binary messages
    void disconnected(Client *c);                                                    // Signal when client disconnects

public slots:
};

#endif // WEBSOCKET_H
