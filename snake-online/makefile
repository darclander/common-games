
OBJS = ./src/*.cpp

CC=g++

OPTIMIZATION= #-O3

CFLAGS= -I$(IDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF -lSDL2_Mixer -lws2_32


IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp Grid.hpp Menu.hpp Score.hpp Controller.hpp Soundmanager.hpp comm_util.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o Score.o Soundmanager.o comm_util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< $(CFLAGS)

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) 

run: snake
	./debug/snake

clean: remove snake

remove:
	./remove.py

