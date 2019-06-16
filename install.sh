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
function err_handler {
if [ $? -ne 0 ] ;then
    echo "An error has occured. Aborting installation. check Install.log for more info."
else
    rm ./Install.log ./Install.log.bckp
    echo -n "Installation completed. a reboot is required, reboot now? [y/n]: "
    read a3
    if [ "$a3" != "${a3#[Yy]}" ] ;then
        reboot
    fi
fi
}
trap err_handler EXIT

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

#generate ssl keypair for the server
SSL_EN=./ssl/rpi-app-engine.crt
if test -f "$SSL_EN"; then
    echo "SSL keys already generated..."
else
    mkdir -p ./ssl
    chown root:root ./ssl
    chmod 700 ./ssl
    echo -n "Generate SSL keys now (You can always do this later manually)? [y/n]: "
    read a4
    if [ "$a4" != "${a4#[Yy]}" ] ;then
        echo "Generating ssl keypair for secure websocket connections. \nplease fill in the required settings..."
        openssl req -x509 -nodes -days 1000 -newkey rsa:2048 -keyout ./ssl/rpi-app-engine.key -out ./ssl/rpi-app-engine.crt
        chmod 600 ./ssl/*
    fi
fi
