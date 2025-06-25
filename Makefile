CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: tarefa9.cpp tarefa10.cpp tarefa11.cpp
	$(CC) tarefa9.cpp ../lib/utils.cpp ./stb_image.h -o tarefa9 $(GLLIBS) 
	$(CC) tarefa10.cpp ../lib/utils.cpp ./stb_image.h -o tarefa10 $(GLLIBS) 
	$(CC) tarefa11.cpp ../lib/utils.cpp ./stb_image.h -o tarefa11 $(GLLIBS) 

clean:
	rm -f tarefa9 tarefa10 tarefa11

