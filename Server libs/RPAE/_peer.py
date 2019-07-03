class Peer:
    __appID = None
    __uid = None
    __appType = None
    __peerAddress = None
    __peerName = None
    __peerOrigin = None
    __appIDSet = False
    __updateSelfDone = False

    def __init__(self, uid=None, appID=None, appType=None,
                 peerAddress=None, peerName=None, peerOrigin=None, clientDict=None):
        self.__init(uid, appID, appType, peerAddress, peerName, peerOrigin, clientDict)

    # this function basically is called once. to prevent usage again it checks with a bool.
    # it will call the init function and update how the server registered this client
    def _updateSelf(self, data):
        if not self.__updateSelfDone:
            self.__init(clientDict=data)
            self.__updateSelfDone = True
            return True
        else:
            return False

    # AppId should only be set once
    def _setAppID(self, aid):
        if not self.__appIDSet:
            self.__appID = aid
            self.__appIDSet = True
            return True
        else:
            return False

    def __init(self, uid=None, appID=None, appType=None,
               peerAddress=None, peerName=None, peerOrigin=None, clientDict=None):
        if clientDict is None:
            self.__uid = uid
            self.__appID = appID
            self.__appType = appType
            self.__peerAddress = peerAddress
            self.__peerName = peerName
            self.__peerOrigin = peerOrigin
        elif clientDict is not None:
            if 'uid' in clientDict:
                self.__uid = clientDict['uid']
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
