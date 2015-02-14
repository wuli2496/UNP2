#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>

int main(int argc, char **argv)
{
	int c, oflag, mqid;
	oflag = 0666 | IPC_CREAT;
	
	/*	
	while ((c = getopt(argc, argv, "e")) != -1) {
		switch (c) {
		case 'e':
			oflag |= IPC_EXCL;
			break;
		}
	}

	if (optind != argc - 1) {
		fprintf(stderr, "usage exename [-e] <pathname>\n");
		exit(-1);
	}
	*/

	
	mqid = msgget(ftok(argv[1], 0), oflag);
	if (mqid < 0) {
		fprintf(stderr, "msgget error:%s\n", strerror(errno));
	}
	exit(0);
}
