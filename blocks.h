// Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/* Icon */        /* Command */           /* Update Interval */   /* Update Signal */
	{" ", "~/.local/bin/desktop/statusbar/corona",     1800,         0},
	{" ", "~/.local/bin/desktop/statusbar/memory",       10,         0},
	{"",   "~/.local/bin/desktop/statusbar/cputemp",      10,         0},
	{"",   "~/.local/bin/desktop/statusbar/battery",      30,         0},
	{"",   "~/.local/bin/desktop/statusbar/volume",       30,         1},
	{"",   "~/.local/bin/desktop/statusbar/layout",       30,         2},
	{" ", "~/.local/bin/desktop/statusbar/clock",         5,         0},
};

// sets delimeter between status commands. NULL pointer means no delimeter.
static const char * delim = " | ";
