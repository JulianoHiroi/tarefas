CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: tarefa9.cpp tarefa10.cpp
	$(CC) tarefa9.cpp ../lib/utils.cpp ./stb_image.h -o tarefa9 $(GLLIBS) 
	$(CC) tarefa10.cpp ../lib/utils.cpp ./stb_image.h -o tarefa10 $(GLLIBS) 

clean:
	rm -f tarefa9 tarefa10

