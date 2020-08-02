#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <chrono>

#include <FastNoise.h>
#include <CubeGrid.h>
#include <CellGrid.h>
#include <Mesh.h>
#include <Camera.h>

// 3D scalar grid size

#define GRID_WIDTH      50
#define GRID_HEIGHT     50
#define GRID_DEPTH      50

// noise function parameters

#define OCTAVES         3
#define LACUNARITY      2.f
#define PERSISTANCE     0.5f
#define NOISE_SCALE     3.f

// cube grid parameters

#define SURFACE_LEVEL   0.5f
#define CUBE_SIZE       0.1f
#define MIN_REGION_SIZE 1000

// camera parameters

#define CAM_ROTATION_SPEED  0.5f
#define CAM_ZOOM_SPEED      0.3f
#define CAM_MIN_DIST        1.f
#define CAM_MAX_DIST        15.f


using namespace std;


// generator class instances
static FastNoise noise;
static CellGrid cellGrid;
static CubeGrid cubeGrid;
static Mesh mesh;

// camera
static Camera cam;

// vertex array elements params
static GLuint vbo, ibo;
static unsigned int vboSize, iboSize;
static unsigned int normalsOffset;
static unsigned int iboCount;

static float frameTime;
static bool drawAxes, drawWireBox;

// generator functions
// generate the mesh by marching cubes
static void generateMap();
static void generateBuffers();

// GLFW event callbacks
static void onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods);
static void onCursorPosition(GLFWwindow *window, double x, double y);
static void onMouseButton(GLFWwindow *window, int button, int action, int mods);
static void onScrollRoll(GLFWwindow *window, double xoff, double yoff);

// mesh material and lighting
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 0.7f, 0.3f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
    // GL window setup

    if(!glfwInit()){
        glfwTerminate();
        return 0;
    }

    GLFWwindow *window;

    window = glfwCreateWindow(600, 400, "Marching Cubes", NULL, NULL);

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, onKeyPressed);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, onCursorPosition);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetScrollCallback(window, onScrollRoll);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return 0;
    }

    glClearColor(0, 0, 0, 1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    // generate first map

    // initialize random seed
    srand(time(0));
    // use simplex noise
    noise.SetNoiseType(FastNoise::Simplex);
    // create grid of cells (3D scalar field)
    cellGrid = CellGrid(GRID_WIDTH, GRID_HEIGHT, GRID_DEPTH);

    generateMap();
    generateBuffers();

    int winWidth, winHeight;
    float sx, sy, sz;
    float ax, ay, az;

    // get box size for wire box, size is constant
    sx = mesh.dimX / 2.f;
    sy = mesh.dimY / 2.f;
    sz = mesh.dimZ / 2.f;

    ax = -sx * 1.1f;
    ay = -sy * 1.1f;
    az = -sz * 1.1f;

    // camera setup : place it at some distance from the center based on mesh size
    cam = Camera(vec3d(0, 0, -mesh.dimZ * 2.f), CAM_ROTATION_SPEED, CAM_ZOOM_SPEED);

    drawWireBox = true;
    drawAxes = false;

    while(!glfwWindowShouldClose(window))
    {
        // rendering loop

        // start recording the frame rendering time
        auto frameStartTime = chrono::high_resolution_clock::now();

        glfwGetWindowSize(window, &winWidth, &winHeight );
        winHeight = winHeight > 0 ? winHeight : 1;

        glViewport( 0, 0, winWidth, winHeight );

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(65.0f, (GLfloat)winWidth/(GLfloat)winHeight, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // recalculate camera coordinates
        cam.update();

        gluLookAt(cam.pos.x, cam.pos.y, cam.pos.z,
                  0.f, 0.f, 0.f,
                  0.f, 1.f, 0.f);

        // draw the mesh from the buffers

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glColor3f(1.f, 1.f, 1.f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (void*)(0));

        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, (void*)(normalsOffset));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, iboCount, GL_UNSIGNED_INT, (void*)(0));

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);

        // draw wire box

        glPointSize(1.f);

        if(drawWireBox){
            glColor3f(1.f, 1.f, 1.f);
            glBegin(GL_LINE_LOOP);
                glVertex3f(-sx, -sy, -sz);
                glVertex3f( sx, -sy, -sz);
                glVertex3f( sx,  sy, -sz);
                glVertex3f(-sx,  sy, -sz);
            glEnd();
            glBegin(GL_LINE_LOOP);
                glVertex3f(-sx, -sy,  sz);
                glVertex3f( sx, -sy,  sz);
                glVertex3f( sx,  sy,  sz);
                glVertex3f(-sx,  sy,  sz);
            glEnd();
            glBegin(GL_LINES);
                glVertex3f(-sx, -sy, -sz);
                glVertex3f(-sx, -sy,  sz);
                glVertex3f( sx, -sy, -sz);
                glVertex3f( sx, -sy,  sz);
                glVertex3f(-sx,  sy, -sz);
                glVertex3f(-sx,  sy,  sz);
                glVertex3f( sx,  sy, -sz);
                glVertex3f( sx,  sy,  sz);
            glEnd();
        }

        // draw axis

        if(drawAxes){
            glBegin(GL_LINES);
                glColor3f(1.f, 0.f, 0.f);
                glVertex3f(ax,     ay, az);
                glVertex3f(ax+1.f, ay, az);

                glColor3f(0.f, 1.f, 0.f);
                glVertex3f(ax, ay,     az);
                glVertex3f(ax, ay+1.f, az);

                glColor3f(0.f, 0.f, 1.f);
                glVertex3f(ax, ay, az    );
                glVertex3f(ax, ay, az+1.f);
            glEnd();
        }

        glfwSwapBuffers(window);

        glfwPollEvents();

        // stop recording frame time
        auto frameFinishTime = chrono::high_resolution_clock::now();
        auto frameDuration = chrono::duration_cast<chrono::microseconds>(frameFinishTime - frameStartTime);

        if(frameDuration.count() > 0.f)
            frameTime = frameDuration.count() / 1000000.f;
    }

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glfwTerminate();

    return EXIT_SUCCESS;
}


