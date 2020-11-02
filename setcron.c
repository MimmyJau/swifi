#include <stdio.h>
#include <stdlib.h>	// For system()
#include <string.h>	// For strcmp()
#include <inttypes.h>	// For strtol()

int addcron(int, int);
int delcron(int, int);

// Takes 3 arguments: 1) on or off, 2) start time and 3) end time
// Arguments 2) and 3) are conditional on arguments 1) and 2) respectively
int main(int argc, char *argv[]) {
	int start_time = 0;
	int end_time = 0;

	// Set start_time and/or end_time 
	switch (argc) {
		case 1:
			return 1;
		case 2:
			break;
		case 3:
			start_time = (int) strtol(argv[2], NULL, 10);
			break;
		case 4:
			start_time = (int) strtol(argv[2], NULL, 10);
			end_time = (int) strtol(argv[3], NULL, 10);
			break;
	}
	
	if (!strcmp(argv[1], "add")) {
		// del all crons first
		addcron(start_time, end_time);
	} else if (!strcmp(argv[1], "rmv")) {
		// To write delcron() function 
		delcron(start_time, end_time);
	} else if (!strcmp(argv[1], "list")) {
		// To write cat list function
		printf("We listing brahs\n");
		return 0;
	} else {
		return 1;
	}
	return 0;
}

// Function for creating cronjob
int addcron(int start_time, int end_time) {
	char cron_wifioff[1000];
	char cron_wifion[1000];

	// Is there a cleaner way of preparing these commands?
	sprintf(cron_wifioff, "(crontab -l ; echo '0 %d * * * /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifioff >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", start_time);
	sprintf(cron_wifion, "(crontab -l ; echo '0 %d * * * /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifion >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", end_time);

	system(cron_wifioff);
	system(cron_wifion);
	return 0;
}

// Function for writing cronjobs to file

// Function for listing cronjobs

// Function for deleting crons related to ./a.out wifioff
int delcron(int start_time, int end_time) {
	// rcron for remove cron
	char rcron_wifioff[1000];
	char rcron_wifion[1000];

	// Is there a cleaner way of preparing these commands?
	sprintf(rcron_wifioff, "(crontab -l | sort - | uniq - | grep -v '0 %d \\* \\* \\* /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifioff >>/dev/null 2>>/dev/null') | crontab -", start_time);
	sprintf(rcron_wifion, "(crontab -l | sort - | uniq - | grep -v '0 %d \\* \\* \\* /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifion >>/dev/null 2>>/dev/null') | crontab -", end_time);

	system(rcron_wifioff);
	system(rcron_wifion);
	return 0;
}
