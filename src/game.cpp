#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for glm::to_string
#include <glm/mat4x4.hpp>

#include "std/matrices.h"
#include "std/utils.h"

#include "callbacks.hpp"
#include "camera.hpp"
#include "collisions.hpp"
#include "game.hpp"
#include "globals.hpp"
#include "obj_loader.hpp"
#include "perlin_noise.hpp"
#include "shaders_provider.hpp"
#include "texture.hpp"
#include "window_provider.hpp"
#include "bezier.hpp"

#define BEZIER_SPEED 0.1

GLuint BuildTriangles();

int game() {
    WindowProvider windowProvider = WindowProvider(800, 800, "MinecraftGL");

    GLFWwindow *window = windowProvider.initWindow(
        ErrorCallback, KeyCallback, MouseButtonCallback, CursorPosCallback,
        ScrollCallback, FramebufferSizeCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    ShadersProvider shaderProvider = ShadersProvider();

    GLuint programId = shaderProvider.loadShadersFromFiles();

    ObjModel cowModel("assets/cow.obj");
    cowModel.ComputeNormals();
    cowModel.BuildTrianglesAndAddToVirtualScene();

    ObjModel leafModel("assets/leaf.obj");
    leafModel.ComputeNormals();
    leafModel.BuildTrianglesAndAddToVirtualScene();

    // Construímos a representação de um triângulo
    GLuint vertex_array_object_id = BuildTriangles();

    GLint model_uniform = glGetUniformLocation(programId, "model"); // Variável da matriz "model"
    GLint view_uniform = glGetUniformLocation(programId, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform = glGetUniformLocation(programId, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint object_id_uniform = glGetUniformLocation(programId, "object_id"); // Variável booleana em shader_vertex.glsl
    GLint sampler_uniform = glGetUniformLocation(programId, "sampler");

    PerlinNoise pn = PerlinNoise(MAP_SIZE, MAP_SIZE);

    vector<vector<float>> map = pn.generateNoise(6);
    int init = -MAP_SIZE / 2;

    glEnable(GL_DEPTH_TEST);

    Texture skyBack = Texture("assets/sky_back.png", GL_TEXTURE_2D);
    skyBack.load();
    Texture skyDown = Texture("assets/sky_down.png", GL_TEXTURE_2D);
    skyDown.load();
    Texture skyUp = Texture("assets/sky_up.png", GL_TEXTURE_2D);
    skyUp.load();
    Texture skyRight = Texture("assets/sky_right.png", GL_TEXTURE_2D);
    skyRight.load();
    Texture skyLeft = Texture("assets/sky_left.png", GL_TEXTURE_2D);
    skyLeft.load();
    Texture skyFront = Texture("assets/sky_front.png", GL_TEXTURE_2D);
    skyFront.load();

    Texture grassSideTexture = Texture("assets/grass_side.png", GL_TEXTURE_2D);
    grassSideTexture.load();

    Texture grassTopTexture = Texture("assets/grass_top.jpg", GL_TEXTURE_2D);
    grassTopTexture.load();

    Texture dirtTexture = Texture("assets/dirt.png", GL_TEXTURE_2D);
    dirtTexture.load();

    BezierCurve bezier = BezierCurve();

    std::chrono::time_point<std::chrono::high_resolution_clock> elapsedTime, timeSinceLastFrame;

    // BEZIER VARIABLES

    float c = 0.0f;

    glm::vec4 p0 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 p1 = glm::vec4(10.0f, -4.0f, 0.0f, 0.0f);
    glm::vec4 c0 = glm::vec4(-10.0f, -6.0f, 0.0f, 0.0f);
    glm::vec4 c1 = glm::vec4(0.0f, -10.0f, 0.0f, 0.0f);

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        elapsedTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<double, std::milli>(elapsedTime - timeSinceLastFrame).count() / 1000;
        timeSinceLastFrame = elapsedTime;

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);

        glBindVertexArray(vertex_array_object_id);

        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection();

        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform1i(sampler_uniform, 0);

        glm::mat4 model;

        for (int i = 0; i < MAP_SIZE; ++i) {
            for (int j = 0; j < MAP_SIZE; ++j) {
                model = Matrix_Translate(init + i * 1.0f, map[i][j] - 20, init + j * 1.0f);

                mapData[i][j] = model[3];

                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

                grassSideTexture.bind(GL_TEXTURE0);

                glDrawElements(g_VirtualScene["cube_sides"].renderingMode,
                            g_VirtualScene["cube_sides"].numIndexes, GL_UNSIGNED_INT,
                            (void *)g_VirtualScene["cube_sides"].firstIndex);

                grassTopTexture.bind(GL_TEXTURE0);

                glDrawElements(g_VirtualScene["cube_top"].renderingMode,
                            g_VirtualScene["cube_top"].numIndexes, GL_UNSIGNED_INT,
                            (void *)g_VirtualScene["cube_top"].firstIndex);

                dirtTexture.bind(GL_TEXTURE0);

                glDrawElements(g_VirtualScene["cube_base"].renderingMode,
                            g_VirtualScene["cube_base"].numIndexes, GL_UNSIGNED_INT,
                            (void *)g_VirtualScene["cube_base"].firstIndex);
            }
        }

        #define COW 4
        // Define the initial position and the speed of the model
        glm::vec3 initialPosition = glm::vec3(-2.0f, 0.0f, -2.0f);
        float speed = 5.0f;

        // Define the elapsed time since the start of the program
        float time = glfwGetTime();

        // Calculate the new position of the model based on the elapsed time
        if (!collideCowWithMap(cowPosition, mapData)) {
            cowPosition = initialPosition + glm::vec3(0.0f, -speed * time, 0.0f);
        }

        model = Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z) * Matrix_Rotate_Y(cowRotate.y);

        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, COW);
        cowModel.DrawVirtualObject("the_cow");

        // BEZIER

        if (c > 1.0f) c = 0.0f;
        
        glm::vec4 point = bezier.calculate(p0, p1, c0, c1, c);

        c += BEZIER_SPEED * deltaTime;

        model = Matrix_Identity() * Matrix_Translate(point[0], point[1], 0);

        #define LEAF 5

        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, LEAF);
        leafModel.DrawVirtualObject("the_leaf");

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

