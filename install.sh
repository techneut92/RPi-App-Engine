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

#check if ssh is enabled, else ask to enable it
SSH_EN=/boot/ssh
if test -f "$FILE"; then
    echo "SSH is already enabled"
else
    echo -n "Enable ssh? [y/n]
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
        echo "SSH Enabled"
    else
        echo "SSH is disabled."
    fi
fi
