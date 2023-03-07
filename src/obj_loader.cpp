#define TINYOBJLOADER_IMPLEMENTATION

#include "obj_loader.hpp"
#include "std/matrices.h"
#include "globals.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/vec4.hpp>

ObjModel::ObjModel(const char* fileName, const char* basePath, bool triangulate){
    std::string fullPath(fileName);
    std::string dirName;

    if (basePath == NULL){
        auto i = fullPath.find_last_of("/");

        if (i != std::string::npos){
            dirName = fullPath.substr(0, i+1);
            basePath = dirName.c_str();
        }
    }

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, basePath, triangulate);

    if (!err.empty()) fprintf(stderr, "\n%s\n", err.c_str());

    if (!ret) throw std::runtime_error("Erro ao carregar modelo.");

    for (size_t shape = 0; shape < shapes.size(); ++shape){
        if (shapes[shape].name.empty()){
            fprintf(stderr, "Objeto sem nome dentro do arquivo '%s'.\n", fileName);
            throw std::runtime_error("Objeto sem nome.");
        }
    }
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ObjModel::ComputeNormals()
{
    if (!this->attrib.normals.empty()) return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t numVertices = this->attrib.vertices.size() / 3;

    std::vector<int> numTriangles_per_vertex(numVertices, 0);
    std::vector<glm::vec4> vertex_normals(numVertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    for (size_t shape = 0; shape < this->shapes.size(); ++shape){
        size_t numTriangles = this->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < numTriangles; ++triangle){
            assert(this->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4 vertices[3];

            for (size_t vertex = 0; vertex < 3; ++vertex){
                tinyobj::index_t idx = this->shapes[shape].mesh.indices[3*triangle + vertex];

                const float vx = this->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = this->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = this->attrib.vertices[3*idx.vertex_index + 2];

                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4 a = vertices[0];
            const glm::vec4 b = vertices[1];
            const glm::vec4 c = vertices[2];

            const glm::vec4 n = crossproduct(b - a, c - a);

            for (size_t vertex = 0; vertex < 3; ++vertex){
                tinyobj::index_t idx = this->shapes[shape].mesh.indices[3*triangle + vertex];
                numTriangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                this->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    this->attrib.normals.resize(3 * numVertices);

    for (size_t i = 0; i < vertex_normals.size(); ++i){
        glm::vec4 n = vertex_normals[i] / (float)numTriangles_per_vertex[i];
        n /= norm(n);

        this->attrib.normals[3*i + 0] = n.x;
        this->attrib.normals[3*i + 1] = n.y;
        this->attrib.normals[3*i + 2] = n.z;
    }
}

void ObjModel::BuildTrianglesAndAddToVirtualScene(){
    GLuint id;

    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    std::vector<GLuint> indices;
    std::vector<float> model_coefficients;
    std::vector<float> normal_coefficients;
    std::vector<float> texture_coefficients;

    for (size_t shape = 0; shape < this->shapes.size(); ++shape){
        size_t firstIndex = indices.size();
        size_t numTriangles = this->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < numTriangles; ++triangle){
            assert(this->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex){
                tinyobj::index_t idx = this->shapes[shape].mesh.indices[3 * triangle + vertex];

                indices.push_back(firstIndex + 3 * triangle + vertex);

                const float vx = this->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = this->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = this->attrib.vertices[3 * idx.vertex_index + 2];

                model_coefficients.push_back(vx); // X
                model_coefficients.push_back(vy); // Y
                model_coefficients.push_back(vz); // Z
                model_coefficients.push_back(1.0f); // W

                if (idx.normal_index != -1){
                    const float nx = this->attrib.normals[3 * idx.normal_index + 0];
                    const float ny = this->attrib.normals[3 * idx.normal_index + 1];
                    const float nz = this->attrib.normals[3 * idx.normal_index + 2];
                    normal_coefficients.push_back(nx); // X
                    normal_coefficients.push_back(ny); // Y
                    normal_coefficients.push_back(nz); // Z
                    normal_coefficients.push_back(0.0f); // W
                }

                if (idx.texcoord_index != -1){
                    const float u = this->attrib.texcoords[2 * idx.texcoord_index + 0];
                    const float v = this->attrib.texcoords[2 * idx.texcoord_index + 1];
                    texture_coefficients.push_back(u);
                    texture_coefficients.push_back(v);
                }
            }
        }

        size_t lastIndex = indices.size() - 1;

        SceneObject theobject;
        theobject.name = this->shapes[shape].name;
        theobject.firstIndex = firstIndex; // Primeiro índice
        theobject.numIndexes = lastIndex - firstIndex + 1; // Número de indices
        theobject.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        theobject.id = id;

        g_VirtualScene[this->shapes[shape].name] = theobject;
    }

    GLuint VBOids;
    glGenBuffers(1, &VBOids);
    glBindBuffer(GL_ARRAY_BUFFER, VBOids);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());

    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint numberDimensions = 4; // vec4 em "shader_vertex.glsl"

    glVertexAttribPointer(location, numberDimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!normal_coefficients.empty()){
        GLuint VBOnormals;

        glGenBuffers(1, &VBOnormals);
        glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);

        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        numberDimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, numberDimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!texture_coefficients.empty()){
        GLuint VBOtextures;

        glGenBuffers(1, &VBOtextures);
        glBindBuffer(GL_ARRAY_BUFFER, VBOtextures);

        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        numberDimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, numberDimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indexesIds;
    glGenBuffers(1, &indexesIds);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexesIds);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void ObjModel::DrawVirtualObject(const char* object_name){
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].id);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas.
    glDrawElements(
        g_VirtualScene[object_name].renderingMode,
        g_VirtualScene[object_name].numIndexes,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].firstIndex * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}