GLuint BuildTriangles() {
    GLfloat model_coefficients[] = {
        // front face
        -0.5f, 0.5f, 0.5f, 1.0f,  // posição do vértice 0
        -0.5f, -0.5f, 0.5f, 1.0f, // posição do vértice 1
        0.5f, -0.5f, 0.5f, 1.0f,  // posição do vértice 2
        0.5f, 0.5f, 0.5f, 1.0f,   // posição do vértice 3

        // right face
        0.5f, 0.5f, 0.5f, 1.0f,   // posição do vértice 4 (3)
        0.5f, -0.5f, 0.5f, 1.0f,  // posição do vértice 5 (2)
        0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 6
        0.5f, 0.5f, -0.5f, 1.0f,  // posição do vértice 7

        // back face
        0.5f, 0.5f, -0.5f, 1.0f,   // posição do vértice 8 (7)
        0.5f, -0.5f, -0.5f, 1.0f,  // posição do vértice 9 (6)
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 10
        -0.5f, 0.5f, -0.5f, 1.0f,  // posição do vértice 11

        // left face
        -0.5f, 0.5f, -0.5f, 1.0f,  // posição do vértice 12 (11)
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 13 (10)
        -0.5f, -0.5f, 0.5f, 1.0f,  // posição do vértice 14 (1)
        -0.5f, 0.5f, 0.5f, 1.0f,   // posição do vértice 15 (0)

        // top face
        -0.5f, 0.5f, -0.5f, 1.0f, // posição do vértice 16
        -0.5f, 0.5f, 0.5f, 1.0f,  // posição do vértice 17
        0.5f, 0.5f, 0.5f, 1.0f,   // posição do vértice 18
        0.5f, 0.5f, -0.5f, 1.0f,  // posição do vértice 19

        // bottom face
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 16
        -0.5f, -0.5f, 0.5f, 1.0f,  // posição do vértice 17
        0.5f, -0.5f, 0.5f, 1.0f,   // posição do vértice 18
        0.5f, -0.5f, -0.5f, 1.0f,  // posição do vértice 19
    };

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);
    GLuint location = 0;
    GLint number_of_dimensions = 4;
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat normal_coefficients[] = {
        // front face
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,

        // right face
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,

        // back face
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,

        // left face
        -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,

        // top face
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,

        // bottom face
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
    };

    GLuint VBO_normal_coefficients_id;
    glGenBuffers(1, &VBO_normal_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normal_coefficients), normal_coefficients);
    location = 1;
    number_of_dimensions = 4;
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat texture_coefficients[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // front face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // right face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // back face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // bottom face
    };

    GLuint VBO_texture_coefficients_id;
    glGenBuffers(1, &VBO_texture_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texture_coefficients), texture_coefficients);
    location = 2;
    number_of_dimensions = 2;
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint indices[] = {0,  1,  2, // triângulo 1
                        0,  2,  3, // triângulo 2
                        4,  5,  6,  
                        4,  6,  7,  
                        8,  9,  10, 
                        8, 10, 11, 
                        12, 13, 14, 
                        12, 14, 15, 
                        16, 17, 18, 
                        16, 18, 19, 
                        20, 21, 22, 
                        20, 22, 23};

    SceneObject cube_sides;
    cube_sides.name = "Lados do cubo";
    cube_sides.firstIndex = 0; 
    cube_sides.numIndexes = 24;
    cube_sides.renderingMode = GL_TRIANGLES; 

    g_VirtualScene["cube_sides"] = cube_sides;

    SceneObject cube_top;
    cube_top.name = "Topo do cubo";
    cube_top.firstIndex = 24 * sizeof(unsigned int);
    cube_top.numIndexes = 6;
    cube_top.renderingMode = GL_TRIANGLES;

    g_VirtualScene["cube_top"] = cube_top;

    SceneObject cube_base;
    cube_base.name = "Base do cubo";
    cube_base.firstIndex = 30 * sizeof(unsigned int);
    cube_base.numIndexes = 6;
    cube_base.renderingMode = GL_TRIANGLES;

    g_VirtualScene["cube_base"] = cube_base;

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    glBindVertexArray(0);

    return vertex_array_object_id;
}
