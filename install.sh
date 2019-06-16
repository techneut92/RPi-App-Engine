#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

#recreate install file
rm ./Install.log.bckp
mv ./Install.log ./Install.log.bckp
touch ./Install.log

#Run the updates
apt-get update > ./Install.log 2>&1
apt-get upgrade -y > /Install.log 2>&1
apt-get dist-upgrade -y >/Install.log 2>&1

#install depency's
sudo apt-get install -y php7.0 php7.0-mcrypt php7.0-mysql \
apache2 > ./Install.log 2>&1

#check if ssh is enabled/disabled, Ask for to either dis/enable ssh.
SSH_EN=/etc/systemd/system/sshd.service
if test -f "$SSH_EN"; then
    echo -n "SSH is already enabled. Disable SSH? [y/n]: "
    read a1
     if [ "$a1" != "${answer#[Yy]}" ] ;then
        systemctl stop ssh >./Install.log 2>&1
        systemctl disable ssh >./Install.log 2>&1
        echo "SSH is disabled."
    else
        echo "SSH is enabled."
    fi
else
    echo -n "SSH is disabled. Enable ssh? [y/n]: "
    read a2
    if [ "$a2" != "${answer#[Yy]}" ] ;then
        systemctl enable ssh >./Install.log 2>&1
        systemctl start ssh >./Install.log 2>&1
        echo "SSH Enabled"
    else
        echo "SSH is disabled."
    fi
fi

echo "Installation done, please reboot"
#sudo reboot
