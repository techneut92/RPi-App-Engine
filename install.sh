#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

#recreate install file
rm ./Install.log.bckp >/dev/null 2>&1
mv ./Install.log ./Install.log.bckp > /dev/null 2>&1
touch ./Install.log
mkdir -p /etc/rpae
#mkdir -p /etc/rpae/ssl
#chown root:root /etc/rpae/ssl
#chmod 700 /etc/rpae/ssl
mkdir -p /etc/rpae/server

set -e
function err_handler {
if [ $? -ne 0 ] ;then
    echo "An error has occured. Aborting installation. check Install.log for more info."
else
    rm ./Install.log ./Install.log.bckp > ./Install.log 2>&1
    echo -n "Installation completed. a reboot is required, reboot now? [y/n]: "
    read rb
    if [ "$rb" != "${rb#[Yy]}" ] ;then
        reboot
    fi
fi
}
trap err_handler EXIT

#################################################################################
#######                      ASK ALL QUESTIONS HERE                       #######
#################################################################################
echo "The install of the raspberry pi app engine will now begin."
# ask to enable/disable SSH
echo "SSH is not needed for the installation or functionality, however it's always a good thing to have enabled."
SSH_EN=/etc/systemd/system/sshd.service
SSH_ENABLED="null"
if test -f "$SSH_EN"; then
    echo -n "SSH is already enabled.\n"
    #read a1
    #if [ "$a1" != "${a1#[Yy]}" ] ;then
    #    SSH_ENABLED = "FALSE"
    #fi
else
    echo -n "SSH is disabled. Enable ssh? [y/n]: "
    read a2
    if [ "$a2" != "${a2#[Yy]}" ] ;then
        SSH_ENABLED = "TRUE"
    fi
fi

# TODO ask openbox full screen chromium
#OBOX_FULL="null"
#echo "If you only run the webclient and nothing else on this raspberry pi it's a good practice to set up openbox with the rpae GUI full screen. If you still wish to use other apps it's best to select 'n'"
#echo -n "Do you wish to install openbox with the full screen rpae GUI? [y/n]: "
#read ofs
#if [ "$ofs" != "${ofs#[Yy]}" ]; then
#    OBOX_FULL="TRUE"
#else
#    OBOX_FULL="FALSE"
#fi

# TODO ask unclutter
#UNCLUTTER="null"
#echo "Unclutter is a program that removes the mouse from the screen for usage with touch screens."
#echo -n "Do you wish to install unclutter? [y/n]: "
#read unc
#if [ "$unc" != "${unc#[Yy]}" ]; then
#    UNCLUTTER="TRUE"
#else
#    UNCLUTTER="FALSE"
#fi

#Run the updates
echo "Running apt-get update..."
apt-get update > ./Install.log
echo "Running apt-get upgrade and dist-upgrade..."
apt-get upgrade -y > /Install.log
apt-get dist-upgrade -y >/Install.log

#install depency's
echo "Installing depency's..."
sudo apt-get install -y php7.0 php7.0-mcrypt php7.0-mysql mplayer libasound2-dev gcc python3-dev \
apache2 qt5-qmake qt5-default libqt5websockets5 libqt5websockets5-dev python3 > ./Install.log
python3 -m pip install --upgrade pip > ./Install.log
python3 -m pip install websocket_client > ./Install.log
python3 -m pip install pyalsaaudio > ./Install.log

#check if ssh is enabled/disabled, Ask for to either dis/enable ssh.
SSH_EN=/etc/systemd/system/sshd.service
if [ "$SSH_ENABLED" == "FALSE" ] ;then
    systemctl stop ssh >./Install.log 2>&1
    systemctl disable ssh >./Install.log 2>&1
    echo "SSH disabled..."
fi
if [ "$SSH_ENABLED" == "TRUE" ] ;then
    systemctl enable ssh >./Install.log 2>&1
    systemctl start ssh >./Install.log 2>&1
    echo "SSH enabled..."
fi

#compile websocket server
OLD_DIR="$(pwd)"
cd Server
echo "Compiling websocket server....."
qmake ./Server.pro >./Install.log
make -j 8 > ./Install.log
cd "$OLD_DIR"

#install websocket server
echo "Installing websocket server..."
SL=/usr/bin/rpae-server
if test -L "$SL"; then
    rm -f /usr/bin/rpae-server
fi
ln -s "$(pwd)/Server/Server" /usr/bin/rpae-server > ./Install.log
mkdir -p /etc/rpae/server > ./Install.log
cp -ar ./Server/server.ini /etc/rpae/server/ > ./Install.log

######## TODO CREATE SERVICE ###############
#echo "Creating Server service..."

######## TODO SET BOOTUP TO OPENBOX AND AUTO OPEN IN FULLSCREEN CHROMIUM #######
#if [ "$OBOX_FULL" == "TRUE" ];then
#    echo "Installing openbox..."
#fi
######## TODO SET UNCLUTTER #########

######## TODO INSTALL WEBCLIENT ########
rm -rf /var/www/html/*
cp -ar ./Webclient/* /var/www/html/

######## INSTALL SERVER LIBS ########
RPLIBS=/usr/local/lib/python3*/dist-packages/RPAE
if test -L "$RPLIBS"; then
    rm -rf /usr/local/lib/python3*/dist-packages/RPAE
fi

# PROTOTYPE PRESENTATION, REMOVE LATER
rpae-server &
python3 ./Server\ Apps/Rpae-Audio/AudioController.py &
python3 ./Server\ Apps/MediaPlayer/MediaPlayer.py &
chromium-browser localhost &

