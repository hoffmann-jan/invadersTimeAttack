prog.bin: mkDirs obj/invader.o obj/myconio.o obj/List.o 
	cc -o bin/prog.bin obj/invader.o obj/myconio.o obj/List.o

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

mkDirs:
	rm -r ./obj
	rm -r ./bin
	mkdir -p obj
	mkdir -p bin
