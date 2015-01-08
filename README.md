# osd-pomodoro

## Introduction

A simple, ultra-lightweight, transparent, unobtrusive pomodoro countdown for Linux. Always on top, yet so small that it doesn't really bother you. And if it does for a moment, just send it a UNIX signal to toggle visibility.

## Features

 - Uses libxosd2 for achieving pseudo-transparency. Floats  on  top  of all  windows  (is  shaped and unmanaged), everything but the letters is transparent.
 - User font, position, outline, drop  shadow, color, duration of countdown
 - Can hide/show/toggle visibility upon signal receive. Define keyboard shortcuts in your favourite WM and toggle visibility when if it obscures some part of the screen you need to see.
 -  Compact, easy to modify, free C source code you can alter to suit your needs.

Suggestions, praises, feature request are welcome.

Depends on libxosd, under debian:

```
# apt-get install libxosd-dev
```

or, maybe


```
# apt-get install libxosd2-dev
```

To compile, type 'make'.

To install, type 'make install' or simply copy the resulting binary anywhere
you like.

## Running

To run, see 

```
$ osd_pomodoro --help 
```

or the included example script 

```
$ run_pomodoro.sh
```

There is also a manual page included, which describes the usage in greater detail


## Credits

Inspired by the osd_clock program by jon beckham <leftorium@leftorium.net>
