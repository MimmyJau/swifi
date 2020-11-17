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

# If wifi is on, turn off after tsleep seconds
tsleep=1

case "$(uname -s)" in
Linux)
	# If wifi on for ubuntu, keyword is UP
	wifion=UP
	# Check status of wi-fi device (is it active or inactive?)
	status() {
		# Use sed here whereas Darwin version uses awk.
		# Just for variety; neither method is preferred.
		ip l show enp0s3 | sed 1q | grep -q 'state UP'
	}
	isactive() {
		sleep $tsleep
		nmcli networking off
	}
	# Main loop of script
	while :; do
		status
		if [ $? == 0 ]
		then isactive
		fi
		sleep 5
	done
;;

Darwin)
	# Name of Wi-Fi device is en0
	device=en0
	# If wifi on for osx, keyword is active
	wifion=active
	# Check status of wi-fi device (is it active or inactive?)
	status() {
		/sbin/ifconfig $device | awk '/status:/{print $2}'
	}
	isactive() {
		sleep $tsleep
		/usr/sbin/networksetup -setairportpower $device off
	}
	# Main loop of script
	while :; do
		if [ "$(status)" == "$wifion" ]
		then isactive
		fi
		sleep 5
	done
;;
esac
