#include <stdio.h>	// For printf()
#include <stdlib.h>	// For system()
#include <string.h>	// For strcmp()
#include <inttypes.h>	// For strtol()
#include <unistd.h>	// For getopt() and getcwd()
#include <errno.h>	// For errno
#include <time.h>	// For time(), ctime(), etc.

// Determine os
#if __APPLE__
	#define PLATFORM_NAME "apple"
#elif __linux__
	#define PLATFORM_NAME "linux"
#endif
#define MAXLINE 1000

struct hourmin {
	int minute;
	int hour;
};

// Need static keyword to avoid -Wmissing-variable-declaration warning
static char cwd[MAXLINE];
static char os[MAXLINE];
static int addflag, rmvflag;
// Default: Be on the whole day
static struct hourmin starttime = { 1, 0 };
static struct hourmin endtime = { 0, 0 };

void getarg(int argc, char **argv);
int addcron(struct hourmin *starttime, struct hourmin *endtime);
int rmvcron(struct hourmin *starttime, struct hourmin *endtime);
int clearcron(void);
int turnoffwifi(void);
int turnonwifi(void);
int gethourmin(char *strtime, struct hourmin *);
int istimebetween(void);

// Takes 3 arguments: 1) add or rmv, 2) start time (-s) and 3) end time (-e)
int main(int argc, char *argv[]) {
	getcwd(cwd, MAXLINE);
	strcpy(os, PLATFORM_NAME);

	if (argc == 1) {
		printf("Error: Provide at least one argument 'add' or 'rmv'\n");
		return 1;
	}

	getarg(argc, argv);

	if (addflag) {
		addcron(&starttime, &endtime);
		if(istimebetween())
			turnoffwifi();
		printf("Wifi set to turn off at %d:%02d\n", starttime.hour, starttime.minute);
		printf("Wifi set to turn on at %d:%02d\n", endtime.hour, endtime.minute);
	} else if (rmvflag) {
		rmvcron(&starttime, &endtime);
		if(istimebetween())
			turnonwifi();
		printf("Timer removed. Start: %d:%02d End: %d:%02d\n", starttime.hour, starttime.minute, endtime.hour, endtime.minute);
	} 

	return 0;
}

