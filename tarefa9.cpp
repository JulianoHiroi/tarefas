/**
 * @file phong.cpp
 * Applies the Phong method.
 * 
 * @author Ricardo Dutra
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../lib/utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* Globals */
int win_width  = 600;
int win_height = 600;

int program;
unsigned int VAO, VBO;
unsigned int texture;

/** Vertex shader */
const char *vertex_code = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)";

/** Fragment shader */
const char *fragment_code = R"(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

void display();
void reshape(int, int);
void keyboard(unsigned char, int, int);
void initData();
void initShaders();

/** Drawing function */
void display()
{
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);

    glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 model = Rx * Ry;
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)win_width/win_height, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27 || key == 'q' || key == 'Q') {
        glutLeaveMainLoop();
    }
    glutPostRedisplay();
}

void initData()
{
    float vertices[] = {
    // positions          // colors           // texture coords
    // Front face
    -0.5f, -0.5f,  0.5f,  1, 0, 0,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0, 0, 1,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0, 0, 1,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1, 0, 0,  0.0f, 0.0f,

    // Back face
    -0.5f, -0.5f, -0.5f,  1, 0, 1,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0, 1, 1,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0, 0, 0,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0, 0, 0,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1, 1, 1,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1, 0, 1,  0.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f,  1, 1, 0,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1, 0, 0,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0, 0, 1,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,  1.0f, 0.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1, 1, 1,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0, 1, 1,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 1,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 1,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1, 1, 0,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1, 1, 1,  1.0f, 0.0f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0, 0, 0,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 0,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0, 0, 1,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0, 0, 0,  0.0f, 1.0f,

    // Top face
    -0.5f,  0.5f, -0.5f,  1, 1, 0,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0, 1, 1,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1, 0, 1,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1, 0, 1,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1, 1, 1,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1, 1, 0,  0.0f, 1.0f
};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Gera e vincula a textura
    // Gera um identificador para a textura
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Seta os parâmetros da textura

    //Repetição da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtro de minificação e magnificação
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a textura 
    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void initShaders()
{
    program = createShaderProgram(vertex_code, fragment_code);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Phong Shading");

    glewInit();

    initData();
    initShaders();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
}
