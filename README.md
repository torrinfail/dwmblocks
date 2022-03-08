# dwmblocks
Modular status bar for dwm written in c.
# usage
To use dwmblocks first run 'make' and then install it with 'sudo make install'.
After that you can put dwmblocks in your xinitrc or other startup script to have it start with dwm.
# modifying blocks
The statusbar is made from text output from commandline programs.
Blocks are added and removed by editing the blocks.h header file.
By default the blocks.h header file is created the first time you run make which copies the default config from blocks.def.h.
This is so you can edit your status bar commands and they will not get overwritten in a future update.
# patches
Here are the patches used in dwm and dwmblocks to give dwm the ability to signal dwmblocks, the location and button when clicking on the script icon on the status bar. This allows to perform additional actions like open up a terminal with the output/program or in combination with shift key open said script in vim for configuration:
<br>
<a href=https://dwm.suckless.org/patches/statuscmd/dwm-statuscmd-20210405-67d76bd.diff>dwm-statuscmd-20210405-67d76bd.diff</a>
<br>
<a href=https://dwm.suckless.org/patches/statuscmd/dwmblocks-statuscmd-20210402-96cbb45.diff>dwmblocks-statuscmd-20210402-96cbb45.diff</a>
