from RPAE import RpaeApp
from AlsaController import AlsaController
import json


class AudioController(RpaeApp):
    def __init__(self):
        super().__init__()
        self.name = "AudioController"
        self._alsa_controller = AlsaController()

    def onMessage(self, message, origin):
        data = json.loads(message)
        if 'task' in data:
            print(data)
            if data['task'] == "setVolume":
                print('action 1')
                self._set_volume(origin, data)

    def onOpen(self):
        status = self._get_status()
        status['task'] = "init"
        self.sendMessage(json.dumps(status))

    def onNewPeer(self, peer):
        status = self._get_status()
        status['task'] = "init"
        self.sendMessage(json.dumps(status), peer.appType, peer.uid)

    def _set_volume(self, origin, data: dict):
        """
        Sets the volume of a mixer, if a mixer isn't in data then the default volume is edited
        :param origin: dict with origin
        :param data: dict with data
        :return: none
        """
        data['task'] = 'update'
        data['origin'] = origin
        if 'mixer' in data and 'value' in data and data['mixer'] != 'default':
            self._alsa_controller.mixers[data['mixer']].volume = data['value']
            self.sendMessage(json.dumps(data), 'clientApp')
        elif 'mixer' in data and 'value' in data and data['mixer'] == 'default':
            self._alsa_controller.volume = data['value']
            self.sendMessage(json.dumps(data))
        else:
            print("Invalid package received: ", data)
            data['task'] = 'ERROR'
            # self.sendMessage(json.dumps(data), origin.appType, origin.uid)

    def _get_status(self):
        """
        Returns the status of all mixers
        :return: dict, data containing everything from the mixers
        """
        data = {"reset_volume_at_start": self._alsa_controller.reset_volume_at_start,
                "startup_volume": self._alsa_controller.startup_volume,
                "volume": self._alsa_controller.volume,
                "mute": self._alsa_controller.muted(),
                "mixers": {}}
        for mixer in self._alsa_controller.mixers.values():
            d = {mixer.name: {}}
            d[mixer.name]['volume'] = mixer.volume
            d[mixer.name]['mute'] = mixer.muted()
            d[mixer.name]['startup_volume'] = mixer.startup_volume
            d[mixer.name]['joined_volume'] = mixer.joined_volume
            d[mixer.name]['channels'] = {}
            for chan in mixer.channels:
                d2 = {chan.name: {}}
                d2[chan.name]['balance'] = chan.balance
                d2[chan.name]['cid'] = chan.cid
                d2[chan.name]['volume'] = chan.volume
                d[mixer.name]['channels'].update(d2)
            data['mixers'].update(d)
        return data


if __name__ == "__main__":
    app = AudioController()      # First create an object of your app
    app.start()
