//Modify this file to change what commands output to your statusbar, and recompile using the make command.
#ifdef __linux__
	#define SIG 10
#elif __OpenBSD__
	#define SIG 1
#elif __FreeBSD__
	#define SIG 1
#endif

static const Block blocks[] = {
//	  Label		   Command				Int		SIG
	{"CPU: ",	"sb-cpuusage",			1,		0},
	{"Vol: ",	"sb-volume",			1,		SIG},
	{"Mem: ",	"sb-memory",			1,		0},
	{"Temp: ",	"sb-cputemp -f",		1,		0},
	//{"IP: ",	"sb-network",			1,		0},
	{"Bat: ",	"sb-battery",			60,		0},
	{"",		"date '+%b %d, %H:%M'",	1,		0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = " | ";
static unsigned int delimLen = 5;

