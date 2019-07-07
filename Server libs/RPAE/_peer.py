import configparser


class Peer:
    __appID = None
    __location = None
    __uid = None
    __appType = None
    __peerAddress = None
    __peerName = None
    __peerOrigin = None
    __config = configparser.ConfigParser()
    __updateSelfDone = False

    def __init__(self, uid=None, location=None, appID=None, appType=None,
                 peerAddress=None, peerName=None, peerOrigin=None, clientDict=None, configFile=None):
        if configFile is not None:
            config = configparser.ConfigParser()
            config.read(configFile)
            # self.__appID = config['app']['appID']
            # self.__location = config['app']['location']
            self.__init(uid=uid, location=config.get('app', 'location'), appID=config.get('app', 'appID'),
                        appType=appType, peerAddress=peerAddress,
                        peerName=peerName, peerOrigin=peerOrigin, clientDict=clientDict)
        else:
            self.__init(uid=uid, location=location, appID=appID,
                        appType=appType, peerAddress=peerAddress,
                        peerName=peerName, peerOrigin=peerOrigin, clientDict=clientDict)

    # this function basically is called once. to prevent usage again it checks with a bool.
    # it will call the init function and update how the server registered this client
    def _updateSelf(self, data):
        if not self.__updateSelfDone:
            self.__init(clientDict=data)
            self.__updateSelfDone = True
            return True
        else:
            return False

    def __init(self, uid=None, location=None, appID=None, appType=None,
               peerAddress=None, peerName=None, peerOrigin=None, clientDict=None):
        if clientDict is None:  # TODO DEBUG AND FIND OUT WHY THIS IS CALLED TWICE WHEN DECLARING A NEW OBJECT
            self.__uid = uid
            self.__location = location
            self.__appID = appID
            self.__appType = appType
            self.__peerAddress = peerAddress
            self.__peerName = peerName
            self.__peerOrigin = peerOrigin
        elif clientDict is not None:
            if 'uid' in clientDict:
                self.__uid = clientDict['uid']
            if 'location' in clientDict:
                self.__location = clientDict['location']
            if 'appId' in clientDict:
                self.__appID = clientDict['appId']
            if 'appType' in clientDict:
                self.__appType = clientDict['appType']
            if 'peerAddress' in clientDict:
                self.__peerAddress = clientDict['peerAddress']
            if 'peerName' in clientDict:
                self.__peerName = clientDict['peerName']
            if 'peerOrigin' in clientDict:
                self.__peerOrigin = clientDict['peerOrigin']

    @property
    def uid(self):
        return self.__uid

    @property
    def appID(self):
        return self.__appID

    @property
    def appType(self):
        return self.__appType

    @property
    def peerAddress(self):
        return self.__peerAddress

    @property
    def peerName(self):
        return self.__peerName

    @property
    def peerOrigin(self):
        return self.__peerOrigin

    @property
    def config(self):
        return self.__config

    @property
    def location(self):
        return self.__location
