from RPAE import RpaeApp


class ExampleApp(RpaeApp):
    def __init__(self):
        super().__init__()

    # Receives messages from other Server or Client Apps.
    def onMessage(self, message, origin):
        pass

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    # Triggers once the handshake has completed.
    def onOpen(self):
        pass  # Your code here

    # executes when a new clients is connected
    def onNewPeer(self, peer):
        pass

    def onPeerDisconnected(self, peer):
        pass


# The main function to start the app.
if __name__ == "__main__":
    app = ExampleApp()      # First create an object of your app
    ExampleApp().start()    # Connect to the server and start listening
