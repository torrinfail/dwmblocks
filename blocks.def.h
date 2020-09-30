//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	{"", " curl -s 'wttr.in/Białystok?format=3'",	   			3600,		0},
	{"", "df -h | awk '{ if ($6 == \"/\") print \"⛁ \"$4 }'",	  	300,		0},
	{"", "sensors | awk '/Core 0/ {print \"🌡 \" $3}'",			5,		0},
	{"", "sensors | awk '/Core 1/ {print \"🌡 \" $3}'",			5,		0},
	{"Mem:", "free -h | awk '/^Pam/ { print $3\"/\"$2 }' | sed s/i//g",	30,		0},

	{"", "date '+%b %d (%a) %H:%M%p'",					30,		0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = " | ";
static unsigned int delimLen = 5;
