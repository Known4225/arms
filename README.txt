compile (windows 64 bit):

gcc arms.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -o arms.exe

or just type 'make' in the terminal.

to run:

arms.exe

additional flags:
-f {filename}, inputs a file

example:
arms.exe PistolData.txt

What is this:
This is a small program I wrote in scratch 5 years ago, now in C!
You basically just draw on the grid by clicking and holding space to make a line
Then you can press G to render your drawing in 3D (perspective projection, fancy) and rotate it with the arrow keys or WASD
That's it, I wanted to use it to draw guns for a first person shooter, but never actually made the first person shooter.
This project also serves as a tech demo for my custom ribbon implementation. It uses the windows API to save and load files through the gui interface, which I have not done before. Also just rendering the ribbon (textGL, it's all coming together) which has a custom ribbonConfig file that can be changed for different ribbon configurations. The output can then be "piped" to use in a wide variety of ways (the windows API implementation is not built in to the ribbon "class", it's a result of piping the output of the ribbon to another "class" which interfaces with the windows API because i felt like i needed an extra layer of abstraction for some reason).
Also I updated turtle to "blit" polygons to the screen so if you want to add a triangle or quad directly you don't have to use the pen tools if you don't want to. Blit will integrate the data into the stream seamlessly (with a very wacky data encoding scheme, also I only added support for triangles and quads for now).

Linux (limited support):
gcc arms.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o arms.o

if it doesn't work you'll probably need to install glad and glfw and compile the libraries (glad and glfw).
once you've obtained the libglad.a and libglfw3.a files, 
replace the ones in the folder called "Linux" and recompile
But also this project uses windows.h and the win32 api, so i'll just say good luck