compile (windows 64 bit):

gcc arms.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -O3 -o arms.exe

or just type 'make' in the terminal.

to run:

arms.exe

additional flags:
-f {filename}, inputs a file

example:
arms.exe PistolData.txt

Linux (limited support):
gcc arms.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o arms.o

if it doesn't work you'll probably need to install glad and glfw and compile the libraries (glad and glfw).
once you've obtained the libglad.a and libglfw3.a files, 
replace the ones in the folder called "Linux" and recompile
But also this project uses windows.h and the win32 api, so i'll just say good luck