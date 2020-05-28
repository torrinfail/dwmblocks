// Modify this file to change what commands output to your statusbar, and recompile using the make command.

static const Block blocks[] = {
	/*Icon*/        /*Command*/        /*Update Interval*/        /*Update Signal*/
	{ "",     "netif.ip",              60,                        0 },
	{ "",     "volume",                0,                         1 },
	{ "",     "datetime",              5,                         0 },
};

// Sets delimeter between status commands.
static char delim[]       = " ][ ";
static char left_delim[]  = "[ ";
static char right_delim[] = " ]";
