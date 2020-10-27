#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#ifndef NO_X
#include<X11/Xlib.h>
#endif
#ifdef __OpenBSD__
#define SIGPLUS			SIGUSR1+1
#define SIGMINUS		SIGUSR1-1
#else
#define SIGPLUS			SIGRTMIN
#define SIGMINUS		SIGRTMIN
#endif
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		50
#define MIN( a, b ) ( ( a < b) ? a : b )
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
	int calledBySignal; //Keep track if "block is called by signal"
} Block;
#ifndef __OpenBSD__
void dummysighandler(int num);
#endif
void sighandler(int num);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getblockstatus(char *str, char *last);
void setroot(int i);
void statusloop();
void termhandler();
void pstdout();
#ifndef NO_X
static void (*writestatus) (int i) = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) (int i) = pstdout;
#endif


#include "blocks.h"

static char statusbar[2][LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[STATUSLENGTH];
static int statusContinue = 1;
static int returnStatus = 0;
static pthread_t threadId;
static pthread_attr_t attr;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Global mutex

//opens process *cmd, call writestatus and loop current block
void *blockLoop(void *currentBlock)
{
	Block *block = (Block*) currentBlock;
	int blockNum = block - blocks;

	//Use temporary string so output doesn't get blank for other threads
	char str[255] = {0};
	strcpy(str, block->icon);

	//Loop until program is to be killed (may be un-necessary)
	while (statusContinue) {
		FILE *cmdf = popen(block->command, "r");
		if (!cmdf)
			goto ENDLOOP;
		int i = strlen(block->icon);
		fgets(str+i, CMDLENGTH-i-delimLen, cmdf);
		i = strlen(str);
		if (i == 0)//return if block and command output are both empty
			goto ENDLOOP;
		if (delim[0] != '\0') {
			//only chop off newline if one is present at the end
			i = str[i-1] == '\n' ? --i : i;
			strncpy(str+i, delim, delimLen); 
		}
		else
			str[i++] = '\0';
		pclose(cmdf);

		strcpy(statusbar[0][blockNum], str);

		//Only allow one block at the time to use writestatus
		pthread_mutex_lock(&mutex);
		writestatus(blockNum);
		pthread_mutex_unlock(&mutex);

ENDLOOP:
		//Kill thread if created by signal
		if ( block->calledBySignal){
			block->calledBySignal = 0;
			break;
		}

		//Wait block specific interval, and call function again
		sleep(block->interval);

	}
	pthread_exit(NULL);
}

void getsigcmds(unsigned int signal)
{
	Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		//Ignore signals if thread called by signal is already running, this seems to prevent other blocks from freezing when signal is spammed
		if (current->signal && !current->calledBySignal){
			current->calledBySignal = 1;
			pthread_create(&threadId, &attr, blockLoop, (void*) current);
		}
	}
}

void setupsignals()
{
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

int getblockstatus(char *str, char *last)
{
	if (strcmp(str, last)) { //0 if they are the same
		strcpy(last, str);
		statusstr[0] = '\0';
		for ( int i = 0; i < LENGTH(blocks); i++)
			strcat(statusstr, statusbar[0][i]);

		statusstr[strlen(statusstr)-strlen(delim)] = '\0';
		return 0;
	}
	return -1;
}

#ifndef NO_X
void setroot(int i)
{
	//Only compare single "block" of text
	if (getblockstatus(statusbar[0][i], statusbar[1][i]))//Only set root if text has changed.
		return;
	XStoreName(dpy, root, statusstr);
	XFlush(dpy);
}

int setupX()
{
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

void pstdout(int i)
{
	if (getblockstatus(statusbar[0][i], statusbar[1][i]))///Only write out if text has changed.
		return;
	printf("%s\n",statusstr);
	fflush(stdout);
}


void statusloop()
{
	setupsignals();
	//Start block threads
	for (int i = 0; i < LENGTH(blocks); i++) {
		Block *current = blocks+i;
		pthread_create(&threadId, &attr, blockLoop, (void*) current);
	}
	//Keep main process running
	while (statusContinue) {
		sleep(1.0);
	}
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void dummysighandler(int signum)
{
    return;
}
#endif

void sighandler(int signum)
{
	getsigcmds(signum-SIGPLUS);
}

void termhandler()
{
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for (int i = 0; i < argc; i++) {//Handle command line arguments
		if (!strcmp("-d",argv[i]))
			strncpy(delim, argv[++i], delimLen);
		else if (!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
#ifndef NO_X
	if (!setupX())
		return 1;
#endif
	delimLen = MIN(delimLen, strlen(delim));
	delim[delimLen++] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);

	//Create attrib for creating thread, so threads won't need to get attached
	pthread_attr_init(&attr);	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	statusloop();
#ifndef NO_X
	XCloseDisplay(dpy);
#endif
	return 0;
}
