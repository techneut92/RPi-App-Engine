#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

#Run the updates
apt-get update
apt-get upgrade -y
apt-get dist-upgrade -y

#install depency's
sudo apt-get install -y php7.0 php7.0-mcrypt php7.0-mysql apache2

#check if ssh is enabled/disabled, Ask for to either dis/enable ssh.
SSH_EN=/etc/systemd/system/sshd.service
if test -f "$FILE"; then
    echo -n "SSH is already enabled. Disable SSH? [y/n]"
    read a1
     if [ "$a1" != "${answer#[Yy]}" ] ;then
        systemctl stop ssh
        systemctl disable ssh
        echo "SSH is disabled."
    else
        echo "SSH is enabled."
    fi
else
    echo -n "SSH is disabled. Enable ssh? [y/n]"
    read a2
    if [ "$a2" != "${answer#[Yy]}" ] ;then
        systemctl enable ssh
        systemctl start ssh
        echo "SSH Enabled"
    else
        echo "SSH is disabled."
    fi
fi

echo "Installation done, please reboot"
#sudo reboot
