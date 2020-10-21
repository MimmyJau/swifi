#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
	// NTD: Cleaner way of specifying directory?
	char *checkon_path = "/Users/mimmyjau/Projects/wifi-scheduler/checkon";
	char *checkon_arg0 = "checkon";

	int pid = fork();
	if (pid == 0) {
		exit(0);
	} else {
		execl(checkon_path, checkon_arg0, (char *) NULL);
		if (errno) {
			printf("errno %d: %s\n", errno, strerror(errno));
		}
	}
	return 0;
}

