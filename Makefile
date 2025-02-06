CXX = g++
CXXFLAGS	= -std=c++17 -Wall -Werror -Wpedantic

all: pacman

clean:
	rm Player.o Game.o pacman

pacman: Player.o Game.o main.cpp
	$(CXX) $(CXXFLAGS) main.cpp Player.o Game.o -o pacman

Player.o: Player.cpp
	$(CXX) $(CXXFLAGS) -c Player.cpp

Game.o: Game.cpp
	$(CXX) $(CXXFLAGS) -c Game.cpp
