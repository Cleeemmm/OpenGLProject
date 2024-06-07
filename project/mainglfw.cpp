#include "common/GLShader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
using namespace std;

GLShader g_BasicShader;
GLuint VBO;
GLuint IBO;

struct Vertex2D {
    float x;
    float y;
};

struct Vertex3D {
    float a;
    float b;
    float c;
};

struct Vertex {
    Vertex2D position;
    Vertex3D color;
};

static const Vertex vertices[] = {
    {{0.5f,0.5f}, {1.f, 0.f, 0.f}},
    {{0.5f,-0.5f}, {0.f, 1.f, 0.f}},
    {{-0.5f,-0.5f}, {0.f, 0.f, 1.f}},
    {{-0.5f,0.5f}, {1.f, 0.f, 0.f}}
};

unsigned int indices[] = {
    0,1,3,
    1,2,3
};

GLfloat* inverseMatrix(GLfloat* matrix1, int size, int length) {

    GLfloat* result = new GLfloat[size] ;

    /*
    std::cout << "initial : " ;
    for (int i  = 0 ; i < size ; i++) {
        std::cout << matrix1[i] << " " ;
    }
    std::cout << std::endl;*/

    int modulo = 0 ;
    int indice = 0 ;
    int i = 0 ;
    while (indice < size){
        if (i%length == modulo) {
            result[indice] = matrix1[i];
            indice++ ;
        }
        i++;
        if(indice == length*(modulo+1)) {
            modulo++;
            i=0;
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

    int size = sizeof(matrix1)/sizeof(GLfloat);
    int length = sqrt(size);

    GLfloat sum = 0 ;
    GLfloat* result  = new GLfloat[size];
    int indice = 0 ;
    int m2value = 0 ;
    for (int j = 0 ; j < size ; j++) {
        for (int i = 0; i < length ; i++) {
            sum += matrix2[m2value]*matrix1[i*length + j/length] ;
            m2value++;
            if (m2value >= size){
                m2value = 0 ;
            }
        }
        result[indice] = sum ;
        sum = 0 ;
        indice++ ;
        if (indice >= size) {break;}
    }   
    result = inverseMatrix(result,size,length);
    return result;
}
 

bool Initialise()
{

/*
    Vertex v1;
    v1.position = {-0.5f,-0.5f};
    v1.color = {1.f,0.f,0.f};

    Vertex v2;
    v2.position = {0.5f,-0.5f};
    v2.color = {0.f,1.f,0.f};

    Vertex v3;
    v3.position = {0.f,0.5f};
    v3.color = {0.f,0.f,1.f};

    static const Vertex triangle[] = {
        v1,
        v2,
        v3
    };
*/
    multiplyMatrix();

    g_BasicShader.LoadVertexShader("basic.vs");
    g_BasicShader.LoadFragmentShader("basic.fs");
    g_BasicShader.Create();
    // cette fonction est spécifique à Windows et permet d’activer (1) ou non (0)
    // la synchronization vertical. Elle necessite l’include wglew.h
    #ifdef WIN32
    //wglSwapIntervalEXT(1);
    #endif

        glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData alloue et transfert octets issus du tableau triangle
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, vertices, GL_STATIC_DRAW);
    // je recommande de reinitialiser les etats a la fin pour eviter les effets de bord
        
    glGenBuffers(1,&IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
        
    glBindVertexArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    return true;
}

void Terminate() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glDeleteBuffers(1, &IBO);

    g_BasicShader.Destroy();
}



void Render(int width, int height)
{


    // etape a. A vous de recuperer/passer les variables width/height
    glViewport(0, 0, width, height);
    // etape b. Notez que glClearColor est un etat, donc persistant
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // etape c. on specifie le shader program a utiliser
    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);
    // etape d.

    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(float) * 5, (void*)offsetof(Vertex,position));
    
    
    int loc_color = glGetAttribLocation(basicProgram, "a_color");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(loc_color);
    glVertexAttribPointer(loc_color, 3, GL_FLOAT, false, sizeof(float) * 5, (void*)offsetof(Vertex,color));

    glEnableVertexAttribArray(0); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IBO);
    
    glDrawElements(GL_TRIANGLES,sizeof(indices)/sizeof(indices[0]),GL_UNSIGNED_INT,NULL);

    // etape e.
    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    // on suppose que la phase d’echange des buffers front et back
    // le « swap buffers » est effectuee juste apres
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

    glewInit();

    Initialise();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);
        Render(width,height);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    Terminate();
    glfwTerminate();
    return 0;
}