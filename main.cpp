#include <iostream>
#include "Angel.h"

typedef vec4 color4;
typedef vec4 point4;

bool isCube = true;

vec3 location(-0.9, 0.8, 0.0);
vec3 velocity(0.001, 0.0000001, 0.0);
vec3 acceleration(0.0, 0.00000001, 0.0);
vec4 white(1.0, 0.0, 0.0, 0.0);
vec4 currentColor = white;

static int window;
static int menu_id;
static int objectTypeMenu;
static int drawingModeMenu;
static int colorMenu;
static int valueMenu = 0;
int colorSelector = 0;

enum {
    Xaxis = 0,
    Yaxis = 1,
    Zaxis = 2,
    NumAxes = 3
};

int Axis = Xaxis;
int Zmeter = Zaxis;

GLfloat Theta[NumAxes] = {0.0, -20, 20};
GLuint ModelView, Projection;
GLuint program;

void colorChanger(std::string);

// ---------- CUBE BEGINNING ----------
// initial parameters taken from textbook - Appendix 4
const int NumVerticesCube = 36;
point4 pointsCube[NumVerticesCube];
color4 colorsCube[NumVerticesCube];

int IndexCube = 0;
double sizeCube = 1.0;

point4 vertices[8] = {
        point4(-0.1, -0.1, 0.1, sizeCube),
        point4(-0.1, 0.1, 0.1, sizeCube),
        point4(0.1, 0.1, 0.1, sizeCube),
        point4(0.1, -0.1, 0.1, sizeCube),
        point4(-0.1, -0.1, -0.1, sizeCube),
        point4(-0.1, 0.1, -0.1, sizeCube),
        point4(0.1, 0.1, -0.1, sizeCube),
        point4(0.1, -0.1, -0.1, sizeCube)
};

// taken from textbook - Appendix 4
void quad(int a, int b, int c, int d) {

    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[a];
    IndexCube++;
    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[b];
    IndexCube++;
    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[c];
    IndexCube++;
    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[a];
    IndexCube++;
    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[c];
    IndexCube++;
    colorsCube[IndexCube] = currentColor;
    pointsCube[IndexCube] = vertices[d];
    IndexCube++;
}

// taken from textbook - Appendix 4
void drawCube() {

    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);

}
// ---------- CUBE END ----------

// ---------- SPHERE BEGINNING ----------
const int NumTriangles = 1024;
const int NumVerticesSphere = 3 * NumTriangles;

point4 pointsSphere[NumVerticesSphere];
vec3 normalsSphere[NumVerticesSphere];
color4 colorSphere[NumVerticesSphere];

int IndexSphere = 0;

// taken from textbook - Appendix 7
void triangle(const point4 &a, const point4 &b, const point4 &c) {

    vec3 normal = normalize(cross(b - a, c - b));

    normalsSphere[IndexSphere] = normal;
    colorSphere[IndexSphere] = currentColor; // Added code for color
    pointsSphere[IndexSphere] = a;
    IndexSphere++;

    normalsSphere[IndexSphere] = normal;
    colorSphere[IndexSphere] = currentColor; // Added Code for color
    pointsSphere[IndexSphere] = b;
    IndexSphere++;

    normalsSphere[IndexSphere] = normal;
    colorSphere[IndexSphere] = currentColor; // Added Code For Color
    pointsSphere[IndexSphere] = c;
    IndexSphere++;
}

float sizeSphere = 10.0;

// taken from textbook - Appendix 7
point4 unit(const point4 &p) {

    float len = p.x * p.x + p.y * p.y + p.z * p.z;
    point4 t;

    if (len > DivideByZeroTolerance) {
        t = p / sqrt(len);
        t.w = sizeSphere;
    }

    return t;
}

// taken from textbook - Appendix 7
void divide_triangle(const point4 &a, const point4 &b, const point4 &c, int count) {

    if (count > 0) {

        point4 v1 = unit(a + b);
        point4 v2 = unit(a + c);
        point4 v3 = unit(b + c);
        divide_triangle(a, v1, v2, count - 1);
        divide_triangle(c, v2, v3, count - 1);
        divide_triangle(b, v3, v1, count - 1);
        divide_triangle(v1, v3, v2, count - 1);

    } else {
        triangle(a, b, c);
    }
}

// taken from textbook - Appendix 7
void tetrahedron(int count) {
    point4 v[4] = {
            vec4(0.0, 0.0, 1.0, 100.0),
            vec4(0.0, 0.942809, -0.333333, 100.0),
            vec4(-0.816497, -0.471405, -0.333333, 100.0),
            vec4(0.816497, -0.471405, -0.333333, 100.0)
    };

    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
}
// ---------- SPHERE END ----------

void init() {

    drawCube();

    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsCube) + sizeof(colorsCube), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsCube), pointsCube);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsCube), sizeof(colorsCube), colorsCube);

    program = InitShader("/Users/mburakaltun/CLionProjects/Comp410_Hw1/vshader.glsl",
                         "/Users/mburakaltun/CLionProjects/Comp410_Hw1/fshader.glsl");

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsCube)));

    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    glUseProgram(program);

    mat4 projection;
    projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.8, 0.8, 0.8, 1.0); // BG Color White Init...
}

