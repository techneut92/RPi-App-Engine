import alsaaudio
from .Mixer import Mixer
from .AudioConfig import AudioConfig


class AlsaController:
    def __init__(self):
        # TODO file path management ?
        self._cfg_file = 'apps/AudioController/audio.ini'
        self._config = AudioConfig(self.cfg_file)
        self._config.validate_ini()
        self.mixers = self._get_mixers()
        self._reset_volume_at_start = None       # load from audio.ini
        self._startup_volume = None              # |->
        self._volume = None                      # |->
        self._mute = None                        # |->
        self._load_ini()

    def _load_ini(self):
        """
        Load all data from the config file. These are all declared by the AudioConfig class beforehand if non-existent.
        :return: none
        """
        self.reset_volume_at_start = self._config.get("audio", "reset_volume_at_start")
        self.startup_volume = self._config.get("audio", "startup_volume")
        if self._config.get("audio", "reset_volume_at_start") == "1":
            self.volume = self._config.get("audio", "startup_volume")
        else:
            self.volume = self._config.get("audio", "volume")
        mute = self._config.get("audio", "mute")
        if mute == "1":
            self._mute = True
        if mute == "0":
            self._mute = False
        self.mute = self._config.get("audio", "mute")

    def _get_mixers(self):
        """
        Get all alsa mixers available and give them a name.
        :return: dict, {mixer1.name: mixer1, ... }
        """
        mixers = {}
        ind = alsaaudio.card_indexes()
        for i in range(0, len(ind)):
            mix = alsaaudio.Mixer(control=alsaaudio.mixers(i)[0], id=0, cardindex=ind[i])
            mixer = Mixer(mix, self._config)
            mixers.update({mixer.name: mixer})
        return mixers

    def mixers_available(self):
        """
        Returns a list of controllable mixers.
        :return: list, [mixer1.name, ...]
        """
        return self.mixers.keys()

    def mixer_volume(self, name):
        """
        Gets the volume of a specific mixer.
        :param name: string, mixerx.name
        :return: int, mixerx.volume
        """
        for n, mix in self.mixers:
            if n == name:
                return mix.volume

    def mute(self):
        for mix in self.mixers.values():
            if mix.joined_volume:
                if not mix.muted():
                    mix.mute()
                    self._mute = True
                    self._config.update_value("audio", "mute", "1")

    def unmute(self):
        for mix in self.mixers.values():
            if mix.joined_volume:
                if not mix.muted():
                    mix.unmute()
                    self._mute = False
                    self._config.update_value("audio", "mute", "0")

    def muted(self):
        return self._mute

    ####################################################################################################################
    @property
    def config(self):
        return self._config

    @property
    def cfg_file(self):
        return self._cfg_file

    @property
    def startup_volume(self):
        return self._startup_volume

    @startup_volume.setter
    def startup_volume(self, value):
        self._startup_volume = value
        self.config.update_value("audio", "startup_volume", str(value))

    @property
    def volume(self):
        return self._volume

    @volume.setter
    def volume(self, value):
        self._volume = value
        self.config.update_value("audio", "volume", str(value))
        for mix in self.mixers.values():
            if mix.joined_volume:
                mix.volume = value

    @property
    def reset_volume_at_start(self):
        return self._reset_volume_at_start

    @reset_volume_at_start.setter
    def reset_volume_at_start(self, value):
        self._reset_volume_at_start = value
        self.config.update_value("audio", "reset_volume_at_start", str(value))
