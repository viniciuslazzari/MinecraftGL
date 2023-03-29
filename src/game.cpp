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
  WindowProvider windowProvider = WindowProvider(800, 800, "Teste");

  GLFWwindow *window = windowProvider.initWindow(
      ErrorCallback, KeyCallback, MouseButtonCallback, CursorPosCallback,
      ScrollCallback, FramebufferSizeCallback);

  // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
  // biblioteca GLAD.
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  ShadersProvider shaderProvider = ShadersProvider();

  // Carregamos os shaders de vértices e de fragmentos que serão utilizados
  // para renderização. Veja slides 180-200 do documento
  // Aula_03_Rendering_Pipeline_Grafico.pdf.
  GLuint programId = shaderProvider.loadShadersFromFiles();

  ObjModel cowModel("assets/cow.obj");
  cowModel.ComputeNormals();
  cowModel.BuildTrianglesAndAddToVirtualScene();

  ObjModel leafModel("assets/leaf.obj");
  leafModel.ComputeNormals();
  leafModel.BuildTrianglesAndAddToVirtualScene();

  // Construímos a representação de um triângulo
  GLuint vertex_array_object_id = BuildTriangles();

  // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
  // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
  // (GPU)! Veja arquivo "shader_vertex.glsl".
  GLint model_uniform =
      glGetUniformLocation(programId, "model"); // Variável da matriz "model"
  GLint view_uniform = glGetUniformLocation(
      programId, "view"); // Variável da matriz "view" em shader_vertex.glsl
  GLint projection_uniform = glGetUniformLocation(
      programId,
      "projection"); // Variável da matriz "projection" em shader_vertex.glsl
  GLint object_id_uniform = glGetUniformLocation(
      programId, "object_id"); // Variável booleana em shader_vertex.glsl
  GLint sampler_uniform = glGetUniformLocation(programId, "sampler");

  PerlinNoise pn = PerlinNoise(MAP_SIZE, MAP_SIZE);

  vector<vector<float>> map = pn.generateNoise(6);

  // Habilitamos o Z-buffer. Veja slides 104-116 do documento
  // Aula_09_Projecoes.pdf.
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

  std::chrono::time_point<std::chrono::high_resolution_clock> elapsedTime,
      timeSinceLastFrame;

  float c = 0.0f;

  // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
  while (!glfwWindowShouldClose(window)) {
    elapsedTime = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<double, std::milli>(elapsedTime -
                                                          timeSinceLastFrame)
                    .count() /
                1000;
    timeSinceLastFrame = elapsedTime;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e também resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(programId);

    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTriangles(). Veja
    // comentários detalhados dentro da definição de BuildTriangles().
    glBindVertexArray(vertex_array_object_id);

    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();

    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE,
                       glm::value_ptr(projection));

    glUniform1i(sampler_uniform, 0);

    int init = -MAP_SIZE / 2;

    glm::mat4 model;

    for (int i = 0; i < MAP_SIZE; ++i) {
      for (int j = 0; j < MAP_SIZE; j++) {
        model =
            Matrix_Translate(init + i * 1.0f, map[i][j] - 20, init + j * 1.0f);

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
      cowPosition.y =  -speed * time;
    }
    else
    {
    int cowX = int(cowPosition.x)-1 +  MAP_SIZE / 2;
    int cowZ = int(cowPosition.z) -1 + MAP_SIZE / 2;
    std::cout << "X: " << cowX << " Z: " << cowZ << std::endl;
    std::cout << "Y: " << mapData[cowX][cowZ].y << std::endl;
        cowPosition.y =  mapData[cowX][cowZ].y + 1.25f;
    }

    model = Matrix_Translate(initialPosition.x, initialPosition.y, initialPosition.z)
          * Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z) 
          * Matrix_Rotate_Y(cowRotate.y);

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, COW);
    cowModel.DrawVirtualObject("the_cow");

    // BEZIER

    glm::vec4 p0 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 p1 = glm::vec4(10.0f, -4.0f, 0.0f, 0.0f);
    glm::vec4 c0 = glm::vec4(-10.0f, -6.0f, 0.0f, 0.0f);
    glm::vec4 c1 = glm::vec4(0.0f, -10.0f, 0.0f, 0.0f);

    if (c > 1.0f) c = 0.0f;
    
    glm::vec4 point = bezier.calculate(p0, p1, c0, c1, c);

    c += BEZIER_SPEED * deltaTime;

    model = Matrix_Identity() * Matrix_Translate(point[0], point[1], 0);

    #define LEAF 5

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, LEAF);
    leafModel.DrawVirtualObject("the_leaf");

    // O framebuffer onde OpenGL executa as operações de renderização não
    // é o mesmo que está sendo mostrado para o usuário, caso contrário
    // seria possível ver artefatos conhecidos como "screen tearing". A
    // chamada abaixo faz a troca dos buffers, mostrando para o usuário
    // tudo que foi renderizado pelas funções acima.
    // Veja o link:
    // https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
    glfwSwapBuffers(window);

    // Verificamos com o sistema operacional se houve alguma interação do
    // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
    // definidas anteriormente usando glfwSet*Callback() serão chamadas
    // pela biblioteca GLFW.
    glfwPollEvents();
  }

  // Finalizamos o uso dos recursos do sistema operacional
  glfwTerminate();

  // Fim do programa
  return 0;
}

// Cow movement with time acceleration, cowPostion is the cow position, deltaTime is the time between frames, cowPosition
// updates using multiplication of deltaTime and speed
// movement is the direction of the cow the cow looks at




// Constrói triângulos para futura renderização
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

  // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
  // coloridas do cubo.
  SceneObject cube_sides;
  cube_sides.name = "Lados do cubo";
  cube_sides.firstIndex = 0; // Primeiro índice está em indices[0]
  cube_sides.numIndexes =
      24; // Último índice está em indices[35]; total de 36 índices.
  cube_sides.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de
                                           // rasterização GL_TRIANGLES.

  // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
  g_VirtualScene["cube_sides"] = cube_sides;

  // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
  // coloridas do cubo.
  SceneObject cube_top;
  cube_top.name = "Topo do cubo";
  cube_top.firstIndex =
      24 * sizeof(unsigned int); // Primeiro índice está em indices[0]
  cube_top.numIndexes =
      6; // Último índice está em indices[35]; total de 36 índices.
  cube_top.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de
                                         // rasterização GL_TRIANGLES.

  // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
  g_VirtualScene["cube_top"] = cube_top;

  // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
  // coloridas do cubo.
  SceneObject cube_base;
  cube_base.name = "Base do cubo";
  cube_base.firstIndex =
      30 * sizeof(unsigned int); // Primeiro índice está em indices[0]
  cube_base.numIndexes =
      6; // Último índice está em indices[35]; total de 36 índices.
  cube_base.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de
                                          // rasterização GL_TRIANGLES.

  // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
  g_VirtualScene["cube_base"] = cube_base;

  // Criamos um buffer OpenGL para armazenar os índices acima
  GLuint indices_id;
  glGenBuffers(1, &indices_id);

  // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

  // Alocamos memória para o buffer.
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

  // Copiamos os valores do array indices[] para dentro do buffer.
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

  // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
  // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
  // caso contrário o VAO irá perder a informação sobre os índices.
  //
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
  //

  // "Desligamos" o VAO, evitando assim que operações posteriores venham a
  // alterar o mesmo. Isso evita bugs.
  glBindVertexArray(0);

  // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
  // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
  return vertex_array_object_id;
}
