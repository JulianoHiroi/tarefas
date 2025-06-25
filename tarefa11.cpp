/**
 * @file light.cpp
 * Defines colors for a light font and for an object.
 * 
 * @author Ricardo Dutra da Silva
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../lib/utils.h"
#include <vector>


/* Globals */
/** Window width. */
int win_width  = 600;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;


/** Vertex shader. */
const char *vertex_code = "\n"
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position, 1.0);\n"
"}\0";

/** Fragment shader. */
const char *fragment_code = "\n"
"#version 330 core\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"void main()\n"
"{\n"
"    fragColor = vec4(1.0 , 1.0, 1.0, 1.0);\n"
"}\0";

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void initData(void);
void initShaders(void);

/** 
 * Drawing function.
 *
 * Draws primitive.
 */
 std::vector<glm::vec3> circlePoints;

void bresenhamCircle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;

    auto plotCirclePoints = [&](int x, int y) {
        circlePoints.push_back(glm::vec3(xc + x, yc + y, 0.0f));
        circlePoints.push_back(glm::vec3(xc - x, yc + y, 0.0f));
        circlePoints.push_back(glm::vec3(xc + x, yc - y, 0.0f));
        circlePoints.push_back(glm::vec3(xc - x, yc - y, 0.0f));
        circlePoints.push_back(glm::vec3(xc + y, yc + x, 0.0f));
        circlePoints.push_back(glm::vec3(xc - y, yc + x, 0.0f));
        circlePoints.push_back(glm::vec3(xc + y, yc - x, 0.0f));
        circlePoints.push_back(glm::vec3(xc - y, yc - x, 0.0f));
    };

    while (x <= y) {
        plotCirclePoints(x, y);
        if (d < 0)
            d = d + 4 * x + 6;
        else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
void printCirclePoints() {
    std::cout << "Pontos do círculo (x, y):" << std::endl;
    for (const auto& p : circlePoints) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }
    
     std::cout << "quantidades de pontos do círculo (x, y):" << circlePoints.size() << std::endl;
}

std::vector<glm::vec3> toNDC(const std::vector<glm::vec3>& points, int width, int height) {
    std::vector<glm::vec3> ndcPoints;
    for (const auto& p : points) {
        float x_ndc = (2.0f * p.x) / width - 1.0f;
        float y_ndc = 1.0f - (2.0f * p.y) / height; // inverte Y para OpenGL
        ndcPoints.push_back(glm::vec3(x_ndc, y_ndc, 0.0f));
    }
    return ndcPoints;
}

 
void display()
{
        std::cout << "Cheguei no display"  << std::endl;
        
        printCirclePoints();
    	glClearColor(0.2, 0.3, 0.3, 1.0); 
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    	glUseProgram(program);
    	glBindVertexArray(VAO);

	glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(1.0f,0.0f,0.0f));
	glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 model = Rx*Ry;
	unsigned int loc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));
	
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f));
	loc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (win_width/(float)win_height), 0.1f, 100.0f);
 	loc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

    	glPointSize(4.0); // Tamanho visível dos pontos
        glDrawArrays(GL_POINTS, 0, circlePoints.size());


    	glutSwapBuffers();
}

/**
 * Reshape function.
 *
 * Called when window is resized.
 *
 * @param width New window width.
 * @param height New window height.
 */
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}


/** 
 * Keyboard function.
 *
 * Called to treat pressed keys.
 *
 * @param key Pressed key.
 * @param x Mouse x coordinate when key pressed.
 * @param y Mouse y coordinate when key pressed.
 */
void keyboard(unsigned char key, int x, int y)
{
        switch (key)
        {
                case 27:
                        glutLeaveMainLoop();
                case 'q':
                case 'Q':
                        glutLeaveMainLoop();
        }
    
	glutPostRedisplay();
}


/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData()
{
    // Gera os pontos do círculo (com centro em 0,0 e raio 100)
    bresenhamCircle(0, 0, 100);
    circlePoints = toNDC(circlePoints, win_width, win_height); 
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circlePoints.size() * sizeof(glm::vec3), &circlePoints[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glEnable(GL_PROGRAM_POINT_SIZE); // Permite mudar tamanho dos pontos
    glEnable(GL_DEPTH_TEST);
}

/** Create program (shaders).
 * 
 * Compile shaders and create the program.
 */
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow(argv[0]);
	glewInit();

    	// Init vertex data for the triangle.
    	initData();
    
    	// Create shaders.
    	initShaders();
	
    	glutReshapeFunc(reshape);
    	glutDisplayFunc(display);
    	glutKeyboardFunc(keyboard);

	glutMainLoop();
}