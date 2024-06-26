#OBJS specifies which files to compile as part of the project
OBJS = main.cpp game.cpp utils.cpp rendob.cpp matte.cpp machine.cpp popup.cpp #SDL_ttf.h

#CC specifies which compiler we're using
CC = g++ -std=c++17

#INCLUDE_PATHS specifies the additional include paths we'll need, for code interface
INCLUDE_PATHS = -IC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2\include\
				-IC:\Users\3rix4\Downloads\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\include\
				-IC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2_image-2.8.2\x86_64-w64-mingw32\include\
				-IC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2_mixer-2.8.0\x86_64-w64-mingw32\include

#LIBRARY_PATHS specifies the additional library paths we'll need, extra code (ttf not working here)
LIBRARY_PATHS = -LC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2\lib\
				-LC:\Users\3rix4\Downloads\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\lib\
				-LC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2_image-2.8.2\x86_64-w64-mingw32\lib\
				-LC:\Users\3rix4\OneDrive\Documents\codeFolder\c\sdlGames\SDL2_mixer-2.8.0\x86_64-w64-mingw32\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -Wall#-w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against, dll binaries, must be in same directory as .exe
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = run

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) -g