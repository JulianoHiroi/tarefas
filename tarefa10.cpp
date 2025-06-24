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
unsigned int VAOretangle, VAOpolygon;
/** Vertex buffer object. */
unsigned int VBOretangle, VBOpolygon;

unsigned int VAOclipped, VBOclipped;
std::vector<glm::vec2> clippedPolygon;




glm::vec2 points[2];    
int click_count = 0;      
bool ready_to_draw = false;

enum Mode { SELECT_RECTANGLE, SELECT_POLYGON };
Mode mode = SELECT_RECTANGLE;

std::vector<glm::vec2> polygonPoints;
bool draw_polygon = false;


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
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"}\0";

const char *fragment_code = R"(
#version 330 core
out vec4 fragColor;
uniform vec3 color;
void main()
{
    fragColor = vec4(color, 1.0);
}
)";

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void initData(void);
void initShaders(void);
std::vector<glm::vec2> sutherlandHodgman(const std::vector<glm::vec2>& polygon, glm::vec2 pMin, glm::vec2 pMax);

/** 
 * Drawing function.
 *
 * Draws primitive.
 */
void display()
{
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (ready_to_draw) {
        glBindVertexArray(VAOretangle);
        glUniform3f(glGetUniformLocation(program, "color"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    if (draw_polygon) {
        if (!polygonPoints.empty()) {
            glBindVertexArray(VAOpolygon);
            glUniform3f(glGetUniformLocation(program, "color"), 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 0, polygonPoints.size());
        }

        if (!clippedPolygon.empty()) {
            glBindVertexArray(VAOclipped);
            glUniform3f(glGetUniformLocation(program, "color"), 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 0, clippedPolygon.size());
        }
    }

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

std::vector<glm::vec2> sutherlandHodgman(const std::vector<glm::vec2>& polygon, glm::vec2 pMin, glm::vec2 pMax)
{
    enum Edge { LEFT, RIGHT, BOTTOM, TOP };

    auto inside = [&](const glm::vec2& p, Edge edge) -> bool {
        switch (edge) {
            case LEFT:   return p.x >= pMin.x;
            case RIGHT:  return p.x <= pMax.x;
            case BOTTOM: return p.y >= pMin.y;
            case TOP:    return p.y <= pMax.y;
        }
        return false;
    };

    auto intersection = [&](const glm::vec2& p1, const glm::vec2& p2, Edge edge) -> glm::vec2 {
        float x, y;
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;

        switch (edge) {
            case LEFT:
                x = pMin.x;
                y = p1.y + dy * (pMin.x - p1.x) / dx;
                break;
            case RIGHT:
                x = pMax.x;
                y = p1.y + dy * (pMax.x - p1.x) / dx;
                break;
            case BOTTOM:
                y = pMin.y;
                x = p1.x + dx * (pMin.y - p1.y) / dy;
                break;
            case TOP:
                y = pMax.y;
                x = p1.x + dx * (pMax.y - p1.y) / dy;
                break;
        }
        return glm::vec2(x, y);
    };

    auto clipPolygon = [&](const std::vector<glm::vec2>& input, Edge edge) -> std::vector<glm::vec2> {
        std::vector<glm::vec2> output;
        if (input.empty()) return output;

        glm::vec2 S = input.back();

        for (const glm::vec2& E : input) {
            if (inside(E, edge)) {
                if (!inside(S, edge)) {
                    output.push_back(intersection(S, E, edge));
                }
                output.push_back(E);
            } else if (inside(S, edge)) {
                output.push_back(intersection(S, E, edge));
            }
            S = E;
        }

        return output;
    };

    std::vector<glm::vec2> output = polygon;

    for (Edge edge : {LEFT, RIGHT, BOTTOM, TOP}) {
        output = clipPolygon(output, edge);
        if (output.empty()) break;
    }

    return output;
}



void initDataFromRectangle(glm::vec2 p1, glm::vec2 p2)
{
    float z = 0.0f;  // profundidade fixa

    float vertices[] = {
        p1.x, p1.y, z,  // Bottom-left
        p2.x, p1.y, z,  // Bottom-right
        p2.x, p2.y, z,  // Top-right
        p1.x, p2.y, z   // Top-left
    };

    if (VAOretangle == 0) glGenVertexArrays(1, &VAOretangle);
    if (VBOretangle == 0) glGenBuffers(1, &VBOretangle);

    glBindVertexArray(VAOretangle);
    glBindBuffer(GL_ARRAY_BUFFER, VBOretangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void initDataFromPolygon(const std::vector<glm::vec2>& points)
{
    std::vector<float> vertices;
    float z = 0.0f;

    for (const auto& p : points) {
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(z);
    }

    if (VAOpolygon == 0) glGenVertexArrays(1, &VAOpolygon);
    if (VBOpolygon == 0) glGenBuffers(1, &VBOpolygon);

    glBindVertexArray(VAOpolygon);
    glBindBuffer(GL_ARRAY_BUFFER, VBOpolygon);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void initDataFromPolygonClipped(const std::vector<glm::vec2>& points)
{
    std::vector<float> vertices;
    float z = 0.0f;

    for (const auto& p : points) {
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(z);
    }

    if (VAOclipped == 0) glGenVertexArrays(1, &VAOclipped);
    if (VBOclipped == 0) glGenBuffers(1, &VBOclipped);

    glBindVertexArray(VAOclipped);
    glBindBuffer(GL_ARRAY_BUFFER, VBOclipped);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}



glm::vec2 windowToNDC(int x, int y)
{
    float ndc_x = (2.0f * x) / win_width - 1.0f;
    float ndc_y = 1.0f - (2.0f * y) / win_height;
    return glm::vec2(ndc_x, ndc_y);
}

void mouseCallback(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        glm::vec2 ndc = windowToNDC(x, y);

        if (mode == SELECT_RECTANGLE) {
            if (click_count < 2) {
                points[click_count] = ndc;
                click_count++;

                if (click_count == 2) {
                    glm::vec2 p1 = glm::min(points[0], points[1]);
                    glm::vec2 p2 = glm::max(points[0], points[1]);
                    initDataFromRectangle(p1, p2);
                    
                    ready_to_draw = true;
                    mode = SELECT_POLYGON; // muda de modo após o retângulo
                }
            }
        } else if (mode == SELECT_POLYGON && !draw_polygon) {
            polygonPoints.push_back(ndc);
        }

        glutPostRedisplay();
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (mode == SELECT_POLYGON && polygonPoints.size() >= 3) {
            initDataFromPolygon(polygonPoints);
            clippedPolygon = sutherlandHodgman(polygonPoints, glm::min(points[0], points[1]), glm::max(points[0], points[1]));
            initDataFromPolygonClipped(clippedPolygon);
            draw_polygon = true;
        }
        glutPostRedisplay();
    }
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
    
    
    	// Create shaders.
    	initShaders();
	    glutMouseFunc(mouseCallback);
    	glutReshapeFunc(reshape);
    	glutDisplayFunc(display);
    	glutKeyboardFunc(keyboard);

	glutMainLoop();
}
