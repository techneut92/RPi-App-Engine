from RPAE import RpaeApp


class MediaPlayer(RpaeApp):
    def __init__(self):
        # set the name of your app configuration, usually app.ini. default is 'app.ini'
        super().__init__()

    def onMessage(self, message, origin):
        print("new message from uid:", origin['uid'], "message:", message)

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    # executes when the connection is made and the handshake was successful
    def onOpen(self):
        pass

    # executes when a new clients is connected
    def onNewPeer(self, peer):
        pass

    def onPeerDisconnected(self, peer):
        pass


# The main function to start the app
if __name__ == "__main__":
    app = MediaPlayer()      # First create an object of your app
    MediaPlayer().start()    # Connect to the server and start listening
