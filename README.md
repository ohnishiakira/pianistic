PIANISTIC
=========
What music geeks listen to when working?
With "pianistic", no need for looking for new musics for BGM.
Your fast key typing makes your own, various BGM.

Pianistic plays sounds according to your keyboard input.

[Demo]().
[Blog Article in Japanese]().

* The same key plays the same sound.
* The same sequence of inputs must plays the same merody, but sometimes 
not.(I am not sure why. Latency of X RECORD?)

How To Use
----------
* Get sounds you like. In my demo, using [Grand Piano Chords Fazioli Minor Samples Highest Pitch](http://ibeat.org/piano-chords-free/).
* Rename them from 'a' to 'z', and put them in 'audio/' directory (See `loadSounds' function).
* Install libraries (these are the package names in Ubuntu).
  > libsdl-mixer1.2-dev
  > libxtst-dev
* Compile with `gcc pianistic.c -o pianistic -g `sdl-config --cflags --libs` -lSDL_mixer -lX11'
* Start ./pianistic, and enjoy!

Problems
--------

This software works on X11 environment only, but some hacker will easily
replicate to OS X and Windows.

This application is a little heavy.
I am really a newbie of X11 programming, there must be more fast way
to do the same thing.
