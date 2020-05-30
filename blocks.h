//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/	                        	/*Update Interval*/	/*Update Signal*/
	/* {"\uf487"  , "cat ~/.pacupdate | sed /0/d"                             , 0  , 9}  , */

	{"\ue28c " , "free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g" , 30 , 0}  ,

	/* {""        , "~/bin/statusbar/volume"                                  , 0  , 10} , */

	{"\uf2c9 " , "sensors | awk '/^temp1:/{print $2}'"                     , 5  , 0}  ,

	/* {""        , "~/bin/statusbar/clock"                                   , 5  , 0}  , */
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = "|@|456";
static unsigned int delimLen = 5;
