#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<X11/Xlib.h>
#include<pthread.h>
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
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getblockstatus(char *str, char *last);
void setroot(int i);
void statusloop();
void termhandler();


#include "blocks.h"

static Display *dpy;
static int screen;
static Window root;
static char statusbar[2][LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[STATUSLENGTH];
static int statusContinue = 1;
static void (*writestatus) (int i) = setroot;
static pthread_t threadId;
static pthread_attr_t attr;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Global mutex
static pthread_mutex_t threadMutex[LENGTH(blocks)] = {PTHREAD_MUTEX_INITIALIZER}; //Block spefific mutexes

//opens process *cmd, call writestatus and loop current block
void *getcmd(void *currentBlock)
{
	Block *block = (Block*) currentBlock;
	int blockNum = block - blocks;

	//Use temporary string so output doesn't get blank for other threads
	char str[255] = {0};
	strcpy(str, block->icon);
	FILE *cmdf = popen(block->command, "r");
	if (!cmdf)
		goto ENDTHREAD;
	int i = strlen(block->icon);
	fgets(str+i, CMDLENGTH-i-delimLen, cmdf);
	i = strlen(str);
	if (i == 0)//return if block and command output are both empty
		goto ENDTHREAD;
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
	
ENDTHREAD:
	//Don't create more threads when called as signal
	if ( block->calledBySignal)
	{
			block->calledBySignal = 0;
			pthread_mutex_unlock(&threadMutex[blockNum]);
			pthread_exit(NULL);
	}

	//Unlock mutex
	pthread_mutex_unlock(&threadMutex[blockNum]);

	//Wait block specific interval, and call function again
	sleep(block->interval);
	
	//Allow only one thread per block at once
	pthread_mutex_lock(&threadMutex[blockNum]);
	pthread_create(&threadId, &attr, getcmd, (void*) block);
	
	
	pthread_exit(NULL);
}

void getsigcmds(unsigned int signal)
{
	Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		//Ignore signals if thread called by signal is already running, this seems to prevent other blocks from freezing when signal is spammed
		if (current->signal && !current->calledBySignal)
		{
			//Allow only one thread per command at once(Not any real reason)
			pthread_mutex_lock(&threadMutex[i]);
			current->calledBySignal = 1;
			pthread_create(&threadId, &attr, getcmd, (void*) current);
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

	for (unsigned int i = 0; i < LENGTH(blocks); i++)
	{
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
		{
			strcat(statusstr, statusbar[0][i]);
		}
		statusstr[strlen(statusstr)-strlen(delim)] = '\0';
		return 0;
	}
	return -1;
}

void setroot(int i)
{
	//Only compare single "block" of text
	if (getblockstatus(statusbar[0][i], statusbar[1][i]))//Only set root if text has changed.
		return;
	Display *d = XOpenDisplay(NULL);
	if (!d)
		return;
	dpy = d;
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	XStoreName(dpy, root, statusstr);
	XCloseDisplay(dpy);
}

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
	for (int i = 0; i < LENGTH(blocks); i++)
	{
		const Block *current = blocks+i;
		//Only one thread per block
		pthread_mutex_lock(&threadMutex[i]);
		pthread_create(&threadId, &attr, getcmd, (void*) current);
	}
	//Keep main process running
	while (statusContinue)
	{
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
	for (int i = 0; i < argc; i++) //Handle command line arguments
	{
		if (!strcmp("-d",argv[i]))
			strncpy(delim, argv[++i], delimLen);
		else if (!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
	delimLen = MIN(delimLen, strlen(delim));
	delim[delimLen++] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);

	//Create attrib for creating thread, so threads won't need to get attached
	pthread_attr_init(&attr);	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	statusloop();
}
