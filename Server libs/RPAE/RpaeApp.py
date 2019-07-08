import websocket
import json
from ._peer import Peer
import configparser
import os

# defines
_DEFAULT_HOST = 'ws://localhost:9738'
_APPTYPE = 'serverApp'
_DEFAULT_TARGET = 'clientApp'
_DEFAULT_CONFIG = 'app.ini'


class RpaeApp(Peer):
    __host = None  # holds the host address
    __ws = None  # the websocket
    __connected = False  # bool to show if host is connected
    __isReady = False  # will set to true once the handshake is done
    __queue = []  # queue to catch all messages during an handshake.
    __appTypes = ['all', 'clientApp', 'serverApp', 'unknownType', 'uid']
    __peers = {}

    def __init__(self, configFile=_DEFAULT_CONFIG, configDir='./'):
        cDir = os.path.dirname(os.path.realpath(__file__))
        print('TRYING TO GET THE APP INI DIR AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA', cDir)
        configFile = cDir + '/' + configFile
        super().__init__(configFile=configFile)
        config = configparser.ConfigParser()
        config.read(configFile)
        if 'server_host' in config['app']:
            self.___host = config['app']['server_host']  # the websocket's host.
        else:
            self.___host = _DEFAULT_HOST
        del config

    # Handshake has to occur before data can be transmitted
    def __handshake(self, message):
        if message == "HANDSHAKE":
            msg = {
                'id': self.appID,
                'appType': _APPTYPE,
                'location': ''
            }
            if self.location is not None:
                msg['location'] = self.location
            self.__ws.send(json.dumps(msg))
        elif message.startswith("HANDSHAKE_SUCCES"):
            self.__isReady = True
            message = message.split(' ')
            self.__onMessage(message[1])
            self.__onMessage(message[2])
            self.onOpen()
            [self.__onMessage(m) for m in self.__queue]
        elif message.startswith("HANDSHAKE_FAILURE"):
            self.__onError(message)
        else:
            self.__queue.append(message)

    # create a package to send over the server
    @staticmethod
    def __createPackage(message, target=_DEFAULT_TARGET, uidTarget=None):
        package = {
            'serverTarget': target,
            'msgData': message
        }
        if target != _DEFAULT_TARGET:
            package["serverTarget"] = target
            if uidTarget is not None and target == 'uid':
                package['uid'] = uidTarget
            elif target == 'uid' and uidTarget is None:
                print("ERROR: trying to send message to specific uid but no uid is given")
                return None
        return json.dumps(package)

    # sends a message to other peers
    def sendMessage(self, message, appType='clientApp', uidTarget=None):
        msgData = self.__createPackage(message, target=appType, uidTarget=uidTarget)
        if msgData is not None:
            self.__ws.send(msgData)

    def init(self):
        pass

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
    def onNewPeer(self, peer):
        pass

    def onPeerDisconnected(self, peer):
        pass

    # start the server thread
    def start(self):
        self.init()
        self.__ws.run_forever()

    def __handleServerMessages(self, message):
        try:
            message = json.loads(message)
        except ValueError:
            print("received message from server not json")
            return
        if message["action"] == 'newClient':
            self.__onNewPeer(message['client'])
        elif message['action'] == 'initSelf':
            self._updateSelf(message['self'])
        elif message['action'] == 'getClients':
            self.__peers = self.__serverGetClients(message['clients'])
        elif message['action'] == 'clientDisconnected':
            self.__onPeerDisconnected(message['clientUid'])

    @staticmethod
    def __serverGetClients(clientDict):
        new_dict = {}
        for client in clientDict:
            p = Peer(clientDict=client)
            new_dict[p.uid] = p
        return new_dict

    # private onMessage function
    def __onMessage(self, message):
        if not self.__isReady:
            self.__handshake(message)
        else:
            try:
                message = json.loads(message)
            except ValueError:
                print("received message not json")
                return
            if 'originName' in message and message['originName'] == "server":
                self.__handleServerMessages(message['msgData'])
            else:
                self.onMessage(message["msgData"], message["origin"])

    # sets connected to true once the websocket opens
    def __onOpen(self):
        self.__connected = True

    # private onError function
    def __onError(self, error):
        print(error)

        self.onError(error)

    # private function of on new client, adds it to the program and executes the public one
    def __onNewPeer(self, peer):
        p = Peer(clientDict=peer)
        self.__peers[p.uid] = p
        self.onNewPeer(p)

    def __onPeerDisconnected(self, clientUid):
        self.onPeerDisconnected(self.__peers.pop(int(clientUid), None))

    # getter for host
    @property
    def host(self):
        return self.__host

    @property
    def ___host(self):
        return self.__host

    # setter for host, automatically tries to connect to the server
    @___host.setter
    def ___host(self, value):
        if self.__connected:
            self.__ws.close()
            del self.__ws
        self.__ws = websocket.WebSocketApp(value, on_message=self.__onMessage,
                                           on_error=self.__onError, on_open=self.__onOpen)
        self.__host = value

    # getter for connected, is true if connected to server before handshake
    @property
    def connected(self):
        return self.__connected

    @property
    def isReady(self):
        return self.__isReady

    @property
    def appTypes(self):
        return self.__appTypes

    @property
    def peers(self):
        return self.__peers
