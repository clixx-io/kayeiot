#!/bin/sh

export GOPATH=$HOME/go
#export GOROOT=$HOME/.local/share/go
ACLIDIR=$HOME/go/bin
ACLCMD=arduino-cli
ACLCONFIG=$HOME/go/bin/.cli-config.yml

if [ -x "$(go version)" ] 
then 
	echo "Go is required but it's not installed. Exiting."
	exit 1
fi
	
go get -u github.com/arduino/arduino-cli

if [ -e $ACLIDIR/$ACLCMD ]
then
    echo "Installation Succeeded."
else
    echo "Installation failed. arduino-cli not found"
fi

#echo Checking for \.cli-config.yml
if [ -e $ACLIDIR/.cli-config.yml ]
then
 echo  ".cli-config.yml already installed."
else
 cp .cli-config.yml $ACLIDIR/.cli-config.yml
fi

$ACLIDIR/arduino-cli core update-index
$ACLIDIR/arduino-cli core install esp8266:esp8266
$ACLIDIR/arduino-cli core install esp32:esp32
