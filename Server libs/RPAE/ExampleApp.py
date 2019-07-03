from RpaeApp import RpaeApp


class ExampleApp(RpaeApp):
    def __init__(self):
        # aid is your app ID, MANDATORY!!
        # By default the host is "ws://localhost:9738" thus optional
        super().__init__(aid="exampleApp", host="ws://192.168.2.8:9738")

    def onMessage(self, message, origin):
        print(message)

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    # executes when the connection is made and the handshake was successful
    def onOpen(self):
        print("my uid: ", self.uid)

    # executes when a new clients is connected
    def onNewClient(self, client):
        print(client)
        print(self.peers)


# The main function to start the app
if __name__ == "__main__":
    app = ExampleApp()      # First create an object of your app
    ExampleApp().start()    # Connect to the server and start listening
