D=dsnake
V=vectorsnake
CC=g++ -Wall -lncurses
all: bin/$(D) bin/$(V)
bin/$(D): obj/$(D).o
	$(CC) obj/$(D).o -o bin/$(D)
obj/$(D).o: src/$(D).cpp
	$(CC) -c src/$(D).cpp -o obj/$(D).o
bin/$(V): obj/$(V).o
	$(CC) obj/$(V).o -o bin/$(V)
obj/$(V).o: src/$(V).cpp
	$(CC) -c src/$(V).cpp -o obj/$(V).o
clean:
	rm -rf bin/* obj/*.o