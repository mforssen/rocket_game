#OBJS specifies which files to compile as part of the project 
OBJS = main.o game.o sprite.o
GAME_OBJS = game.cpp game.hpp sprite.cpp sprite.hpp game_enum.cpp
MAIN_OBJS = main.cpp game.cpp game.hpp
SPRITE_OBJS = sprite.cpp sprite.hpp

#CC specifies which compiler we're using 
CC = g++ 

#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 
COMPILER_FLAGS = -Wall -Wextra

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lSDL -lSDL_image

#OBJ_NAME specifies the name of our exectuable 
OBJ_NAME = game

#This is the target that compiles our executable 
all: game

game: main.o game.o sprite.o
	$(CC) $(OBJS) -o $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

main.o: main.cpp game.cpp game.hpp
	$(CC) -c $(MAIN_OBJS)
        
game.o: game.cpp game.hpp sprite.cpp sprite.hpp game_enum.cpp
	$(CC) -c $(GAME_OBJS)

sprite.o: sprite.cpp sprite.hpp
	$(CC) -c $(SPRITE_OBJS)
	
clean:
	rm -rf *o game
		
