pianistic: pianistic.c
	gcc pianistic.c -o pianistic -g `sdl-config --cflags --libs` -lSDL_mixer -lX11
