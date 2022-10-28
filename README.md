# DWMBlocks
Status bar built for my build of [DWM](/swindlesmccoop/dwm). Compatibel with Linux, OpenBSD, and FreeBSD, however I have not yet written the actual functionality for FreeBSD in the scripts yet.

# Compiling and Installing
```
./configure && make && doas make install
```
Replace doas with sudo as necessary.

## Software Made to Work With This
- [DWM](/swindlesmccoop/dwm)
- [DMenu](/swindlesmccoop/dmenu)
- [ST](/swindlesmccoop/st)

# Dependencies
Each module depends on a script of mine, specifically the following:
- `sb-cpuusage`
- `sb-volume`
- `sb-memory`
- `sb-cputemp`
- `sb-network`
- `sb-battery`\
All of these scripts can be found in my [dotfiles repo](/swindlesmccoop/not-just-dotfiles). Simply download each script, make them each executable, and place them somewhere in your $PATH.
