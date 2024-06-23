//
// Created by bastien on 28/05/24.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "common/GLShader.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"


struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct vec4 { float x, y, z, w; };

struct Vertex
{
    vec2 position;
    vec3 color;
    vec3 normal;
    vec2 texture;
};

struct Application {
    GLShader g_BasicShader;
    GLuint VBO;
    GLuint IBO;
    GLuint VAO;
    GLuint texID;

    bool Initialize()
    {
        g_BasicShader.LoadVertexShader("Basic.vs.glsl");
        g_BasicShader.LoadFragmentShader("Basic.fs.glsl");
        g_BasicShader.Create();

        auto basicProgram = g_BasicShader.GetProgram();
        glUseProgram(basicProgram);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        const std::vector<Vertex> triangles =
        {
                { { 0.0f, 0.5f },{0.f,0.f,0.8f}, {0.f, -0.f, 1.f},{0.f,0.f}},    // sommet 0
                { { -0.5f, -0.5f }, {0.f,0.f,0.8f}, {-0.3f, 0.f, 0.f},{1.f,0.f}},  // sommet 1
                { { 0.5f, -0.5f },{0.f,0.f,0.8f}, {0.3f, 0.f, 0.f},{0.5f,1.f}},   // sommet 2
        };

        unsigned short indices[] = {
                0,1,2
        };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, triangles.data(), GL_STATIC_DRAW);



        int loc_position = glGetAttribLocation(basicProgram, "a_position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(Vertex), 0);

        int color = glGetAttribLocation(basicProgram, "a_color");
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));

        int normal = glGetAttribLocation(basicProgram, "a_N");
        glEnableVertexAttribArray(normal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        GLfloat L[3] = {0, 0, -1.0};
        const int u_L = glGetUniformLocation(basicProgram, "u_L");
        glUniform3fv(u_L, 1, L);

        GLfloat Id[3] = {1.0, 1.0, 1.0};
        const int u_Id = glGetUniformLocation(basicProgram, "u_Id");
        glUniform3fv(u_Id, 1, Id);




        int texture_loc = glGetAttribLocation(basicProgram, "a_texcoords");
        glEnableVertexAttribArray(texture_loc);
        glVertexAttribPointer(texture_loc, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texture));

        auto locationTexture = glGetUniformLocation(basicProgram, "u_sampler");
        glUniform1i(locationTexture, 1);

        glGenTextures(1, &texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texID);

        int w, h;
        uint8_t *data = stbi_load("./stb-master/data/map_01.png", &w, &h, nullptr, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }






        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*3, indices, GL_STATIC_DRAW);


        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_TEXTURE_2D, 0);


        // cette fonction est spécifique à Windows et permet d’activer (1) ou non (0)
        // la synchronization vertical. Elle necessite l’include wglew.h
        #ifdef WIN32
            glfwSwapInterval(1);
        #endif
        return true;
    }

    void Terminate() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &texID);
        g_BasicShader.Destroy();
    }

    void Render() {
        // etape a. A vous de recuperer/passer les variables width/height
        glViewport(0, 0, 640, 480);
        // etape b. Notez que glClearColor est un etat, donc persistant
        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        // etape c. on specifie le shader program a utiliser

        // etape d.



        // glBindTexture(GL_TEXTURE_2D, texID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT /*ou _INT*/, 0 /*nullptr*/);






        // etape e.
        // etape f. dessin de triangles dont la definition provient d’un tableau
        // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // on suppose que la phase d’echange des buffers front et back
        // le « swap buffers » est effectuee juste apres


    }

};







int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Exemple OpenGL 2.+", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ICI !
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
    }

    Application app;
    app.Initialize();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        app.Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    app.Terminate();

    glfwTerminate();
    return 0;
}