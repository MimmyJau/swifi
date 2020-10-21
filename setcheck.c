#include <stdio.h>	// For strerror()
#include <stdlib.h>	// For exit()
#include <string.h>	// For strcmp()
#include <unistd.h>	// For fork()
#include <errno.h>	// For errno

int startcheck(void);
int endcheck(void);

// Cron calls main(), telling it to either turn on wifi (normal state)
// or turn off wifi (run script)
int main(int argc, char *argv[]) {
        if (argc == 1) {
                printf("Include argument 'wifion' or 'wifioff'\n");
                return 1;
        }

        // Turn off or on the checkon script
        if (!strcmp(argv[1], "wifion")) {
                printf("Turning on wifi\n");
		// Run program that sends signal to checkon and shuts it down
		if (endcheck())
			return 1;
		system("networksetup -setairportpower en0 on");
        } else if (!strcmp(argv[1], "wifioff")) {
                printf("Turning off wifi\n");
		system("networksetup -setairportpower en0 off");
		if(startcheck())
			return 1;
        } 
        return 0;
}

// Launch checkon script
int startcheck(void) {
	// char *checkon_path = "/Users/mimmyjau/Projects/wifi-scheduler/checkon";
	char *checkon_path = "/Users/mimmyjau/Projects/wifi-scheduler/checkon";
	char *checkon_arg0 = "checkon";

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
	char *killcommand = "kill $(pgrep -f '/bin/bash /Users/mimmyjau/Projects/wifi-scheduler/checkon')";
	// pgrep -f finds name of script and kill will end it
	system(killcommand);
	return 0;
}
