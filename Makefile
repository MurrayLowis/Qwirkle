.default: all

all: qwirkle

clean:
	rm -rf Qwirkle *.o *.dSYM

qwirkle: Tile.o LinkedList.o Game.o Player.o Qwirkle.o 
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c $^
