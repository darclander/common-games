
OBJS = ./src/*.cpp

CC=g++

CFLAGS= -I$(IDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image 


IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ = main.o Gui.o Snake.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

snake: $(OBJ)
	$(CC) -o ./debug/$@ $^ $(CFLAGS) 

run: snake
	./debug/snake

clean:
	rm -r **/*.o
	rm ./debug/snake.exe
