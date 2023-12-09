
OBJS = ./src/*.cpp

CC=g++

OPTIMIZATION= # O3

CFLAGS= -I$(IDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF


IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp Grid.hpp Menu.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< $(CFLAGS)

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) 

run: snake
	./debug/snake

clean:
	rm -r **/*.o
	rm ./debug/snake.exe
