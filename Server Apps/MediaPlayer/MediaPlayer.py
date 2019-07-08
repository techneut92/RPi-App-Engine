from Controller import Controller
import subprocess
import os
from IcyData import IcyData
import signal
import json
import atexit


class MediaPlayer:
    __controller = None
    __playerType = 'shell'
    __playerCommand = 'mplayer'
    __playerProcess = None
    __playingFile = None
    __fileData = None

    def __init__(self):
        self.__controller = Controller(self)
        # do stuff
        self.__controller.start()
        atexit.register(self.stop)

    def __del__(self):
        self.stop()

    def play(self, file, name=None):
        if self.isPlaying:
            self.stop()
        shell = False
        if self.__playerType == 'shell':
            shell = True
        self.__playerProcess = subprocess.Popen(self.__playerCommand + ' ' + file, shell=shell,
                                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                                preexec_fn=os.setsid)
        self.__playingFile = file
        print('GOING TO FILL FILEDATA:', file, name)
        self.__fillFileData(file, name)

    def stop(self):
        os.killpg(os.getpgid(self.__playerProcess.pid), signal.SIGTERM)
        self.__fileData.close()
        self.__playingFile = None
        self.__playerProcess = None

    def __fillFileData(self, file, name=None):
        if file.lower().startswith('http://') or file.lower().startswith('https://'):
            self.__fileData = IcyData(r_url=file, onUpdate=self.__onIcyUpdate, name=name)
            print(self.__fileData)

    def __onIcyUpdate(self, data):
        data['action'] = 'updateIcyData'
        self.__controller.sendMessage(json.dumps(data))

    @property
    def isPlaying(self):
        return self.__playingFile is not None

    @property
    def playingFile(self):
        return self.__playingFile

    def sendStatus(self, peer):
        self.__controller.sendMessage(self.status, peer.appType, peer.uid)

    @property
    def playing(self):
        return self.__playingFile is not None

    @property
    def status(self):
        data = {
            'playing': self.playing,
        }
        if self.playing:
            data['fileData'] = self.__fileData.data
        return json.dumps(data)


# The main function to start the app
if __name__ == "__main__":
    app = MediaPlayer()      # First create an object of your app
