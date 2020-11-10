#include <stdio.h>	// For strerror()
#include <stdlib.h>	// For exit() and system()
#include <string.h>	// For strcmp()
#include <unistd.h>	// For fork(), getcwd()
#include <errno.h>	// For errno
#include <time.h>	// For time()

// Determine os
#if __APPLE__
	#define PLATFORM_NAME "apple"
#elif __linux__
	#define PLATFORM_NAME "linux"
#endif

int startcheck(void);
int endcheck(void);
int pmessages(char *message);
int turnonwifi(void);
int turnoffwifi(void);

// Cron calls main(), telling it to either turn on wifi (normal state)
// or turn off wifi (run script)
int main(int argc, char *argv[]) {
        if (argc == 1) {
                printf("Include argument 'wifion' or 'wifioff'\n");
                return 1;
        }

        // Turn off or on the background script
        if (!strcmp(argv[1], "wifion")) {
		pmessages("Turning on wifi");
		turnonwifi();
		// Run program shuts down checkon script
		if (endcheck())
			return 1;
        } else if (!strcmp(argv[1], "wifioff")) {
		pmessages("Turning off wifi"); 
		turnoffwifi();
		if(startcheck())
			return 1;
        } else {
		pmessages("Error: Argument must be 'wifion' or 'wifioff': %s\n");
	}
        return 0;
}

// Launch checkon script
int startcheck(void) {
	char cwd[1000];
	getcwd(cwd, 1000);

	char *checkon_path2 = "/checkon.sh";
	char *checkon_path = strcat(cwd, checkon_path2);
	char *checkon_arg0 = "checkon.sh";

	// Check if checkon.sh already exists, if so, print error and exit
	if(system("/usr/bin/pgrep -f checkon.sh >>/dev/null 2>>/dev/null") == 0) {
		pmessages("Error: checkon.sh already exists");
		exit(1);
	}

	// Fork creates a child process that is identical except it returns
	// 0 for the child and the pid of the child for the parent process
	int pid = fork();
	// We fork so that child runs in background when parent exits
	if (pid == 0) {
		setsid();
		execl(checkon_path, checkon_arg0, (char *) NULL);
		if (errno) {
			printf("errno %d: %s\n", errno, strerror(errno));
			return 1;
		}
	} else {
		exit(0);
	}
	return 0;
}

// Find checkon script and kill it
int endcheck(void) {	
	// pgrep -f finds name of script and kill will end it
	// Redirects output otherwise if checkon.sh doesn't exist, 
	// command will print error to terminal
	system("/bin/kill $(/usr/bin/pgrep -f checkon.sh) >>/dev/null 2>>/dev/null");
	return 0;
}

// Print messages
int pmessages(char *pmessage) {
	time_t current_time = time(NULL);
	char *c_time_string = ctime(&current_time);

	if (pmessage != NULL) {
		printf("%s: %s", pmessage, c_time_string);
		return 0;
	} else {
		printf("pmessages error: no message: %s\n", c_time_string);
		return 1;
	}
}

// Turn on wifi
int turnonwifi() {
	// Inclue full path b/c cron sets a different environment 
	// than shell, meaning PATH variable is different
	if (!strcmp(PLATFORM_NAME, "apple")) {
		system("/usr/sbin/networksetup -setairportpower en0 on");
	} else if (!strcmp(PLATFORM_NAME, "linux")) {
		system("nmcli networking on");
	}
	return 0;
}

// Turn off wifi
int turnoffwifi() {
	// Inclue full path b/c cron sets a different environment 
	// than shell, meaning PATH variable is different
	if (!strcmp(PLATFORM_NAME, "apple")) {
		system("/usr/sbin/networksetup -setairportpower en0 off");
	} else if (!strcmp(PLATFORM_NAME, "linux")) {
		system("nmcli networking off");
	}
	return 0;
}
