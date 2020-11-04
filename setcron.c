#include <stdio.h>
#include <stdlib.h>	// For system()
#include <string.h>	// For strcmp()
#include <inttypes.h>	// For strtol()
#include <unistd.h>	// For getopt()
#include <errno.h>	// For errno

struct hourmin {
	int minute;
	int hour;
};

int addcron(struct hourmin *starttime, struct hourmin *endtime);
int rmvcron(struct hourmin *starttime, struct hourmin *endtime);
int clearcron(void);
int gethourmin(char *, struct hourmin *);

// Takes 3 arguments: 1) on or off, 2) start time and 3) end time
// Arguments 2) and 3) are conditional on arguments 1) and 2) respectively
int main(int argc, char *argv[]) {
	// int start_time = 0;
	// int end_time = 0;
	int ch;
	int addflag, rmvflag, allflag, startflag, endflag;
	struct hourmin starttime = { 0 , 0 };
	struct hourmin endtime = { 0 , 0 };
	addflag = rmvflag = allflag = startflag = endflag = 0;

	// NTD: Not sure what to do with this block
	// Set start_time and/or end_time 
	switch (argc) {
	case 1:
		return 1;
	case 2:
		break;
	case 3:
		// start_time = (int) strtol(argv[2], NULL, 10);
		break;
	case 4:
		// start_time = (int) strtol(argv[2], NULL, 10);
		// end_time = (int) strtol(argv[3], NULL, 10);
		break;
	}

	while (optind < argc) {
		if ((ch = getopt(argc, argv, "s:e:")) != -1) {
			switch (ch) {
			case 's':
				startflag = 1;
				gethourmin(optarg, &starttime);
				break;
			case 'e':
				endflag = 1;
				gethourmin(optarg, &endtime);
				break;
			default:
				break;
			}
		} else if (!strcmp(argv[optind], "add")) {
			addflag = 1;
			optind++;
		} else if (!strcmp(argv[optind], "rmv")) {
			rmvflag = 1;
			optind++;
		} else if (!strcmp(argv[optind], "clear")) {
			// Write function that removes all cronjobs 
			// and turns on wifi
			clearcron();
			optind++;
		} else if (!strcmp(argv[optind], "list")) {
			return 0;
		} else {
			printf("optind: %d, argv: %s\n", optind, argv[optind]);
			printf("Error: Unrecognized argument.\n");
			return 1;
		}
	}

	if (addflag) {
		addcron(&starttime, &endtime);
		printf("Wifi set to turn off at %d:%02d\n", starttime.hour, starttime.minute);
		printf("Wifi set to turn on at %d:%02d\n", endtime.hour, endtime.minute);
	} else if (rmvflag) {
		rmvcron(&starttime, &endtime);
		printf("Timer removed. Start: %d:%02d End: %d:%02d\n", starttime.hour, starttime.minute, endtime.hour, endtime.minute);
	} 

	return 0;
}

// Function for creating cronjob
int addcron(struct hourmin *start_time, struct hourmin *end_time) {
	char cron_wifioff[1000];
	char cron_wifion[1000];

	// Is there a cleaner way of preparing these commands?
	sprintf(cron_wifioff, "(crontab -l ; echo '%d %d * * * /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifioff >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", start_time->minute, start_time->hour);
	sprintf(cron_wifion, "(crontab -l ; echo '%d %d * * * /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifion >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", end_time->minute, end_time->hour);

	system(cron_wifioff);
	system(cron_wifion);
	return 0;
}

// Function for removing crons related to ./a.out wifioff
int rmvcron(struct hourmin *start_time, struct hourmin *end_time) {
	// rcron for remove cron
	char rcron_wifioff[1000];
	char rcron_wifion[1000];

	// Is there a cleaner way of preparing these commands?
	sprintf(rcron_wifioff, "(crontab -l | sort - | uniq - | grep -v '%d %d \\* \\* \\* /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifioff >>/dev/null 2>>/dev/null') | crontab -", start_time->minute, start_time->hour);
	sprintf(rcron_wifion, "(crontab -l | sort - | uniq - | grep -v '%d %d \\* \\* \\* /Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifion >>/dev/null 2>>/dev/null') | crontab -", end_time->minute, end_time->hour);

	system(rcron_wifioff);
	system(rcron_wifion);
	return 0;
}

int clearcron(void) {
	system("crontab -r");
	system("/Users/mimmyjau/Projects/wifi-scheduler/setcheck.out wifion >>/dev/null 2>>/dev/null");
	return 0;
}

// Function converts string with 'HHMM' format into struct hourmin with 
// members int minute and int hour 
int gethourmin(char *strtime, struct hourmin *strhourmin) {
	// Arrays are size 3 because HH and MM can be at most 2 digits + '\0'
	char hour[3] = { '0', '\0', '\0' };
	char minute[3] = { '0', '\0', '\0' }; 

	// Takes 'HHMM' string and separates into minutes and hours strings
	switch (strlen(strtime)) {
	case 1: case 2:
		strcpy(hour, strtime);
		break;
	case 3:
		hour[0] = *strtime++;
		hour[1] = '\0';
		strcpy(minute, strtime);
		break;
	case 4:
		hour[0] = *strtime++;
		hour[1] = *strtime++;
		hour[2] = '\0';
		strcpy(minute, strtime);
		break;
	default:
		printf("Error: Bad time arguments. Must be of the form H, HH, HMM, or HHMM.\n");
		exit(1);
	}

	strhourmin->hour = (int) strtol(hour, NULL, 10);
	strhourmin->minute = (int) strtol(minute, NULL, 10);

	// Checks that mins and hours are legal values. First errno occurs if
	// either strtol() above fail (if str can't be converted to number).
	if (errno) {
		printf("Error: Bad time argument. Must provide a number.\n");
		printf("errno: %s\n", strerror(errno));
		exit(1);
	}
	if (strhourmin->hour > 23 || strhourmin->hour < 0) {
		printf("Error: Bad time argument. Hour must be between 0-23.\n");
		exit(1);
	} 
	if (strhourmin->minute > 59 || strhourmin->minute <0) {
		printf("Error: Bad time argument. Minute must be between 0-59.\n");
		exit(1);
	}

	return 0;
}
