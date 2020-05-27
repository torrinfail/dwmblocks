#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<X11/Xlib.h>

#include "buffer.h"

#define STRLEN(X) (sizeof(X) - 1)
#define LENGTH(X) (sizeof(X) / sizeof((X)[0]))

typedef struct
{
	char const*  icon;
	char const*  command;
	unsigned int interval;
	int          signal;
} Block;

#include "blocks.h"
static_assert(LENGTH(blocks) > 0, "there must be at least one block");

#define CMDLENGTH    50
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + STRLEN(left_delim) + STRLEN(right_delim) + (LENGTH(blocks) - 1) * STRLEN(delim))

typedef BUFFER(char, CMDLENGTH) CommandBuffer;
typedef BUFFER(char, STATUSLENGTH + 1) StatusBuffer;

static CommandBuffer statusbar[LENGTH(blocks)];
static StatusBuffer  statusstrs[2] = { { .count = 0 }, { .count = 0 } };
static StatusBuffer* statusstr = &statusstrs[0];

static int statusContinue = 1;

static void statusloop();
static int  getstatus();
static void setroot();
static void pstdout();
static void (*writestatus) () = setroot;
static void getcmd(Block const* block, CommandBuffer* output);
static void getcmds(int time);
#ifndef __OpenBSD__
static void getsigcmds(int signum);
static void setupsignals();
static void sighandler(int signum);
static void refreshhandler(int signum);
#endif
static void termhandler(int signum);

//opens process *block->command and stores output in *output
void getcmd(Block const* block, CommandBuffer* output)
{
	size_t icon_length = buffer_copy_str(output, block->icon)->count;

	FILE *cmdf = popen(block->command, "r");

	if(! cmdf)
	{ return; }

	buffer_read_append(output, cmdf);

	pclose(cmdf);

	if(output->count > 0 && output->data[output->count - 1] == '\n')
	{ --output->count; }

	if(output->count == icon_length)
	{ output->count = 0; }
}

void getcmds(int time)
{
	for(size_t i = 0; i < LENGTH(blocks); ++i)
	{	
		Block const* current = blocks + i;

		if((current->interval != 0 && time % current->interval == 0) || time == -1)
		{ getcmd(current, statusbar + i); }
	}
}

#ifndef __OpenBSD__
void getsigcmds(int signum)
{
	for(size_t i = 0; i < LENGTH(blocks); ++i)
	{
		Block const* current = blocks + i;

		if(current->signal == signum)
		{ getcmd(current, statusbar + i); }
	}
}

void refreshhandler(int signum)
{
	(void)signum;
	getcmds(-1);
	writestatus();
}

void sighandler(int signum)
{
	getsigcmds(signum-SIGRTMIN);
	writestatus();
}

void setupsignals()
{
	for(size_t i = 0; i < LENGTH(blocks); ++i)
	{
		if(blocks[i].signal > 0)
		{ signal(SIGRTMIN+blocks[i].signal, sighandler); }
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
		if(statusbar[i].count > 0)
		{
			buffer_append_chrarr(statusstr, delim);
			buffer_append(statusstr, &statusbar[i]);
		}
	}

	buffer_append_chrarr(statusstr, right_delim);

	return buffer_eq(&statusstrs[0], &statusstrs[1]);
}

void setroot()
{
	if(getstatus()) // Only write out if text has changed.
	{ return; }

	statusstr->data[statusstr->count] = '\0';

	Display* dpy    = XOpenDisplay(NULL);
	int      screen = DefaultScreen(dpy);
	Window   root   = RootWindow(dpy, screen);

	XStoreName(dpy, root, statusstr->data);
	XCloseDisplay(dpy);
}

void pstdout()
{
	if(getstatus()) // Only write out if text has changed.
	{ return; }

	buffer_write(statusstr, stdout);
	putchar('\n');
	fflush(stdout);
}

void statusloop()
{
	for(int i = -1; statusContinue; ++i)
	{
		getcmds(i);
		writestatus();
		sleep(1.0);
	}
}

void termhandler(int signum)
{
	(void)signum;
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for(int i = 0; i < argc; ++i)
	{	
		if(! strcmp("-p", argv[i]))
		{ writestatus = pstdout; }
	}

	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
#ifndef __OpenBSD__
	setupsignals();
#endif
	statusloop();
}
