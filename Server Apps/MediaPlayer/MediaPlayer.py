from Controller import Controller
import subprocess
import os
from IcyData import IcyData
import signal


class MediaPlayer:
    __controller = None
    __playerType = 'shell'
    __playerCommand = 'mplayer'
    __playerProcess = None
    __playingFile = None
    __fileData = None

    def __init__(self):
        self.controller = Controller(self)
        # do stuff
        self.controller.start()

    def __del__(self):
        self.stop()

    def play(self, file):
        if self.isPlaying:
            self.stop()
        shell = False
        if self.__playerType == 'shell':
            shell = True
        self.__playerProcess = subprocess.Popen(self.__playerCommand + ' ' + file, shell=shell,
                                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                                preexec_fn=os.setsid)
        self.__playingFile = file
        self.__fillFileData(file)

    def stop(self):
        os.killpg(os.getpgid(self.__playerProcess.pid), signal.SIGTERM)
        self.__playingFile = None
        self.__playerProcess = None

    def __fillFileData(self, file):
        if file.lower().startswith('http://') or file.lower().startswith('https://'):
            self.__fileData = IcyData(file, self.__onIcyUpdate)
            print(self.__fileData.data)

    def __onIcyUpdate(self, data):
        print('icy updated: ', data)

    @property
    def isPlaying(self):
        return self.__playingFile is not None

    @property
    def playingFile(self):
        return self.__playingFile


# The main function to start the app
if __name__ == "__main__":
    app = MediaPlayer()      # First create an object of your app
