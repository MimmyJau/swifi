#include <stdio.h>	// For strerror()
#include <stdlib.h>	// For exit()
#include <string.h>	// For strcmp()
#include <unistd.h>	// For fork()
#include <errno.h>	// For errno
#include <time.h>	// For time()

int startcheck(void);
int endcheck(void);

// Cron calls main(), telling it to either turn on wifi (normal state)
// or turn off wifi (run script)
int main(int argc, char *argv[]) {
	time_t current_time;
	char *c_time_string;
	
        if (argc == 1) {
                printf("Include argument 'wifion' or 'wifioff'\n");
                return 1;
        }

        // Turn off or on the background script
        if (!strcmp(argv[1], "wifion")) {
		current_time = time(NULL);
		c_time_string = ctime(&current_time);
                printf("Turning on wifi: %s\n", c_time_string);
		// Run program that sends signal to checkon and shuts it down
		if (endcheck())
			return 1;
		// Inclue full path b/c cron sets a different environment 
		// than shell, meaning PATH variable is different
		system("/usr/sbin/networksetup -setairportpower en0 on");
        } else if (!strcmp(argv[1], "wifioff")) {
		current_time = time(NULL);
		c_time_string = ctime(&current_time);
                printf("Turning off wifi: %s\n", c_time_string);
		system("/usr/sbin/networksetup -setairportpower en0 off");
		if(startcheck())
			return 1;
        } else {
		current_time = time(NULL);
		c_time_string = ctime(&current_time);
                printf("Error: Argument must be 'wifion' or 'wifioff': %s\n", c_time_string);
	}
        return 0;
}

// Launch checkon script
int startcheck(void) {
	char *checkon_path = "/Users/mimmyjau/Projects/wifi-scheduler/checkon.sh";
	char *checkon_arg0 = "checkon.sh";

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
	// char *killcommand = "kill $(pgrep -f checkon)";
	// pgrep -f finds name of script and kill will end it
	system("/bin/kill $(/usr/bin/pgrep -f checkon.sh)");
	return 0;
}
