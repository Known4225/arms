all:
	gcc armsLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o armsLinux.o
val:
	gcc armsLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o armsLinux.o
clean:
	rm armsLinux.o