// Parse arguments using getopt(). Need a separate function 
// to handle GNU / Linux case and OSX / BSD case.
void getarg(int argc, char **argv) {
	int ch;		// getopt man page uses "ch"
	addflag = rmvflag = 0;

	if (!strcmp(os, "apple")) {
		while (optind < argc) {
			if ((ch = getopt(argc, argv, "s:e:")) != -1) {
				switch (ch) {
				case 's':
					gethourmin(optarg, &starttime);
					break;
				case 'e':
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
				clearcron();
				optind++;
			} else {
				printf("Error: Unrecognized argument.\n");
				exit(1);
			}
		}
	} else if (!strcmp(os, "linux")) {
		while ((ch = getopt(argc, argv, "s:e:")) != -1) {
			switch (ch) {
			case 's':
				gethourmin(optarg, &starttime);
				break;
			case 'e':
				gethourmin(optarg, &endtime);
				break;
			default:
				break;
			}
		}
		int index;
		for (index = optind; index < argc; index++) {
			if (!strcmp(argv[optind], "add")) {
				addflag = 1;
			} else if (!strcmp(argv[optind], "rmv")) {
				rmvflag = 1;
			} else if (!strcmp(argv[optind], "clear")) {
				clearcron();
			} else {
				printf("Error: Unrecognized argument.\n");
				exit(1);
			}
		}

	}
}

// Adds cronjob to crontab
int addcron(struct hourmin *start_time, struct hourmin *end_time) {
	char cron_wifioff[MAXLINE];
	char cron_wifion[MAXLINE];

	// NTD: Is there a cleaner way of preparing these commands?
	sprintf(cron_wifioff, "(crontab -l 2>>/dev/null; echo '%d %d * * * %s/setcheck.out wifioff >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", start_time->minute, start_time->hour, cwd);
	sprintf(cron_wifion, "(crontab -l 2>>/dev/null; echo '%d %d * * * %s/setcheck.out wifion >>/dev/null 2>>/dev/null') | sort - | uniq - | crontab -", end_time->minute, end_time->hour, cwd);

	system(cron_wifioff);
	system(cron_wifion);
	return 0;
}

// Removes cronjobs from crontab
int rmvcron(struct hourmin *start_time, struct hourmin *end_time) {
	// rcron stands for remove cron
	char rcron_wifioff[MAXLINE];
	char rcron_wifion[MAXLINE];

	// NTD: Is there a cleaner way of preparing these commands?
	sprintf(rcron_wifioff, "(crontab -l | sort - | uniq - | grep -v '%d %d \\* \\* \\* %s/setcheck.out wifioff >>/dev/null 2>>/dev/null') | crontab -", start_time->minute, start_time->hour, cwd);
	sprintf(rcron_wifion, "(crontab -l | sort - | uniq - | grep -v '%d %d \\* \\* \\* %s/setcheck.out wifion >>/dev/null 2>>/dev/null') | crontab -", end_time->minute, end_time->hour, cwd);

	system(rcron_wifioff);
	system(rcron_wifion);
	return 0;
}

// Clears crontab and restarts wifi
int clearcron(void) {
	system("crontab -r");
	turnonwifi();
	return 0;
}

int turnoffwifi(void) {
	char wificmd[MAXLINE];
	sprintf(wificmd, "%s/setcheck.out wifioff >>/dev/null 2>>/dev/null", cwd);
	system(wificmd);
	return 0;
}

int turnonwifi(void) {
	char wificmd[MAXLINE];
	sprintf(wificmd, "%s/setcheck.out wifion >>/dev/null 2>>/dev/null", cwd);
	system(wificmd);
	return 0;
}

// Get current time and compare if it is in between
// NTD: Not sure what to do if starttime = endtime. cronjob race condition?
int istimebetween(void) {
	// Get current time into int format (tm struc that contains int members) 
	time_t current_time = time(NULL);
	struct tm *current_tm = localtime(&current_time);

	// Rename for ease
	int chour = current_tm->tm_hour;
	int cmin = current_tm->tm_min;
	int shour = starttime.hour;
	int smin = starttime.minute;
	int ehour = endtime.hour;
	int emin = endtime.minute;

	// If endtime > starttime, check if current time is in between
	// If endtime < starttime, check if current time is NOT in between
	int checkbetween = 0;
	if ((ehour > shour) || (ehour == shour && emin > smin))
		checkbetween = 1;

	// Compare current time to starttime and endtime
	int isbetween = 0;
	switch (checkbetween) {
	case 0:
		// In case 0, endtime is "earlier" than starttime, so 
		// we check endtime < currenttime < starttime. 4 possibilities:
		// 1) endhour < currenthour < starthour
		// 2) endhour = currenthour < starthour; endmin < currentmin
		// 3) endhour < currenthour = starthour; currentmin < startmin
		// 4) endhour = currenthour = starthour; endmin < cmin < startmin
		if ((chour > ehour && chour < shour)
			|| (chour == ehour && cmin > emin && chour < shour)
			|| (chour == shour && cmin < smin && chour > ehour)
			|| (chour == ehour && chour == shour 
				&& cmin > emin && cmin < smin))
			isbetween = 1;
		break;
	case 1:
		// In case 1, end time is "later" than starttime, so
		// we check starttime < currenttime < endtime. Inverse of above.
		if ((chour > shour && chour < ehour)
			|| (chour == shour && cmin > smin && chour < ehour)
			|| (chour == ehour && cmin < emin && chour > shour)
			|| (chour == shour && chour == ehour 
				&& cmin > smin && cmin < emin))
			isbetween = 1;
		break;
	}

	return (checkbetween == isbetween);
}

// Converts string with 'HHMM' format into struct hourmin 
int gethourmin(char *strtime, struct hourmin *hmtime) {
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

	hmtime->hour = (int) strtol(hour, NULL, 10);
	hmtime->minute = (int) strtol(minute, NULL, 10);

	// Checks that mins and hours are legal values. First errno occurs if
	// either strtol() above fail (if str can't be converted to number).
	if (errno) {
		printf("Error: Bad time argument. Must provide a number.\n");
		printf("errno: %s\n", strerror(errno));
		exit(1);
	}
	if (hmtime->hour > 23 || hmtime->hour < 0) {
		printf("Error: Bad time argument. Hour must be between 0-23.\n");
		exit(1);
	} 
	if (hmtime->minute > 59 || hmtime->minute <0) {
		printf("Error: Bad time argument. Minute must be between 0-59.\n");
		exit(1);
	}

	return 0;
}
