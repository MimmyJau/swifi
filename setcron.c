#include <stdio.h>
#include <stdlib.h>	// For system()
#include <string.h>	// For strcmp()
#include <inttypes.h>	// For strtol()

int addcron(int, int);

// Takes 3 arguments: 1) on or off, 2) start time and 3) end time
// Arguments 2) and 3) are conditional on arguments 1) and 2) respectively
int main(int argc, char *argv[]) {
	int start_time = 0;
	int end_time = 0;

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

	if (!strcmp(argv[1], "on")) {
		addcron(start_time, end_time);
	} else if (!strcmp(*argv, "off")) {
		// To write delcron() function 
		return 0;
	} else {
		return 1;
	}
	return 0;
}

// Function for creating cronjob
int addcron(int start_time, int end_time) {
	char add_cron_command[1000];
	(void) end_time;
	sprintf(add_cron_command, "crontab -l ; echo '0 %d * * * /Users/mimmyjau/Projects/wifi-scheduler/a.out wifioff' | sort - | uniq - | crontab -", start_time);
	printf("add_cron_command: %s\n", add_cron_command);
	// system(add_cron_command);
	return 0;
}
