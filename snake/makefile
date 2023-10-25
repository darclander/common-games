
OBJS = ./src/*.cpp

CC=g++

CFLAGS= -I$(IDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image 


IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp Grid.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c -O3 -o $@ $< $(CFLAGS)

snake: $(OBJ)
	$(CC) -O3 -o ./debug/$@ $^ $(CFLAGS) 

run: snake
	./debug/snake

clean:
	rm -r **/*.o
	rm ./debug/snake.exe
