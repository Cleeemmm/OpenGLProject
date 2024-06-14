#include "common/GLShader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#define STEP = 5 ;

GLShader g_BasicShader;
GLuint VAO;
GLuint VBO;
GLuint IBO;

int matriceViewLocation;

struct Camera {
    vec3 position = { 0,0,0 };
    vec3 direction = { 0,0,-1 };
};

Camera camera;

struct Vertex {
    float position[2];
    float color[3];
    float normal[3];
};

static const Vertex vertices[] = {
    { { 0.0f, 0.5f },{1.f,0.f,0.f},{1.f,0.f,0.f}},
    { { -0.5f, -0.5f },{1.f,0.f,0.f},{0.f,1.f,0.f}},
    { { 0.5f, -0.5f },{1.f,0.f,0.f},{0.f,0.f,1.f}},
};

static const unsigned short indices[] = {
    0, 1, 2
};

struct vec3 { float x, y, z; };
struct vec4 { float x, y, z, w; };

struct mat4 {
    vec4 col1;
    vec4 col2;
    vec4 col3;
    vec4 col4;
};

vec3 normalize(vec3 vector) {
    float length = 0;
    length = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
    length = sqrt(length);

    vec3 result = { 0,0,0 };
    result.x = vector.x / length;
    result.y = vector.y / length;
    result.z = vector.z / length;
    
    return result;
}

vec3 vectProduct(vec3 vector1, vec3 vector2) {
    vec3 result = { 0,0,0 };
    result.x = vector1.y * vector2.z - vector1.z * vector2.y;
    result.y = - vector1.x * vector2.z + vector1.z * vector2.x;
    result.z = vector1.x * vector2.y - vector1.y * vector2.x;
    return result;
}

float scalProduct(vec3 vector1, vec3 vector2) {
    float result = 0;
    result = vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
    return result;
}

GLfloat* mat4toFloat(mat4* matrix) {
    return &matrix->col1.x;
}

void LookAt(vec3 position, vec3 target, vec3 up) {
    vec3 forward = { 0.0f, 0.0f, 0.0f };

    vec3 right = { 0.0f, 0.0f, 0.0f };
    vec3 upCorrected = { 0.0f, 0.0f, 0.0f };

    forward = normalize({ target.x - position.x, target.y - position.y, target.z - position.z });
    right = vectProduct(up, forward);
    upCorrected = vectProduct(forward, right);

    vec3 scalResults = { -scalProduct(position, right), -scalProduct(position, upCorrected), -scalProduct(position, forward) };

    mat4 matriceView;
    matriceView.col1 = { -forward.x, right.x, upCorrected.x, 0 };
    matriceView.col2 = { -forward.y, right.y, upCorrected.y, 0 };
    matriceView.col3 = { -forward.z, right.z, upCorrected.z, 0 };
    matriceView.col4 = {scalResults.x, scalResults.y, scalResults.z, 1};

    glUniformMatrix4fv(matriceViewLocation, 1, false, mat4toFloat(&matriceView));
}

GLfloat* inverseMatrix(GLfloat* matrix1, int size, int length) {

    GLfloat* result = new GLfloat[size];

    /*
    std::cout << "initial : " ;
    for (int i  = 0 ; i < size ; i++) {
        std::cout << matrix1[i] << " " ;
    }
    std::cout << std::endl;*/

    int modulo = 0;
    int indice = 0;
    int i = 0;
    while (indice < size) {
        if (i % length == modulo) {
            result[indice] = matrix1[i];
            indice++;
        }
        i++;
        if (indice == length * (modulo + 1)) {
            modulo++;
            i = 0;
        }
    }

    /*
    std::cout << "final : ";
    for (int i  = 0 ; i < size ; i++) {
        std::cout << result[i] << " " ;
    }
    std::cout << std::endl;*/

    return result;
}

GLfloat* multiplyMatrix(GLfloat* matrix1, GLfloat* matrix2) {

    int size = sizeof(matrix1) / sizeof(GLfloat);
    int length = sqrt(size);

    GLfloat sum = 0;
    GLfloat* result = new GLfloat[size];
    int indice = 0;
    int m2value = 0;
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < length; i++) {
            sum += matrix2[m2value] * matrix1[i * length + j / length];
            m2value++;
            if (m2value >= size) {
                m2value = 0;
            }
        }
        result[indice] = sum;
        sum = 0;
        indice++;
        if (indice >= size) { break; }
    }
    result = inverseMatrix(result, size, length);
    return result;
}

bool Initialise()
{
    g_BasicShader.LoadVertexShader("Basic.vs");
    g_BasicShader.LoadFragmentShader("Basic.fs");
    g_BasicShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, vertices, GL_STATIC_DRAW);

    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(Vertex), 0);

    int color = glGetAttribLocation(basicProgram, "a_color");
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    matriceViewLocation = glGetUniformLocation(basicProgram, "v_viewMatrix");

    return true;
}

void Terminate() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_BasicShader.Destroy();
}

void Render(int width, int height)
{
    // etape a. A vous de recuperer/passer les variables width/height
    glViewport(0, 0, width, height);
    LookAt({ 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 });
    // etape b. Notez que glClearColor est un etat, donc persistant
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_SHORT, NULL);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        std::cout << "right" << std::endl;
        camera.position.x += STEP;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        std::cout << "left" << std::endl;
        camera.position.y -= STEP;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        std::cout << "down" << std::endl;
        camera.position.y -= STEP;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        std::cout << "up" << std::endl;
        camera.position.y += STEP;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        std::cout << "forward" << std::endl;
        camera.position.z -= STEP;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        std::cout << "backward" << std::endl;
        camera.position.z += STEP;
    }
}


int main(void)
{

    GLFWwindow* window;

    int width = 640;
    int height = 480;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Test1", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glewInit();

    Initialise();

    //stop it if user close the window
    while (1) { 
        
        if (glfwWindowShouldClose(window)) { break; }
        // Render :
        Render(width, height);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    Terminate();
    glfwTerminate();

    return 0;
}