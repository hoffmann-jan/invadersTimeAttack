all: ncurseInvader
# all: prog.bin
# all: example

prog.bin: clean obj/invader.o obj/myconio.o obj/List.o obj/InputThread.o
	cc -o bin/prog.bin obj/invader.o obj/myconio.o obj/List.o obj/InputThread.o -lpthread

ncurseInvader: clean invader_ncurses.c obj/InputThread.o obj/List.o
	cc -o bin/invader_ncurses.bin obj/InputThread.o obj/List.o invader_ncurses.c -lncurses -lpthread

obj/invader.o: invader.c
	cc -c invader.c -o obj/invader.o

obj/myconio.o: myconio.c
	cc -c myconio.c -o obj/myconio.o

obj/List.o: List.c
	cc -c List.c -o obj/List.o

obj/Position.o: Position.c
	cc -c Position.c -o obj/Position.o

obj/InputThread.o: InputThread.c
	cc -c InputThread.c -o obj/InputThread.o

example: example_Invaders.c
	cc -o bin/example.bin example_Invaders.c -lncurses

clean:
	rm -rf ./obj
	rm -rf ./bin
	mkdir -p obj
	mkdir -p bin

