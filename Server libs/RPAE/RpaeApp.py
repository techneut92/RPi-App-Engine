import websocket
import json


class RpaeApp:
    def __init__(self):
        self.id = None
        self.uid = None
        self.host = None
        self.ws = None
        self.connected = False
        self.queue = []

    def handshake(self, message):
        if message == "HANDSHAKE":
            self.ws.send(json.dumps({
                'id': self.id,
                'appType': 'server'
            }))
        elif message.startswith("HANDSHAKE_SUCCES"):
            message.split(' ')
            self.uid = int(message[1])
            self.ws.on_message = self.onMessage
            self.ws.on_open = None
            self.onOpen()
            [self.onMessage(m) for m in self.queue]
        else:
            self.queue.append(message)

    def preHandshakeOpen(self):
        self.connected = True

    def sendMessage(self, message):
        self.ws.send(self.createPackage(message))

    def createPackage(self, message):
        return json.dumps({
            'origin': self.uid,
            'originName': self.id,
            'msgData': message
        })

    def onMessage(self, message):
        pass

    def onOpen(self):
        pass

    def onError(self, error):
        print(error)

    @property
    def host(self):
        return self.host

    @host.setter
    def host(self, value):
        if self.connected:
            self.ws.close()
        self.ws = websocket.WebSocketApp(value, on_message=self.handshake,
                                         on_error=self.onError, on_open=self.preHandshakeOpen)
        self.host = value
