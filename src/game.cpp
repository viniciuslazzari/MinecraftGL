#include <string>
#include <chrono>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "std/utils.h"
#include "std/matrices.h"

#include "game.hpp"
#include "globals.hpp"
#include "callbacks.hpp"
#include "camera.hpp"
#include "shaders_provider.hpp"
#include "window_provider.hpp"
#include "texture.hpp"
#include "perlin_noise.hpp"
#include "obj_loader.hpp"

#define MAP_SIZE 8

GLuint BuildTriangles();

int game(){
    WindowProvider windowProvider = WindowProvider(800, 800, "Teste");

    GLFWwindow* window = windowProvider.initWindow(ErrorCallback, KeyCallback, MouseButtonCallback, CursorPosCallback, ScrollCallback, FramebufferSizeCallback);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    ShadersProvider shaderProvider = ShadersProvider();

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    GLuint programId = shaderProvider.loadShadersFromFiles();

    ObjModel cowModel("assets/cow.obj");
    cowModel.ComputeNormals();
    cowModel.BuildTrianglesAndAddToVirtualScene();

    // Construímos a representação de um triângulo
    GLuint vertex_array_object_id = BuildTriangles();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform = glGetUniformLocation(programId, "model"); // Variável da matriz "model"
    GLint view_uniform = glGetUniformLocation(programId, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform = glGetUniformLocation(programId, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint object_id_uniform = glGetUniformLocation(programId, "object_id"); // Variável booleana em shader_vertex.glsl
    GLint sampler_uniform = glGetUniformLocation(programId, "sampler"); 

    PerlinNoise pn = PerlinNoise(MAP_SIZE, MAP_SIZE);

    vector<vector<float>> map = pn.generateNoise(8);

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    Texture grassSideTexture = Texture("assets/grass_side.png", GL_TEXTURE_2D);
    grassSideTexture.load();

    Texture grassTopTexture = Texture("assets/grass_top.jpg", GL_TEXTURE_2D);
    grassTopTexture.load();

    std::chrono::time_point<std::chrono::high_resolution_clock> elapsedTime, timeSinceLastFrame;

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window)){
        elapsedTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<double, std::milli>(elapsedTime - timeSinceLastFrame).count() / 1000;
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
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform1i(sampler_uniform, 0);

        int init = -MAP_SIZE / 2;

        for (int i = 0; i < MAP_SIZE; ++i){
            for(int j = 0; j < MAP_SIZE; j++){
                glm::mat4 model;

                model = Matrix_Translate(init + i * 1.0f, map[i][j] - 30, init + j * 1.0f);

                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

                grassSideTexture.bind(GL_TEXTURE0);

                // glDrawElements(
                //     g_VirtualScene["cube_sides"].renderingMode,
                //     g_VirtualScene["cube_sides"].numIndexes,
                //     GL_UNSIGNED_INT,
                //     (void*)g_VirtualScene["cube_sides"].firstIndex
                // );

                grassTopTexture.bind(GL_TEXTURE0);

                glDrawElements(
                    g_VirtualScene["cube_top"].renderingMode,
                    g_VirtualScene["cube_top"].numIndexes,
                    GL_UNSIGNED_INT,
                    (void*)g_VirtualScene["cube_top"].firstIndex
                );
            }
        }

        #define COW 4

        glm::mat4 model = Matrix_Translate(-2.0f, 0.0f, -2.0f);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, COW);
        cowModel.DrawVirtualObject("the_cow");

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
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

// Constrói triângulos para futura renderização
GLuint BuildTriangles(){
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
    // homogêneas.  Veja as seguintes referências:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
        // front face
        -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 0
        -0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 1
         0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 2
         0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 3

        // right face
         0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 4 (3)
         0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 5 (2)
         0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 6
         0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 7

        // back face
         0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 8 (7)
         0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 9 (6)
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 10
        -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 11

        // left face
        -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 12 (11)
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 13 (10)
        -0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 14 (1)
        -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 15 (0)

        // top face
        -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 16
        -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 17
         0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 18
         0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 19
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posição.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_model_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat texture_coefficients[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // front face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // right face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // back face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left face
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top face
    };

    GLuint VBO_texture_coefficients_id;
    glGenBuffers(1, &VBO_texture_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texture_coefficients), texture_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir polígonos utilizando os vértices do array
    // model_coefficients.
    //
    // Para referência sobre os modos de renderização, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint indices[] = {
        0, 1, 2, // triângulo 1 
        0, 2, 3, // triângulo 2
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_sides;
    cube_sides.name = "Lados do cubo";
    cube_sides.firstIndex = 0; // Primeiro índice está em indices[0]
    cube_sides.numIndexes = 24; // Último índice está em indices[35]; total de 36 índices.
    cube_sides.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_sides"] = cube_sides;

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_top;
    cube_top.name = "Topo do cubo";
    cube_top.firstIndex = 24; // Primeiro índice está em indices[0]
    cube_top.numIndexes = 6; // Último índice está em indices[35]; total de 36 índices.
    cube_top.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_top"] = cube_top;

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