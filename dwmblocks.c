#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<X11/Xlib.h>
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define STRLEN(X) (sizeof(X) - 1)
#define CMDLENGTH		50
#define STATUSLENGTH 256
#define MIN(X, Y) ({ typeof(X) __x = (X); typeof(Y) __y = (Y); __x > __y ? __y : __x; })

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	int signal;
} Block;
static void sighandler(int num);
static void getcmds(int time);
#ifndef __OpenBSD__
static void getsigcmds(int signal);
static void setupsignals();
static void sighandler(int signum);
#endif
static int getstatus();
static void setroot();
static void statusloop();
static void termhandler(int signum);

#include "blocks.h"
#include "buffer.h"

typedef BUFFER(CMDLENGTH) CommandBuffer;
typedef BUFFER(STATUSLENGTH) StatusBuffer;

static Display *dpy;
static int screen;
static Window root;
static CommandBuffer statusbar[LENGTH(blocks)];
static StatusBuffer statusstr;
static int statusContinue = 1;
static void (*writestatus) () = setroot;

static char* str_copy(char* dest, char const* src)
{
	while(*src != '\0')
	{
		*dest = *src;
		++dest;
		++src;
	}
	return dest;
}

//opens process *block->command and stores output in *output
void getcmd(const Block *block, CommandBuffer* output)
{
	output->count = str_copy(output->data, block->icon) - output->data;

	FILE *cmdf = popen(block->command, "r");

	if (!cmdf)
	{ return; }

	output->count += fread(output->data + output->count, 1, LENGTH(output->data) - output->count, cmdf);

	pclose(cmdf);

	if(output->count > 0 && output->data[output->count - 1] == '\n')
	{ --output->count; }
}

void getcmds(int time)
{
	const Block* current;
	for(size_t i = 0; i < LENGTH(blocks); i++)
	{	
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
		{ getcmd(current, statusbar + i); }
	}
}

#ifndef __OpenBSD__
void getsigcmds(int signal)
{
	const Block *current;
	for(size_t i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current, statusbar + i);
	}
}

void setupsignals()
{
	for(size_t i = 0; i < LENGTH(blocks); i++)
	{
		if (blocks[i].signal > 0)
			signal(SIGRTMIN+blocks[i].signal, sighandler);
	}

}
#endif

int getstatus()
{
	StatusBuffer old;
	buffer_copy(&old, &statusstr);

	buffer_copy(&statusstr, &statusbar[0]);

	for(size_t i = 1; i < LENGTH(blocks); ++i)
	{
		if(statusstr.count + statusbar[i].count + 1 > STRLEN(statusstr.data))
		{
			char error[] = "error: status is too long to be stored in the buffer";
			memcpy(statusstr.data, error, MIN(STRLEN(statusstr.data), STRLEN(error)));
			statusstr.count = MIN(STRLEN(statusstr.data), STRLEN(error));
			break;
		}

		if(delim != '\0' && statusbar[i].count > 0)
		{ statusstr.data[statusstr.count++] = delim; }

		buffer_append(&statusstr, &statusbar[i]);
	}

	statusstr.data[statusstr.count++] = '\0';

	return buffer_eq(&statusstr, &old);
}

void setroot()
{
	if (getstatus())//Only set root if text has changed.
		return;
	Display *d = XOpenDisplay(NULL);
	if (d) {
		dpy = d;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	XStoreName(dpy, root, statusstr.data);
	XCloseDisplay(dpy);
}

void pstdout()
{
	if (getstatus())//Only write out if text has changed.
		return;
	printf("%s\n", statusstr.data);
	fflush(stdout);
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
		sleep(1.0);
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
	(void)signum;
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for(int i = 0; i < argc; i++)
	{	
		if (!strcmp("-d",argv[i]))
			delim = argv[++i][0];
		else if(!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
}
