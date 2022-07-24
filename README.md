# DWMBlocks
Made to compile for OpenBSD. Comment out the OpenBSD line in the Makefile to compile for Linux. Uncomment the FreeBSD line to compile for FreeBSD.

# Compiling and Installing
`make && doas make install`
Replace doas with sudo as necessary.

# Dependencies
Each module depends on a script of mine, specifically the following:
- `sb-cpuusage`
- `sb-volume` (need to find a better solution)
- `sb-memory`
- `sb-cputemp`
- `sb-network`
- `sb-battery`
All of these scripts can be found in my [dotfiles repo](/swindlesmccoop/not-just-dotfiles). Simply download each script, make them each executable, and place them somewhere in your $PATH.
