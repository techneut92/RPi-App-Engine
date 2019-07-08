import threading
import configparser
import time
import alsaaudio


class AudioConfig(configparser.ConfigParser):
    def __init__(self, config_file):
        """
        Extension to configparser especially made for AudioController
        :param config_file: string, path to config file
        """
        configparser.ConfigParser.__init__(self, allow_no_value=True)
        self._config_file = config_file
        self.read(self._config_file)
        self._update_list = []
        self._updater = threading.Thread(target=self._config_updater).start()

    def _config_updater(self):
        """
        Retrieves data from self._update_list and updates changed values every second.
        :return: none
        """
        while True:
            needs_update = self._update_list
            self._update_list = []
            for item in needs_update:
                self.set(item[0], item[1], item[2])
            self.flush_config()
            time.sleep(1)

    def update_value(self, section, key, value, immediate=False):
        """
        A timed updater for the config file.
        :param section: string, section
        :param key: string, key
        :param value: string, value
        :param immediate: boolean, default: False, if set to True the config is flushed directly
        :return: none
        """
        if immediate:
            self.set(section, key, value)
            self.flush_config()
        else:
            append = True
            for item in self._update_list:
                if item[0] == key and item[1] == section:
                    self._update_list.remove(item)
                    self._update_list.append([section, key, value])
                    append = False
            if append:
                self._update_list.append([section, key, value])

    def flush_config(self):
        """
        Writes pending data to the config file
        :return: none
        """
        if self._config_file:
            with open(self._config_file, 'w') as configfile:
                self.write(configfile)

    def in_section(self, section, key):
        """
        Checks if a key is available in a section
        :param section: string, section
        :param key: string, key
        :return: Returns False if the key is not available else True
        """
        keys = []
        for k in self.items(section):
            keys.append(k[0])
        if key in keys:
            return True
        else:
            return False

    @staticmethod
    def _get_alsa_mixers():
        """
        Returns all alsa mixers
        :return: dict, alsa mixers
        """
        mixers = {}
        ind = alsaaudio.card_indexes()
        for i in range(0, len(ind)):
            mix = alsaaudio.Mixer(control=alsaaudio.mixers(i)[0], id=0, cardindex=ind[i])
            mixers.update({mix.cardname(): mix})
        return mixers

    def validate_ini(self):
        """
        Generates, validates and corrects (if necessary) the config file.
        Prints the output if any errors were found
        :return: none
        """
        # Create a config file if non-existent
        open(self._config_file, "r")
        # Check if [audio] exists else add.
        if "audio" not in self.sections():
            self.add_section("audio")

        # Check if [mixers_general] exists, else add.
        if "mixers_general" not in self.sections():
            self.add_section("mixers_general")

        if not self.in_section("mixers_general", "check_outside"):
            self.update_value("mixers_general", "check_outside", "1", True)
        elif self.get("mixers_general", "check_outside") != "1" and self.get("mixers_general", "check_outside") != "0":
            print("audio.ini: Invalid variable for 'check_outside' in section [mixers_general]\r\n"
                  "check_outside set to 1")
            self.update_value("mixers_general", "check_outside", "1", True)

        # Add [audio] reset_volume_at_start if non-existent and set to 1, else validate it is correct.
        if not self.in_section("audio", "reset_volume_at_start"):
            self.update_value("audio", "reset_volume_at_start", "1", True)
        elif self.get("audio", "reset_volume_at_start") != "1" and self.get("audio", "reset_volume_at_start") != "0":
            print("audio.ini: Invalid variable for 'reset_volume_at_start' in section [audio]\r\n"
                  "Reset_volume_at_start set to 1")
            self.update_value("audio", "reset_volume_at_start", "1", True)

        # Add [audio] startup_volume if non-existent and set to 50, else grab the data and validate
        if not self.in_section("audio", "startup_volume"):
            self.update_value("audio", "startup_volume", "50", True)
        elif 100 < int(self.get("audio", "startup_volume")) < 0:
            print("audio.ini: Invalid variable for 'startup_volume' in section [audio]\r\n"
                  "volume set to 50")
            self.update_value("audio", "startup_volume", "50", True)

        # Add [audio] reset_volume_at_start if non-existent and set to 50, else validate it is correct.
        if not self.in_section("audio", "volume"):
            self.update_value("audio", "volume", "50", True)
        elif 100 < int(self.get("audio", "volume")) < 0:
            print("audio.ini: Invalid variable for 'volume' in section [audio]\r\n"
                  "volume set to 50")
            self.update_value("audio", "volume", "50", True)

        # Add [audio] mute if non-existent and set to 0, else validate it is correct.
        if not self.in_section("audio", "mute"):
            self.update_value("audio", "mute", "0", True)
        elif self.get("audio", "mute") != "1" and self.get("audio", "mute") != "0":
            print("audio.ini: Invalid variable for 'mute' in section [audio]\r\n"
                  "Reset_volume_at_start set to 0")
            self.update_value("audio", "mute", "0", True)

        # Get all mixers from the alsa library.
        for cardname, mixer in self._get_alsa_mixers().items():
            # Check if a section [cardname] is existent, else add it
            if cardname not in self.sections():
                self.add_section(cardname)

            # Check if [cardname] easy_name exists, else add it.
            if not self.in_section(cardname, "easy_name"):
                self.update_value(cardname, "easy_name", cardname, True)

            # Add [audio] joined_volume if non-existent and set to 1, else validate it is correct.
            if not self.in_section(cardname, "joined_volume"):
                self.update_value(cardname, "joined_volume", "1", True)
            elif self.get(cardname, "joined_volume") != "1" and self.get(cardname, "joined_volume") != "0":
                print("audio.ini: Invalid variable for 'joined_volume' in section [" + cardname + "]\r\n",
                      "joined_volume set to 1")
                self.update_value(cardname, "joined_volume", "1", True)

            # Add [audio] volume if non-existent and set to 50, else validate it is correct.
            if not self.in_section(cardname, "volume"):
                self.update_value(cardname, "volume", "50", True)
            elif 100 < int(self.get(cardname, "volume")) < 0:
                print("audio.ini: Invalid variable for 'volume' in section [" + cardname + "]\r\n",
                      "volume set to 50")
                self.update_value(cardname, "volume", "50", True)

            # Add [audio] mute if non-existent and set to 0, else validate it is correct.
            if not self.in_section(cardname, "mute"):
                self.update_value(cardname, "mute", "0", True)
            elif self.get(cardname, "mute") != "1" and self.get(cardname, "mute") != "0":
                print("audio.ini: Invalid variable for 'mute' in section [" + cardname + "]\r\n"
                      "Reset_volume_at_start set to 0")
                self.update_value(cardname, "mute", "0", True)

            # Add [audio] startup_volume if non-existent and set to 50, else validate it is correct.
            if not self.in_section(cardname, "startup_volume"):
                self.update_value(cardname, "startup_volume", "50", True)
            elif 100 < int(self.get(cardname, "startup_volume")) < 0:
                print("audio.ini: Invalid variable for 'startup_volume' in section [" + cardname + "]\r\n",
                      "volume set to 50")
                self.update_value(cardname, "startup_volume", "50", True)

            # Set each individual channel of [cardname]
            for i in range(0, len(mixer.getvolume())):
                chn = "channel_" + str(i) + "_name"
                ch = "channel_" + str(i)
                if len(mixer.getvolume()) == 1:
                    chname = "mono"
                elif len(mixer.getvolume()) == 2 and i == 0:
                    chname = "left"
                elif len(mixer.getvolume()) == 2 and i == 1:
                    chname = "right"
                else:
                    chname = ch

                if not self.in_section(cardname, chn):
                    self.update_value(cardname, chn, chname, True)

                if not self.in_section(cardname, ch):
                    self.update_value(cardname, ch, "5", True)
                elif 10 < int(self.get(cardname, ch)) < 0:
                    print("audio.ini: Invalid variable for '" + ch + "' in section [" + cardname +
                          "]\r\n" + ch + " set to 5")
                    self.update_value(cardname, ch, "5", True)

    def add_section(self, section):
        """
        Add section to config file and flush it.
        :param section: string, section
        :return: none
        """
        super().add_section(section)
        self.flush_config()
