#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<X11/Xlib.h>
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		50

typedef struct {
	const char *icon;
	const char *command;
	const unsigned int interval;
	const int signal;
} Block;
void sighandler(int num);
void replace(char *str, char oldc, char newc);
void getcmds(int time);
#ifndef __OpenBSD__
void getsigcmds(int signal);
void setupsignals();
void sighandler(int signum);
#endif
int getstatus(char *str, char *last);
void setroot();
void statusloop();
void termhandler(int signum);


#include "blocks.h"

static Display *dpy;
static int screen;
static Window root;
static char statusbar[LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[2][256];
static int statusContinue = 1;
static void (*writestatus) () = setroot;

void replace(char *str, char oldc, char newc)
{
	int N = strlen(str);
	for(int i = 0; i < N; i++)
		if(str[i] == oldc)
			str[i] = newc;
}

// opens process *cmd and stores output in *output
void getcmd(const Block *block, char *output)
{
	strcpy(output, block->icon);
	const char *cmd = block->command;
	FILE *cmdf = popen(cmd, "r");
	if (!cmdf)
		return;

	int i = strlen(block->icon);
	fgets(output+i, CMDLENGTH-i, cmdf);
	i = strlen(output);

	// delete \n from cmd output
	output[i - 1] = '\0';
	pclose(cmdf);
}

void getcmds(int time)
{
	const Block *current;
	for(size_t i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current, statusbar[i]);
	}
}

#ifndef __OpenBSD__
void getsigcmds(int signal)
{
	const Block *current;
	for (size_t i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current, statusbar[i]);
	}
}

void setupsignals()
{
	for(size_t i = 0; i < LENGTH(blocks); i++)
		if (blocks[i].signal > 0)
			signal(SIGRTMIN + blocks[i].signal, sighandler);
}
#endif

int getstatus(char *str, char *last)
{
	strcpy(last, str);
	str[0] = '\0';
	for(size_t i = 0; i < LENGTH(blocks); i++) {
		strcat(str, statusbar[i]);
		if (i != LENGTH(blocks) - 1 && strlen(statusbar[i]))
			strcat(str, delim);
	}
	str[strlen(str)] = '\0';
	return strcmp(str, last); // 0 if they are the same
}

void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1])) // Only set root if text has changed.
		return;
	Display *d = XOpenDisplay(NULL);
	if (d)
		dpy = d;

	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	XStoreName(dpy, root, statusstr[0]);
	XCloseDisplay(dpy);
}

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1])) // Only write out if text has changed.
		return;
	printf("%s\n", statusstr[0]);
}


void statusloop()
{
#ifndef __OpenBSD__
	setupsignals();
#endif
	int i = 0;
	getcmds(-1);
	while(statusContinue)
	{
		getcmds(i);
		writestatus();
		sleep(1);
		i++;
	}
}

#ifndef __OpenBSD__
void sighandler(int signum)
{
	getsigcmds(signum-SIGRTMIN);
	writestatus();
}
#endif

void termhandler(int signum)
{
	statusContinue = 0;
	exit(0);
}

int main(int argc, char **argv)
{
	for(int i = 0; i < argc; i++)
		if (!strcmp("-d", argv[i]))
			delim = argv[++i];
		else if(!strcmp("-p", argv[i]))
			writestatus = pstdout;

	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
}
