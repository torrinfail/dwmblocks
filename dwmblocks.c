#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<X11/Xlib.h>

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	int signal;
} Block;

#include "blocks.h"
#include "buffer.h"

#define STRLEN(X)    (sizeof(X) - 1)
#define CMDLENGTH    50
#define STATUSLENGTH 256
// make sure there is enough space for at least one block
static_assert(STATUSLENGTH >= CMDLENGTH + STRLEN(left_delim) + STRLEN(right_delim));

typedef BUFFER(char, CMDLENGTH) CommandBuffer;
typedef BUFFER(char, STATUSLENGTH + 1) StatusBuffer;

static void sighandler(int num);
static void getcmds(int time);
#ifndef __OpenBSD__
static void getsigcmds(int signal);
static void setupsignals();
static void sighandler(int signum);
static void refreshhandler(int signum);
#endif
static int getstatus();
static void setroot();
static void statusloop();
static void termhandler(int signum);

static Display *dpy;
static int screen;
static Window root;
static CommandBuffer statusbar[LENGTH(blocks)];
static StatusBuffer statusstrs[2];
static StatusBuffer* statusstr = &statusstrs[0];
static int statusContinue = 1;
static void (*writestatus) () = setroot;

//opens process *block->command and stores output in *output
void getcmd(const Block *block, CommandBuffer* output)
{
	size_t icon_length = buffer_copy_str(output, block->icon)->count;

	FILE *cmdf = popen(block->command, "r");

	if (!cmdf)
	{ return; }

	buffer_read_append(output, cmdf);

	pclose(cmdf);

	if(output->count > 0 && output->data[output->count - 1] == '\n')
	{ --output->count; }

	if(output->count == icon_length)
	{
		output->count = 0;
		return;
	}
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

void refreshhandler(int signum)
{
	(void)signum;
	getcmds(-1);
	writestatus();
}

void setupsignals()
{
	for(size_t i = 0; i < LENGTH(blocks); i++)
	{
		if (blocks[i].signal > 0)
			signal(SIGRTMIN+blocks[i].signal, sighandler);
	}

	signal(SIGRTMIN, refreshhandler);
}
#endif

int getstatus()
{
	// change the active status buffer
	statusstr = statusstr == &statusstrs[0] ? &statusstrs[1] : &statusstrs[0];

	buffer_copy_chrarr(statusstr, left_delim);

	size_t i = 0;

	// find the first non-empty block
	for(; i < LENGTH(blocks) && statusbar[i].count == 0; ++i);

	if(i < LENGTH(blocks))
	{ buffer_append(statusstr, &statusbar[i++]); }

	for(; i < LENGTH(blocks); ++i)
	{
		if(statusstr->count + statusbar[i].count + STRLEN(delim) + STRLEN(right_delim) > STATUSLENGTH)
		{
			buffer_copy_chrarr(statusstr, "error: status is too long to be stored in the buffer");
			goto done;
		}

		if(statusbar[i].count > 0)
		{
			buffer_append_chrarr(statusstr, delim);
			buffer_append(statusstr, &statusbar[i]);
		}
	}

	buffer_append_chrarr(statusstr, right_delim);

done:
	return buffer_eq(&statusstrs[0], &statusstrs[1]);
}

void setroot()
{
	if (getstatus())//Only set root if text has changed.
		return;

	statusstr->data[statusstr->count] = '\0';

	Display *d = XOpenDisplay(NULL);
	if (d) {
		dpy = d;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	XStoreName(dpy, root, statusstr->data);
	XCloseDisplay(dpy);
}

void pstdout()
{
	if (getstatus())//Only write out if text has changed.
		return;

	buffer_write(statusstr, stdout);
	putchar('\n');
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
		if(!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
}
