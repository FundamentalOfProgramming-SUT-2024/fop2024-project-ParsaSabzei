main: main.c Makefile
	gcc main.c -I/usr/include/SDL2 -lSDL2 -lSDL2_mixer -lncursesw && ./a.out

clean:
	rm -f main
