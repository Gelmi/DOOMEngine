PROJ_NAME=DOOMengine

C_SOURCE=$(wildcard *.cpp)

OBJ=$(C_SOURCE:.c=.o)

CC=g++

CC_FLAGS=-c         \
         -W         \
         -Wall      \
         -ansi      \
         -pedantic

# CC_SDL=`pkgconf --cflags --libs sdl2`
CC_SDL := `sdl2-config --libs --cflags` -lSDL2_ttf

all: $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CC_SDL)

%.o: %.c
	$(CC) -o $@ $< $(CC_FLAGS)

clean:
	rm -rf *.o $(PROJ_NAME) *~
