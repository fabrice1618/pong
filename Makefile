pong: pong.c
	gcc pong.c `sdl2-config --cflags --libs` -o pong -lSDL2main -lSDL2_ttf -lSDL2
