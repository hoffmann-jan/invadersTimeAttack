prog.bin: mkDirs obj/invader.o obj/myconio.o obj/List.o obj/InputThread.o
	cc -o bin/prog.bin obj/invader.o obj/myconio.o obj/List.o obj/InputThread.o -lpthread

obj/invader.o: invader.c
	cc -c invader.c -o obj/invader.o

obj/myconio.o: myconio.c
	cc -c myconio.c -o obj/myconio.o

obj/List.o: List.c
	cc -c List.c -o obj/List.o

obj/Entity.o: Entity.c
	cc -c Entity.c -o obj/Entity.o

obj/Position.o: Position.c
	cc -c Position.c -o obj/Position.o

obj/InputThread.o: InputThread.c
	cc -c InputThread.c -o obj/InputThread.o

mkDirs:
	rm -rf ./obj
	rm -rf ./bin
	mkdir -p obj
	mkdir -p bin