static void onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS){
        if(key == GLFW_KEY_SPACE){
            // clear buffers before generating a new map
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ibo);
            generateMap();
            generateBuffers();

        } else if(key == GLFW_KEY_A){
            // toggle axes drawing
            drawAxes = !drawAxes;
        } else if(key == GLFW_KEY_B){
            // toggle wire box drawing
            drawWireBox = !drawWireBox;
        }
    }
}


static void onCursorPosition(GLFWwindow *window, double x, double y)
{
    cam.updatePos((float)x, (float)y, frameTime);
}


static void onMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        cam.enableMovement();
    } else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        cam.disableMovement();
    }
}


static void onScrollRoll(GLFWwindow *window, double xoff, double yoff)
{
    cam.updateDistance((float)yoff, CAM_MIN_DIST, CAM_MAX_DIST);
}

static void generateMap()
{
    cout << "Generating new box";

    auto startTime = chrono::high_resolution_clock::now();

    // delete previous mesh data
    mesh.clear();
    // configure noise to a random seed
    noise.SetSeed(rand());
    // fill the 3D scalar field
    cellGrid.fillGrid(noise, OCTAVES, LACUNARITY, PERSISTANCE, NOISE_SCALE);
    // generate the cube grid according to that scalar field
    cubeGrid.generateGrid(cellGrid, CUBE_SIZE, SURFACE_LEVEL, MIN_REGION_SIZE);
    cubeGrid.marchCubes(mesh.vertices);

    mesh.generateMesh(cubeGrid, MIN_REGION_SIZE);

    // free memory of useless data
    cubeGrid.clear();

    auto finishTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(finishTime - startTime);

    cout << ": seed " << noise.GetSeed();
    cout << " - " << mesh.vertices.size() << " vertices, ";
    cout << mesh.triangles.size() << " triangles";
    cout << " - " << duration.count() << "ms" << endl;
}


static void generateBuffers()
{
    // generate the VBO and load the vertices and normals of the mesh inside

    float *vertices = mesh.getVertexArray();

    normalsOffset = mesh.vertices.size() * 3 * sizeof(float);
    vboSize = normalsOffset * 2;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vboSize, &(vertices[0]), GL_STATIC_DRAW);

    delete[] vertices;

    // generate the IBO and load the triangle indices inside

    unsigned int *triangles = mesh.getTriangleArray();

    iboCount = mesh.triangles.size() * 3;
    iboSize = iboCount * sizeof(unsigned int);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, &(triangles[0]), GL_STATIC_DRAW);

    delete[] triangles;
}
