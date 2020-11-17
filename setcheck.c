#include <stdio.h>	// For strerror()
#include <stdlib.h>	// For exit() and system()
#include <string.h>	// For strcmp()
#include <unistd.h>	// For fork(), getcwd(), access()
#include <signal.h>	// For kill()
#include <errno.h>	// For errno
#include <time.h>	// For time()

// Determine os
#if __APPLE__
	#define PLATFORM_NAME "apple"
#elif __linux__
	#define PLATFORM_NAME "linux"
#endif

#define MAXLINE 1000

// 1) If I don't assign macro to a variable, compiler will warn that code
// strcmp(PLATFORM_NAME, "apple") will result in unreachable code. 
// Need to determine condition at run-time vs compile-time.
// 2) Need `static` to avoid -Wmissing-variable-declaration warning
static char os[MAXLINE];

int startcheck(void);
int endcheck(void);
int pmessages(char *message);
int turnonwifi(void);
int turnoffwifi(void);

// Cron calls main(), telling it to either turn on wifi (normal state)
// or turn off wifi (run script)
int main(int argc, char *argv[]) {
	strcpy(os, PLATFORM_NAME);
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
	char cwd[MAXLINE];
	getcwd(cwd, MAXLINE);

	char *checkon_path2 = "/checkon.sh";
	char *checkon_path = strcat(cwd, checkon_path2);
	char *checkon_arg0 = "checkon.sh";

	// Check if checkon.sh already exists, by checking for pid_sh.txt file
	// that script creates, and if it exists, print error and exit
	int isrunning = 1;
	char fpath[MAXLINE];
	getcwd(fpath, MAXLINE);
	strcat(fpath, "/pid_sh.txt");
	// F_OK checks if file exists and returns 0 if true
	isrunning = access(fpath, F_OK);
	if(isrunning == 0) {
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
	// 1) Get pathname to file with PID of shell script
	char fpath[MAXLINE];
	getcwd(fpath, MAXLINE);
	strcat(fpath, "/pid_sh.txt");
	// 2) Open text file with PID and read number
	FILE *fpid = fopen(fpath, "r");
	if (fpid == NULL) {
		pmessages("Error: checkon.sh not running");
		exit(1);
	}
	int pid_sh;
	fscanf(fpid, "%d", &pid_sh);
	fclose(fpid);
	// 3) Kill shell script using PID
	int killret = kill(pid_sh, SIGTERM);
	if (killret != 0) {
		pmessages("Error: Could not send kill signal to checkon.sh");
		exit(1);
	}
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
	// Include full path b/c cron sets a different environment 
	// than shell, meaning PATH variable is different
	if (!strcmp(os, "apple")) {
		system("/usr/sbin/networksetup -setairportpower en0 on");
	} else if (!strcmp(os, "linux")) {
		system("nmcli networking on");
	}
	return 0;
}

// Turn off wifi
int turnoffwifi() {
	// Include full path b/c cron sets a different environment 
	// than shell, meaning PATH variable is different
	if (!strcmp(os, "apple")) {
		system("/usr/sbin/networksetup -setairportpower en0 off");
	} else if (!strcmp(os, "linux")) {
		system("nmcli networking off");
	}
	return 0;
}
