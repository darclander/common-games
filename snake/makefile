
OBJS = ./src/*.cpp

CC=g++

OPTIMIZATION= #-O3

CFLAGS= -I$(IDIR) -Wall

ifeq ($(OS),Windows_NT)
    # Windows system
    CFLAGS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF -lSDL2_Mixer
else
    # Non-Windows system
    CFLAGS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
endif

IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp Grid.hpp Menu.hpp Score.hpp Controller.hpp Soundmanager.hpp Task.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o Score.o Soundmanager.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< $(CFLAGS)

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) 

run: snake
	./debug/snake

clean: remove snake

# Fix removing for linux as well?
remove:
	rm -r **/*.o
	rm ./debug/snake.exe