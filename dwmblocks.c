#include "dwmblocks.h"

//opens process *cmd and stores output in *output
void getcmd(const Block *block, char *output) {
	strcpy(output, block->icon);
	FILE *cmdBlock = popen(block->command, "r");
	if (!cmdBlock)
		return;
	int charOffset = strlen(block->icon);
	//Merge icon and command output
	fgets(output+charOffset , CMDLENGTH-charOffset -delimLen, cmdBlock);
	charOffset = strlen(output);
	if (charOffset == 0) {
		//return if block and command output are both empty
	}else {
		//only chop off newline if one is present at the end
		charOffset = output[charOffset-1] == '\n' ? charOffset-1 : charOffset;
		if (delim[0] != '\0') {
		//add delim to output
		strncpy(output+charOffset, delim, delimLen); 
		} else//no delimeter
			output[charOffset++] = '\0';
	}
	pclose(cmdBlock);
	return;
}

void getcmds(int time) {
	const Block* current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current,statusbar[i]);
	}
}

void getsigcmds(unsigned int signal) {
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current,statusbar[i]);
	}
}

void setupsignals() {
#ifndef __OpenBSD__
	    /* initialize all real time signals with dummy handler */
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
        signal(i, dummysighandler);
#endif

	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].signal > 0)
			signal(SIGMINUS+blocks[i].signal, sighandler);
	}
}

int getstatus(char *str, char *last) {
	strcpy(last, str);
	str[0] = '\0';
	for (unsigned int i = 0; i < LENGTH(blocks); i++)
		strcat(str, statusbar[i]);
	str[strlen(str)-strlen(delim)] = '\0';
	return strcmp(str, last);//0 if they are the same
}

#ifndef NO_X
void setroot() {
	if (!getstatus(statusstr[0], statusstr[1]))//Only set root if text has changed.
		return;
	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX() {
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout() {
	if (!getstatus(statusstr[0], statusstr[1]))//Only write out if text has changed.
		return;
	printf("%s\n",statusstr[0]);
	fflush(stdout);
}

void statusloop() {
	setupsignals();
	int time = 0;//69 year uptime not possible to overflow 
	getcmds(-1);
	while (1) {
		getcmds(time++);
		writestatus();//update status bar
		if (!statusContinue)
			break;
		sleep(1.0);//wait 1 sec and control again
	}
}

void sighandler(int signum) {
	getsigcmds(signum-SIGPLUS);
	writestatus();
}

void termhandler() {
	statusContinue = 0;
}

int main(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {//Handle command line arguments
		if (!strcmp("-d",argv[i]))
			strncpy(delim, argv[++i], delimLen);
		else if (!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
#ifndef NO_X
	if (!setupX())//Failed to open display
		return 1;
#endif
	delim[delimLen-1] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
#ifndef NO_X
	XCloseDisplay(dpy);
#endif
	return 0;
}