void resetObjectCube() {
    isCube = true;
    IndexCube = 0;
    drawCube();

    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsCube) + sizeof(colorsCube), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsCube), pointsCube);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsCube), sizeof(colorsCube), colorsCube);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsCube)));
}

void resetObjectSphere() {
    isCube = false;
    IndexSphere = 0;
    tetrahedron(4);

    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(colorSphere), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(colorSphere), colorSphere);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsSphere)));
}

void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 model_view = (Scale(1.0, 1.0, 1.0) * Translate(location) *
                       RotateX(Theta[Xaxis]) *
                       RotateY(Theta[Yaxis]) *
                       RotateZ(Theta[Zaxis]));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

    if (isCube) {
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);
    }
    glutSwapBuffers();
}

static void Timer(int n) {

    if (location.y <= -0.8) {
        velocity = vec3(-0.008, -velocity.y * 0.75 + 0.00000001, 0.0);
    }

    if (location.y >= 0.8) {
        velocity = vec3(-0.008, -velocity.y * 0.75, 0.0);
    }

    /*if (location.x >= 1.0) {
        location = (-0.9, 0.8, 0.0);
        velocity = vec3(-0.008, -velocity.y / 2, 0.0);
    }*/

    if (location.y < -0.8) {
        location.y = -0.8;
    }

    velocity.y += (acceleration.y);

    acceleration.y = 0.001;

    location.operator-=(velocity);

    glutTimerFunc(1, Timer, 0);
    glutPostRedisplay();
}

void idle(void) {
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 033:
        case 'q':
        case 'Q':
            exit(EXIT_SUCCESS);
        case 'i':
        case 'I':
            location = vec3(-0.8, 0.8, 0.0);
            velocity = vec3(0.0, 0.01, 0.0);
        case 'h':
        case 'H':
            printf("Instructions\n");
            printf("Left Click: Open Menu\n");
            printf("i/I: init position\n");
            printf("q/Q: exit from program\n");
    }
}

void menu(int num) { // Menu Options Handled

    if (num == 0) {
        glutDestroyWindow(window);
        exit(0);

    } else if (num == 5) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    } else if (num == 6) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    } else if (num == 3) {
        colorChanger("blue");

    } else if (num == 4) {
        colorChanger("red");

    } else if (num == 7) {
        colorChanger("green");

    } else if (num == 8) {
        colorChanger("yellow");

    } else if (num == 9) {
        colorChanger("white");
        glClearColor(0.1, 0.1, 0.1, 1.0);

    } else if (num == 10) {
        colorChanger("black");
        glClearColor(0.8, 0.8, 0.8, 1.0);

    } else if (num == 11) {
        resetObjectCube();
        glutPostRedisplay();

    } else if (num == 2) {
        resetObjectSphere();
        glutPostRedisplay();

    } else {
        valueMenu = num;
    }
    glutPostRedisplay();
}

void colorChanger(std::string k) {

    if (k == "blue") {

        vec4 blue(0.0, 0.0, 1.0, 1.0);
        currentColor = blue;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }

    } else if (k == "red") {

        vec4 red(1.0, 0.0, 0.0, 1.0);
        currentColor = red;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }

    } else if (k == "yellow") {

        vec4 yellow(1.0, 1.0, 0.0, 1.0);
        currentColor = yellow;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }

    } else if (k == "green") {

        vec4 green(0.0, 1.0, 0.0, 1.0);
        currentColor = green;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }


    } else if (k == "white") {

        vec4 white(1.0, 1.0, 1.0, 1.0);
        currentColor = white;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }

    } else if (k == "black") {

        vec4 black(0.0, 0.0, 0.0, 1.0);
        currentColor = black;

        if (isCube) {
            resetObjectCube();
        } else {
            resetObjectSphere();
        }
    }
}

void createMenu(void) {
    colorMenu = glutCreateMenu(menu);
    glutAddMenuEntry("wireframe", 5);
    glutAddMenuEntry("solid", 6);
    drawingModeMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Blue", 3);
    glutAddMenuEntry("Red", 4);
    glutAddMenuEntry("Green", 7);
    glutAddMenuEntry("Yellow", 8);
    glutAddMenuEntry("White", 9);
    glutAddMenuEntry("Black", 10);
    objectTypeMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Sphere", 2);
    glutAddMenuEntry("Square", 11);
    menu_id = glutCreateMenu(menu);
    glutAddSubMenu("Object type", objectTypeMenu);
    glutAddSubMenu("Drawing mode", colorMenu);
    glutAddSubMenu("Color", drawingModeMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// taken from textbook - Appendix 7
void reshape(int width, int height) {

    glViewport(0, 0, width, height);
    GLfloat left = -1.0, right = 1.0;
    GLfloat top = 1.0, bottom = -1.0;
    GLfloat zNear = -40.0, zFar = 40.0;
    GLfloat aspect = GLfloat(width) / height;

    if (aspect > 1.0) {

        left *= aspect;
        right *= aspect;

    } else {
        top /= aspect;
        bottom /= aspect;
    }

    mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

int main(int argc, char **argv) { 

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Bouncing Ball");
    init();
    createMenu();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    Timer(1);
    glutMainLoop();
    return 0;
}