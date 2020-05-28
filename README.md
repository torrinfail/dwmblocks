# dwmblocks
Modular status bar for dwm written in c.
# Modifying blocks
The statusbar is made from text output from commandline programs.
Blocks are added and removed by editing the blocks.h header file.
# Dependencies
This implementation uses two other projects: [buffer](https://github.com/tomboehmer/buffer.git)
and [findandreplace](https://github.com/tomboehmer/findandreplace.git).
In order to build you will need to clone these into your *dwmblocks* directory
and run `make` on *findandreplace*.
