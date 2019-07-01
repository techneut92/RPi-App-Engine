import websocket
import json


class RpaeApp:
    def __init__(self):
        self.id = None              # the app id, when inheriting should be set
        self._uid = None            # the unique connection id. is set when the handshake is successful
        self._host = None           # the websocket's host. when set it tries to connect and set self._ws
        self._ws = None             # the websocket
        self._connected = False     # bool to show if host is connected
        self._isReady = False       # will set to true once the handshake is done
        self._queue = []            # queue to catch all messages during an handshake.

    # Handshake has to occur before data can be transmitted
    def _handshake(self, message):
        if message == "HANDSHAKE":
            self._ws.send(json.dumps({
                'id': self.id,
                'appType': 'server'
            }))
        elif message.startswith("HANDSHAKE_SUCCESS"):
            message.split(' ')
            self._uid = int(message[1])
            self._ws.on_message = self._onMessage
            self._ws.on_open = None
            self.onOpen()
            [self._onMessage(m) for m in self._queue]
            self._isReady = True
        elif message.startswith("HANDSHAKE_FAILURE"):
            self._onError(message)
        else:
            self._queue.append(message)

    # create a package to send over the server
    def _createPackage(self, message, appType, uidTarget):
        package = json.dumps({
            # 'origin': self._uid,
            # 'originName': self.id,
            'msgData': message
        })
        if appType is not None:
            package["serverTarget"] = appType
        if uidTarget is not None:
            package["serverTarget"] = "uid"
            package["serverTarget"] = uidTarget
        return package

    # sends a message to other peers
    def sendMessage(self, message, appType=None, uidTarget=None):
        self._ws.send(self._createPackage(message, appType=appType, uidTarget=uidTarget))

    # Receives messages after handshake is complete, should be overwritten
    def onMessage(self, message, origin):
        pass

    # onOpen function triggers when handshake is complete, could be overwritten
    def onOpen(self):
        pass

    # public onError function, could be overwritten
    def onError(self, error):
        pass

    # private onMessage function
    def _onMessage(self, message):
        try:
            message = json.loads(message)
        except ValueError:
            self.onMessage(message, None)
            return
        if message["originName"] == "server":
            pass  # TODO CREATE
        else:
            self.onMessage(message["msgData"], message["origin"])

    # sets connected to true once the websocket opens
    def _onOpen(self):
        self._connected = True

    # private onError function
    def _onError(self, error):
        print(error)
        self.onError(error)

    # getter for host
    @property
    def host(self):
        return self.host

    # setter for host, automatically tries to connect to the server
    @host.setter
    def host(self, value):
        if self._connected:
            self._ws.close()
        self._ws = websocket.WebSocketApp(value, on_message=self._handshake,
                                          on_error=self._onError, on_open=self._onOpen)
        self.host = value

    # getter for connected, is true if connected to server before handshake
    @property
    def connected(self):
        return self._connected

    # getter for uid, should never be changed
    @property
    def uid(self):
        return self._uid

    @property
    def isReady(self):
        return self._isReady
