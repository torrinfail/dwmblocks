#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

#ifndef NO_X
#include<X11/Xlib.h>
#endif

#ifdef __OpenBSD__
#define SIGPLUS		SIGUSR1+1
#define SIGMINUS	SIGUSR1-1
#else
#define SIGPLUS		SIGRTMIN
#define SIGMINUS	SIGRTMIN
#endif

#define LENGTH(X)	(sizeof(X) / sizeof (X[0]))
#define CMDLENGTH	 50
#define MIN( a, b ) ( ( a < b) ? a : b )
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + 1)

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void dummysighandler(int signum) { return; }
#endif

#ifndef NO_X
void setroot();
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif

void sighandler(int num);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;

#include "blocks.h"
static char statusbar[LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[2][STATUSLENGTH];
static int statusContinue = 1;
static int returnStatus = 0;
