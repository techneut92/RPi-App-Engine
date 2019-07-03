import websocket
import json


class RpaeApp:
    def __init__(self, aid, host="ws://localhost:9738"):
        self._id = aid                # the app id, when inheriting should be set
        self._uid = None            # the unique connection id. is set when the handshake is successful
        self._host = None           # the websocket's host. when set it tries to connect and set self._ws
        self._ws = None             # the websocket
        self._connected = False     # bool to show if host is connected
        self._isReady = False       # will set to true once the handshake is done
        self._queue = []            # queue to catch all messages during an handshake.
        self._appTypes = ['all', 'clientApp', 'serverApp', 'all', 'uid']
        self.host = host
        self._peers = []

    # Handshake has to occur before data can be transmitted
    def _handshake(self, message):
        print(message)
        if message == "HANDSHAKE":
            self._ws.send(json.dumps({
                'id': self.id,
                'appType': 'serverApp'
            }))
        elif message.startswith("HANDSHAKE_SUCCES"):
            message = message.split(' ')
            self._uid = int(message[1])
            self._isReady = True
            [self._onMessage(m) for m in self._queue]
        elif message.startswith("HANDSHAKE_FAILURE"):
            self._onError(message)
        else:
            print("msg queued")
            self._queue.append(message)

    # create a package to send over the server
    @staticmethod
    def _createPackage(message, target='clientApp', uidTarget=None):
        package = json.dumps({
            'serverTarget': target,
            'msgData': message
        })
        if target is not 'clientApp':
            package["serverTarget"] = target
        elif uidTarget is not None and target is 'uid':
            package["uid"] = uidTarget
        elif target is 'uid' and uidTarget is None:
            print("ERROR: trying to send message to specific uid but no uid is given")
            return None
        return package

    # sends a message to other peers
    def sendMessage(self, message, appType='clientApp', uidTarget=None):
        msgData = self._createPackage(message, target=appType, uidTarget=uidTarget)
        if msgData is not None:
            self._ws.send()

    # Receives messages after handshake is complete, should be overwritten
    def onMessage(self, message, origin):
        pass

    # onOpen function triggers when handshake is complete, could be overwritten
    def onOpen(self):
        pass

    # public onError function, could be overwritten
    def onError(self, error):
        pass

    # public function for when a new client connects
    def onNewClient(self, client):
        pass

    # start the server thread
    def start(self):
        self._ws.run_forever()

    def _handleServerMessages(self, message):
        try:
            message = json.loads(message)
        except ValueError:
            print("received message not json")
            return
        if message["action"] is "newClient":
            self._peers.append(message['client'])
            self.onNewClient(message['client'])
        elif message['action'] is "getClients":
            self._peers = message["clients"]

    # private onMessage function
    def _onMessage(self, message):
        if not self._isReady:
            self._handshake(message)
        else:
            try:
                message = json.loads(message)
            except ValueError:
                print("received message not json")
                # self.onMessage(message, None)
                return
            if message["originName"] == "server":
                self._handleServerMessages(message['msgData'])
            else:
                print(type(message['msgData']))
                self.onMessage(message["msgData"], message["origin"])

    # sets connected to true once the websocket opens
    def _onOpen(self):
        self._connected = True
        self.onOpen()

    # private onError function
    def _onError(self, error):
        print(error)
        self.onError(error)

    # private function of on new client, adds it to the program and executes the public one
    def _onNewClient(self, data):
        client = data
        self.onNewClient(client)

    # getter for host
    @property
    def host(self):
        return self.host

    # setter for host, automatically tries to connect to the server
    @host.setter
    def host(self, value):
        if self._connected:
            self._ws.close()
            del self._ws
        self._ws = websocket.WebSocketApp(value, on_message=self._onMessage,
                                          on_error=self._onError, on_open=self._onOpen)
        self._host = value

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

    @property
    def appTypes(self):
        return self._appTypes

    @property
    def id(self):
        return self._id

    @property
    def peers(self):
        return self._peers
