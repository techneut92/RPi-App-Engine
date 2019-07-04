from RPAE import RpaeApp


class ExampleApp(RpaeApp):
    def __init__(self):
        # set the name of your app configuration, usually app.ini. default is 'app.ini'
        super().__init__()
        # super().__init__(configFile='myconfig.ini')

    def onMessage(self, message, origin):
        print("new message from uid:", origin['uid'], "message:", message)

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    # executes when the connection is made and the handshake was successful
    def onOpen(self):
        print('available peers: ', self.peers)
        print("my app ID:", self.appID)
        print("my uid:", self.uid)
        print("my location:", self.location)
        print("number of connected peers", len(self.peers))
        print("connected with:", self.host)
        print("app type:", self.appType)
        print("peer address:", self.peerAddress)
        print("peer name:", self.peerName)
        print("connected as:", self.peerOrigin)
        print("isReady is true once the handshake is done.", self.isReady)
        print("Apptypes contains targets one can send messages too", self.appTypes)
        # lets send hello to all the webclients!
        self.sendMessage(message="hello to all clientApps!")
        # it's a bit sad if we left anyone out. (self.appTypes[0] contains 'all')
        self.sendMessage(message="hello to all!", appType=self.appTypes[0])  # default app type is: clientApp

    # executes when a new clients is connected
    def onNewPeer(self, peer):
        print("new peer connected with uid:", peer.uid, "app type:", peer.appType)
        print("lets say hello to our new friend!")
        self.sendMessage(message='hello new peer!!', appType='uid', uidTarget=peer.uid)

    def onPeerDisconnected(self, peer):
        print("Bye peer! uid:", peer.uid, "app type:", peer.appType)

    def init(self):
        print("init is called when the start function triggers.")
        print("This function triggers before the handshake is done. as you can see connected is", self.connected)


# The main function to start the app
if __name__ == "__main__":
    app = ExampleApp()      # First create an object of your app
    ExampleApp().start()    # Connect to the server and start listening
