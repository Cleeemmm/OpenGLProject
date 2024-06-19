#include "common/GLShader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#define TINYOBJLOADER_IMPLEMENTATION
#include "library/tinyobjloader-release/tiny_obj_loader.h"

const int STEP = 5 ;

GLShader g_BasicShader;
GLuint VAO;
GLuint VBO;
GLuint IBO;
GLuint VAOObject;
GLuint VBOObject;

int matriceViewLocation;
int matriceProjectionLocation;
int matriceWorldLocation;

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct vec4 {
    float x, y, z, w;

    float& operator[](int index) {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        default: throw std::out_of_range("Index out of range");
        }
    }

    const float& operator[](int index) const {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        default: throw std::out_of_range("Index out of range");
        }
    }


};

struct Camera {
    vec3 position = { 0,0,0 };
    vec3 direction = { 0,0,-1 };
};

Camera camera;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture;
};


/*
struct std::vector<Vertex> vertices = {
    { { 0.0f, 0.5f,0.0f },{1.f,0.f},{1.f,0.f,0.f}},
    { { -0.5f, -0.5f,0.0f },{1.f,0.f},{0.f,1.f,0.f}},
    { { 0.5f, -0.5f,0.0f },{1.f,0.f},{0.f,0.f,1.f}},
};*/


static const unsigned short indices[] = {
    0, 1, 2
};

int sizeVertices;


struct mat4
{
    vec4 matrix[4];

    vec4& operator[](int index) {
        if (index < 0 || index >= 4)
            throw std::out_of_range("Index out of range");
        return matrix[index];
    }

    const vec4& operator[](int index) const {
        if (index < 0 || index >= 4)
            throw std::out_of_range("Index out of range");
        return matrix[index];
    }

    mat4 operator*(const mat4& other) const {
        mat4 result;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result[i][j] += matrix[k][i] * other[j][k];
                }
            }
        }

        return result;
    }
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


void LookAt(vec3 position, vec3 target, vec3 up) {
    vec3 forward = { 0.0f, 0.0f, 0.0f };

    vec3 right = { 0.0f, 0.0f, 0.0f };
    vec3 upCorrected = { 0.0f, 0.0f, 0.0f };

    forward = normalize({ target.x - position.x, target.y - position.y, target.z - position.z });
    right = vectProduct(up, forward);
    upCorrected = vectProduct(forward, right);

    vec3 scalResults = { -scalProduct(position, right), -scalProduct(position, upCorrected), -scalProduct(position, forward) };

    mat4 matriceView = { vec4{-forward.x, right.x, upCorrected.x, 0.0f} ,
                        vec4{ -forward.y, right.y, upCorrected.y, 0.0f},
                        vec4{-forward.z, right.z, upCorrected.z, 0.0f },
                        vec4{scalResults.x, scalResults.y, scalResults.z, 1 }
};

    glUniformMatrix4fv(matriceViewLocation, 1, false, (float*) & matriceView);
}


void projectionMatrix(GLfloat width, GLfloat height, GLfloat far, GLfloat near) {

    GLfloat aspect_ratio = width / height;
    GLfloat fov = 3.14f / 4.0f;

    mat4 matriceProjection = {
                            vec4{(1.0f / tanf(fov / 2.0f)) / aspect_ratio , 0.0f, 0.0f, 0.0f },
                            vec4{0.0f, 1 / tanf(fov / 2.0f), 0.0f, 0.0f},
                            vec4{0.0f, 0.0f, -(far + near) / (far - near), -1.0f },
                            vec4{0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f }
                            };

    glUniformMatrix4fv(matriceProjectionLocation, 1, false, (float*) &matriceProjection);
}


void worldMatrix() {
    mat4 worldMatrix;
    float time = (float)glfwGetTime();

    mat4 translate = { vec4{1.f, 0.f, 0.f, 0.f },
                        vec4{0.f, 1.f, 0.f, 0.f },
                        vec4{0.f, 0.f, 1.f, 0.f },
                        vec4{0.f, 0.f, -5.f, 1.f }
    };

    mat4 scale = { vec4{1.f, 0.f, 0.f, 0.f },
                    vec4{0.f, 1.f, 0.f, 0.f },
                    vec4{0.f, 0.f, 1.f, 0.f },
                    vec4{0.f, 0.f, 0.0f, 1.f }
    };

    mat4 rotateZ = { vec4{cosf(2 * time), sinf(2 * time), 0.f, 0.f},
                    vec4{-sinf(2 * time), cosf(2 * time), 0.f, 0.f},
                    vec4{0.f, 0.f, 1.f, 0.f },
                    vec4{0.f, 0.f, -5.f, 1.f }
    };

    worldMatrix = translate * scale;
    worldMatrix = worldMatrix * rotateZ;

    glUniformMatrix4fv(matriceWorldLocation, 1, false, (float*) &worldMatrix);
}

bool Initialise()
{   
    //reading my object :
    std::string modelPath = "./object/suzanne.obj";

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;
    tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, modelPath.c_str());

    std::vector<Vertex> vertices;

    int size = 0;
    for (int i = 0; i < shapes.size(); i++) {
        tinyobj::shape_t& shape = shapes[i];
        tinyobj::mesh_t& mesh = shape.mesh;
        size += mesh.indices.size();
        for (int j = 0; j < mesh.indices.size(); j++) {
            tinyobj::index_t i = mesh.indices[j];
            vec3 normal = { 0.0f, 0.0f, 0.0f };
            vec2 texCoord = { 0.0f, 0.0f };
            vec3 position = {
                attributes.vertices[i.vertex_index * 3],
                attributes.vertices[i.vertex_index * 3 + 1],
                attributes.vertices[i.vertex_index * 3 + 2]
            };
            if (i.normal_index >= 0)
                normal = {
                    attributes.normals[i.normal_index * 3],
                    attributes.normals[i.normal_index * 3 + 1],
                    attributes.normals[i.normal_index * 3 + 2]
            };
            if (i.texcoord_index >= 0)
                texCoord = {
                    attributes.texcoords[i.texcoord_index * 2],
                    attributes.texcoords[i.texcoord_index * 2 + 1],
            };
            Vertex vert = { position, normal, texCoord };
            vertices.push_back(vert);
        }
    }

    g_BasicShader.LoadVertexShader("Basic.vs");
    g_BasicShader.LoadFragmentShader("Basic.fs");
    g_BasicShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    /*
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices, GL_STATIC_DRAW);
    */
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
    sizeVertices = size * 3;

    /*
    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, false, sizeof(Vertex), 0);

    int color = glGetAttribLocation(basicProgram, "a_color");
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));

    int normal = glGetAttribLocation(basicProgram, "a_normal");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    */
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    matriceViewLocation = glGetUniformLocation(basicProgram, "v_viewMatrix");
    matriceProjectionLocation = glGetUniformLocation(basicProgram, "v_projectionMatrix");
    matriceWorldLocation = glGetUniformLocation(basicProgram, "v_worldMatrix");

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
    
    LookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    projectionMatrix(width, height, 500.0f, 0.1f);
    worldMatrix();

    // etape b. Notez que glClearColor est un etat, donc persistant
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, sizeVertices);
    //glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_SHORT, NULL);
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