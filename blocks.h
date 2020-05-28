//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
  /* icon */ /* command */ /* update interval */ /* update signal */
  {"net: ", "nmcli | (rg 'connected to' || echo disconnected) | sed 's/.*connected to //g'", 5, 0},

  {"",      "echo ' | '", 0, 0},

  {"vol: ", "amixer get Master | tail -n1 | grep -o \"\\[.*\\]\"", 0, 1},

  {"",      "echo ' | '", 0, 0},

  {"bat: ", "upower -i $(upower -e | rg BAT0) | rg -om1 '.*%' | awk '{print $2}'", 60, 0},

  {"",      "echo ' | '", 0, 0},

  {"",      "date +\'%a %F %H:%M\'", 60, 0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = ' ';
