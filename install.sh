#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

#recreate install file
rm ./Install.log.bckp >/dev/null 2>&1
mv ./Install.log ./Install.log.bckp > /dev/null 2>&1
touch ./Install.log

set -e
trap 'echo "\"${last_command}\" command filed with exit code $?. Aborting installation"' EXIT

#Run the updates
echo "Running apt-get update..."
apt-get update > ./Install.log
echo "Running apt-get upgrade and dist-upgrade..."
apt-get upgrade -y > /Install.log
apt-get dist-upgrade -y >/Install.log

#install depency's
echo "Installing depency's..."
sudo apt-get install -y php7.0 php7.0-mcrypt php7.0-mysql \
apache2 > ./Install.log

#check if ssh is enabled/disabled, Ask for to either dis/enable ssh.
SSH_EN=/etc/systemd/system/sshd.service
if test -f "$SSH_EN"; then
    echo -n "SSH is already enabled. Disable SSH? [y/n]: "
    read a1
     if [ "$a1" != "${a1#[Yy]}" ] ;then
        systemctl stop ssh >./Install.log 2>&1
        systemctl disable ssh >./Install.log 2>&1
        echo "SSH is disabled..."
    fi
else
    echo -n "SSH is disabled. Enable ssh? [y/n]: "
    read a2
    if [ "$a2" != "${a2#[Yy]}" ] ;then
        systemctl enable ssh >./Install.log 2>&1
        systemctl start ssh >./Install.log 2>&1
        echo "SSH Enabled..."
    fi
fi

echo "Installation done, please reboot"
#sudo reboot
