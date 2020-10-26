#!/bin/bash

# Name of Wi-Fi device is en0
device=en0

# Check status of wi-fi device (is it active or inactive?)
status() {
        /sbin/ifconfig $device | awk '/status:/{print $2}'
}

# If wifi is on, turn off after X seconds
isactive() {
        sleep 10
        /usr/sbin/networksetup -setairportpower $device off
}

# Every 5 seconds, check if wifi is active or inactive
while :; do
        if [ $(status) == active ]
        then isactive
        fi
        sleep 5
done
