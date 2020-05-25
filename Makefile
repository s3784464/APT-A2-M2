.default: all

all: Azul

clean:
	rm -f Azul *.o

Azul: main.o Bag.o Board.o Factory.o GameState.o Lid.o Player.o Tile.o LinkedListTileADT.o Persistence.o 
	g++ -Wall -Werror -std=c++14 -O -o $@ $^

%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -O -c $^