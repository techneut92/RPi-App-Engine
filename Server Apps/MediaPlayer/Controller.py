from RPAE import RpaeApp
import subprocess
import os


class Controller(RpaeApp):
    def __init__(self, mediaplayer):
        # set the name of your app configuration, usually app.ini. default is 'app.ini'
        super().__init__()
        self.mediaPlayer = mediaplayer
        self.mp_pid = None
        self.mp_proc = None

    def onMessage(self, message, origin):
        print("new message from uid:", origin['uid'], "message:", message)
        if message.startswith('PLAY'):
            msg = message.split(' ')
            if len(msg) > 2:
                self.mediaPlayer.play(file=msg[1], name=msg[2])
            else:
                self.mediaPlayer.play(file=msg[1])
        elif message == 'STOP':
            self.mediaPlayer.stop()

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    # executes when the connection is made and the handshake was successful
    def onOpen(self):
        pass

    # executes when a new clients is connected
    def onNewPeer(self, peer):
        self.mediaPlayer.sendStatus(peer)

    def onPeerDisconnected(self, peer):
        pass


# The main function to start the app
if __name__ == "__main__":
    app = MediaPlayer()      # First create an object of your app
    MediaPlayer().start()    # Connect to the server and start listening
