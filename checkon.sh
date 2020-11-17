#!/bin/bash

# Set up signal handler that deletes tmp file when script exits
cleanup() {
	rm $PWD/pid_sh.txt
}
trap cleanup EXIT

# Get PID of process and place into file
# NTD: Doesn't work if binaries and / or scripts move to another directory,
# should pick a location that's based on the operating system
echo $$ > $PWD/pid_sh.txt

# Check OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	# If wifi on for ubuntu, keyword is UP
	wifion=UP
elif [[ "$OSTYPE" == "darwin"* ]]; then
	# Name of Wi-Fi device is en0
	device=en0
	# If wifi on for osx, keyword is active
	wifion=active
fi

# Check status of wi-fi device (is it active or inactive?)
status() {
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then
		ip l show enp0s3 | awk '/state/{print $9}'
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		/sbin/ifconfig $device | awk '/status:/{print $2}'
	fi
}

# If wifi is on, turn off after X seconds
isactive() {
        sleep 1
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then
		nmcli networking off
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		/usr/sbin/networksetup -setairportpower $device off
	fi
}

# Every 5 seconds, check if wifi is active or inactive
while :; do
        if [[ "$(status)" == "$wifion" ]]
        then isactive
        fi
        sleep 5
done
