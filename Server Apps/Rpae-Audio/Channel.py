class Channel:
    def __init__(self, cid, mixer):
        """
        This class handles a specific channel for an alsaaudio.Mixer() and is used in Mixer.
        A channel is usually the mono, right or left channel of an audio device.
        :param cid:
        :param mixer:
        """
        self._name = None
        self._cid = cid
        self._balance = None
        self._gen_name = None
        self._volume = None
        self._mixer = mixer
        self._config = self._mixer.config
        self._amixer = self._mixer.mixer
        self.load_ini()

    def load_ini(self):
        """
        Imports Channel data from the config file and set variables accordingly
        :return: none
        """
        self._gen_name = "channel_" + str(self._cid)
        ch_name_key = self.gen_name + "_name"

        self._name = self._config.get(self._mixer.cardname, ch_name_key)
        self._balance = int(self._config.get(self._mixer.cardname, self._gen_name))

    @property
    def name(self):
        """
        The easy name of a channel. This name, if not manually set, usually contains 'left', 'right', 'mono' or
        'channel_x' where x is a numeric value.
        :return: string, easy name of a channel
        """
        return self._name

    @name.setter
    def name(self, value):
        """
        name.setter
        :param value: string, easy name for a specific channel. Setting this will also update the config file.
        :return: none
        """
        self._name = value
        self._config.update_value(self._mixer.cardname, self._gen_name + "_name", value)

    @property
    def balance(self):
        """
        This property holds a numeric value between 0 and 10. When Mixer.volume is set all the balances are requested
        to calculate each channels individual volume. Be aware the balance is calculated in the following way:
        highest balance = Mixer.volume
        other balances = Mixer.volume * ( other balance / highest balance)
        Because of this, if the volume is 50 and all balances are equal (e.g. left and right are both 1 or perhaps 10)
        the volume of all channels are still 50.
        One exception is when the balance is 0, then, because a division on 0 is made, the channel is muted.
        :return: Numeric value between 0 and 10
        """
        return self._balance

    @balance.setter
    def balance(self, value):
        """
        balance.setter. Setting this will also update the config file.
        :param value: Numeric value between 0 and 10
        :return:
        """
        old_val = self._balance
        if 0 <= int(value) <= 10:
            if old_val != int(value):
                self._balance = int(value)
                self._config.update_value(self._mixer.cardname, self._gen_name, str(value))
                # setting the mixers volume to itself will invoke the setter, balancing the volumes again.
                self._mixer.volume = self._mixer.volume
        else:
            print("attempted to set channel balance mixer: [" + self._mixer.cardname + "] but received invalid value: " +
                  str(value))

    @property
    def volume(self):
        """
        Returns the alsa audio volume for a specific channel
        :return: Numeric value between 0 and 100
        """
        return self._amixer.getvolume()[self._cid]

    @volume.setter
    def volume(self, value):
        """
        volume.setter. Directly updates the alsa mixer volume for this specific channel.
        :param value: Numeric value between 0 and 100
        :return: none
        """
        self._amixer.setvolume(value, self._cid)

    @property
    def gen_name(self):
        """
        Returns a generated name in the format 'channel_x' where x is the channel id.
        This is mainly used for config related purposes
        :return: string, self generated channel name
        """
        return self._gen_name

    @property
    def cid(self):
        return self._cid
