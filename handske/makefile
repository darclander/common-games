
OBJS = ./src/*.cpp

CC=g++

OPTIMIZATION= #-O3

#CFLAGS= -I$(IDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF -lSDL2_Mixer

ifeq ($(OS),Windows_NT)
    # Windows system
    CFLAGS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF -lSDL2_Mixer
else
    # Non-Windows system
    CFLAGS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
endif

IDIR = ./src/headers
_DEPS = Gui.hpp Grid.hpp Menu.hpp Score.hpp Controller.hpp Soundmanager.hpp Player.hpp Camera.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Grid.o Player.o main.o Menu.o Score.o Soundmanager.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: handske

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< $(CFLAGS)

handske: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) 

run: handske
	./debug/handske

clean: remove handske

remove:
	rm -r **/*.o
	rm ./debug/handske.exe
