prog.bin: mkDirs obj/invader.o obj/myconio.o
	cc -o bin/prog.bin obj/invader.o obj/myconio.o

obj/invader.o: invader.c
	cc -c invader.c -o obj/invader.o

obj/myconio.o: myconio.c
	cc -c myconio.c -o obj/myconio.o

mkDirs:
	rm -r ./obj
	rm -r ./bin
	mkdir -p obj
	mkdir -p bin
