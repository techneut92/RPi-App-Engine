from .Channel import Channel
import time


class Mixer:
    def __init__(self, mixer, config):
        """
        This class handles the audio settings for a specific alsaaudio.Mixer() and saves
        those settings in the config file.
        :param mixer: alsaaudio.Mixer() object
        :param config: AudioConfig object
        """
        self._config = config
        self._volume = None
        self._name = None
        self._joined_volume = None
        self._startup_volume = None
        self._mute = None
        self._channels = []
        self._mixer = mixer
        self._cardname = self._mixer.cardname()
        self._load_ini()

    def _check_outside(self, once=False):
        """
        Preparation for checking outside changes made to alsa.
        As I personally am not going to use this I will leave it like this.
        :param once: boolean, if set to true it is run once, else indefinitely
        :return:
        """
        run = True
        while run:
            vol = self._mixer.getvolume()
            for chan in self._channels:
                if chan.volume != vol:
                    # fix it
                    pass
            if once:
                run = False
            time.sleep(1)

    def _load_ini(self):
        """
        Imports Mixer data from the config file and sets variables and mixers accordingly
        variables set:
            _name
            _joined_volume
            _startup_volume
            _volume
        :return: none
        """
        for i in range(0, len(self._mixer.getvolume())):
            self._channels.append(Channel(i, self))
        self._name = self._config.get(self.cardname, "easy_name")
        self.joined_volume = self._config.get(self.cardname, "joined_volume")
        self.startup_volume = self._config.get(self.cardname, "startup_volume")
        mute = self._config.get(self.cardname, "mute")
        if mute == "1":
            self._mute = True
        if mute == "0":
            self._mute = False
        self._set_volume_variable()

    def _set_volume_variable(self):
        """
        Loads the correct volume variable from the config file and activate volume.setter.
        :return: none
        """
        if self._config.get("audio", "reset_volume_at_start") == "1":
            if self.joined_volume:
                self.volume = int(self._config.get("audio", "startup_volume"))
            else:
                self.volume = int(self._config.get(self.cardname, "startup_volume"))
        else:
            if self.joined_volume:
                self.volume = int(self._config.get("audio", "volume"))
            else:
                self.volume = int(self._config.get(self.cardname, "volume"))
        # self.volume = self._volume

    def mute(self):
        """
        Mute Mixer
        :return: none
        """
        old_val = self._mute
        self._mute = True
        if old_val != self._mute:
            self._config.update_value(self.cardname, "mute", "1")
        self._mixer.setmute(1)

    def unmute(self):
        """
        Unmute Mixer
        :return: none
        """
        old_val = self._mute
        self._mute = False
        if old_val != self._mute:
            self._config.update_value(self.cardname, "mute", "0")
        self._mixer.setmute(0)

    def muted(self):
        """
        Returns False if Mixer is muted
        Returns True if Mixer is not muted
        :return: boolean
        """
        return self._mute

    ####################################################################################################################
    # Property's
    ####################################################################################################################
    @property
    def cardname(self):
        """
        The cardname of Mixer in the format 'hw:x' where x is the card number
        :return: String in format 'hw:x' where x is the card number
        :rtype: string
        """
        return self._cardname

    @property
    def volume(self):
        """
        Mixer.Volume
        This variable can be set to a value between 0 and 100.
        If Mixer.joined_volume is true you can still set the volume, but be aware that
        once AudioController.volume is changed, this will also be changed.

        :return: Numeric value between 0 and 100
        :rtype: int
        """
        return self._volume

    @volume.setter
    def volume(self, value):
        """
        Calculates the volume for each channel according to the set balance, changes, and updates it in the config file.
        :param value: a numeric value between 0 and 100
        :return: none
        """
        old_val = self._volume
        # perhaps a more stable calculation, requires channel_x values in audio.ini 0-100
        # i = 0
        # for chan_name in self.channels.keys():
        #    new_vol_chan = round(new_vol * (self.channels[chan_name] / 100))
        #    self.mixer.set_volume(new_vol_chan, i)
        #    i += 1
        if 0 <= int(value) <= 100:
            self._volume = int(value)
            if old_val != self._volume:
                self._config.update_value(self.cardname, "volume", str(value))
            if len(self._channels) > 1:
                highest = 0
                for ch in self._channels:
                    if highest < ch.balance:
                        highest = ch.balance
                i = 0
                for chan in self._channels:
                    new_vol_chan = round(float(value) * chan.balance / highest)
                    self.channels[i].volume = new_vol_chan
                    i += 1
            else:
                self._channels[0].volume = self._volume
        else:
            print("attempted to set volume mixer: [" + self.name + "] but received invalid value: " + str(value))

    @property
    def name(self):
        """
        Mixer.name
        This variable holds the easy Mixer name.
        When this value is unchanged in either the audio.ini or here this value will be the same as Mixer.cardname
        This value can be changed to anything (e.g.: "usb audio stick 1")
        Change this value either here or in audio.ini.
        In audio.ini only change this variable when Mixer isn't running.
        This variable is found under the section equal to Mixer.cardname and option: easy_name

        :return: Mixer easy name
        :rtype: string
        """
        return self._name

    @name.setter
    def name(self, value):
        """
        name setter. Setting this will also update the config file.
        :param value: any type of string. replaces the easy name for Mixer
        :return: none
        """
        self._name = value
        self._config.update_value(self.cardname, "easy_name", value)

    @property
    def joined_volume(self):
        """
        If this value is True the volume is managed by the AudioController class.
        If this value is False the volume is managed by Mixer, thus Mixer.volume can be set
        :return: boolean
        """
        return self._joined_volume

    @joined_volume.setter
    def joined_volume(self, value):
        """
        joined_volume setter. Setting this will also update the config file.
        :param value: Numeric value between 0 and 100
        :return: none
        """
        # TODO: preserve state when joined volume changes
        old_val = self._joined_volume
        if value == "0" or value == 0 or not value:
            self._joined_volume = False
            if old_val != self._joined_volume:
                self._config.update_value(self.cardname, "joined_volume", "0")
        elif value == "1" or value == 0 or value:
            self._joined_volume = True
            if old_val != self._joined_volume:
                self._config.update_value(self.cardname, "joined_volume", "1")
        else:
            print("attempted to set joined_volume mixer: [" + self.name + "] but received invalid value: ", value)

    @property
    def startup_volume(self):
        """
        The Mixer.volume that is set when Mixer is created.
        :return: Numeric value between 0 and 100
        """
        return int(self._startup_volume)

    @startup_volume.setter
    def startup_volume(self, value):
        """
        startup_volume setter. Setting this will also update the config file.
        :param value: Numeric value between 0 and 100
        :return: none
        """
        old_val = self._startup_volume
        if 0 <= int(value) <= 100:
            self._startup_volume = int(value)
            if old_val != int(value):
                self._config.update_value(self.cardname, "startup_volume", value)
        else:
            print("attempted to set startup_volume mixer: [" + self.name + "] but received invalid value: " +
                  str(value))

    @property
    def channels(self):
        """
        Individual channel of Mixer
        :return: Channel object
        """
        return self._channels

    @property
    def mixer(self):
        """
        Holds the value for the alsaaudio mixer
        :return: alsaaudio.Mixer() object
        """
        return self._mixer

    @property
    def config(self):
        """
        Returns the config file used for Mixer. Mainly used for passing to subclasses
        :return: AudioConfig Object
        """
        return self._config

    @property
    def volumes(self):
        """
        Returns a dict in the format:
        { "volume": mixer volume, "channel_x_name" : channel.volume, ... }
        :return: dict with mixer and channel volumes
        """
        volumes = {"volume": self._volume}
        for chan in self._channels:
            volumes.update({chan.name: chan.volume})
        return volumes
