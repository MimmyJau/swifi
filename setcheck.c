#include <stdio.h>	// For strerror()
#include <stdlib.h>	// For exit()
#include <string.h>	// For strcmp()
#include <unistd.h>	// For fork()
#include <errno.h>	// For errno
#include <time.h>	// For time()

int startcheck(void);
int endcheck(void);
int pmessages(char *);

// Cron calls main(), telling it to either turn on wifi (normal state)
// or turn off wifi (run script)
int main(int argc, char *argv[]) {
        if (argc == 1) {
                printf("Include argument 'wifion' or 'wifioff'\n");
                return 1;
        }

        // Turn off or on the background script
        if (!strcmp(argv[1], "wifion")) {
		// Run program that sends signal to checkon and shuts it down
		if (endcheck())
			return 1;
		pmessages("Turning on wifi");
		// Inclue full path b/c cron sets a different environment 
		// than shell, meaning PATH variable is different
		system("/usr/sbin/networksetup -setairportpower en0 on");
        } else if (!strcmp(argv[1], "wifioff")) {
		pmessages("Turning off wifi"); 
		system("/usr/sbin/networksetup -setairportpower en0 off");
		if(startcheck())
			return 1;
        } else {
		pmessages("Error: Argument must be 'wifion' or 'wifioff': %s\n");
	}
        return 0;
}

// Launch checkon script
int startcheck(void) {
	char *checkon_path = "/Users/mimmyjau/Projects/wifi-scheduler/checkon.sh";
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
