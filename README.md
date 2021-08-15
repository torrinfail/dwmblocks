# About  dwmblocks
modular status bar for [suckless dwm](https://dwm.suckless.org/) (dynamic window manager)  written in c.

# Installation and usage
## On gentoo linux
dwmblocks can be installed by portage, when the **glarbs-overlay** is added with [eselect/repository]() or [layman](https://wiki.gentoo.org/wiki/layman).

With eselect/repository:
```
emerge --ask app-eselect/eselect-repository
```
```
eselect repository enable glarbs-overlay
```
With layman:
```
emerge --ask app-portage/layman
```
```
layman -a glarbs-overlay
```
install dwmblocks with the command:
```
emerge --ask --verbose x11-mic/dwmblocks
```

## On other linux distributions
to use dwmblocks first run 'make' and then install it with elevated privileges (doas or sudo) eg. 'doas make install' or 'sudo make install'.
after that you can put dwmblocks in your xinitrc or other startup script to have it start with dwm.

# Modifying blocks
## On gentoo linux
the statusbar is made from text output from commandline programs.
blocks are added and removed by editing the blocks.h header file, which by portage is renamed to *dwmblocks-1.0.0*.
enable the **savedconfig** use flag in order to save customized configuration file to */etc/portage/savedconfig/x11-misc/dwmblocks-1.0.0.* for later editing.
```
euse --enable savedconfig
```

## On other linux distributions
the statusbar is made from text output from commandline programs.
blocks are added and removed by editing the **blocks.h** header file.
by default the blocks.h header file is created the first time you run make which copies the default config from **blocks.def.h**.
this is so you can edit your status bar commands and they will not get overwritten in a future update.

# Patches
## On gentoo linux
patches to dwmblocks can be installed by portage, when the **glarbs-overlay** is added with [eselect/repository]() or [layman](https://wiki.gentoo.org/wiki/layman).
With eselect/repository:
```
emerge --ask app-eselect/eselect-repository
```
```
eselect repository enable glarbs-overlay
```
With layman:
```
emerge --ask app-portage/layman
```
```
layman -a glarbs-overlay
```

**if a patch is not in the glarbs-overlay, read the [glarbs article on the gentoo wiki](https://wiki.gentoo.org/wiki/user:et-8/glarbs) how to add it.**

## On other linux distributions
here are some patches to dwmblocks that add features that torrinfail either don't want to merge in, or that require a dwm patch to work.
torrinfail do not maintain these, but will take pull requests to update them.

* [dwmblocks-statuscmd-b6b0be4.diff](https://gist.github.com/igeragera/e4a5583b91b3eec2e81fdceb44dea717)
