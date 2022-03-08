//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {

/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
  {"",  "nettraf",  1,  16},
  {"",  "memory",   10, 14}, 
  {"",  "volume",   1,  10},
  {"",  "clock",    5,  1},
  {"",  "battery",  5,  3},
  {"",  "internet", 5,  4},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = "|";
static unsigned int delimLen = 5;
