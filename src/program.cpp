#include <string>
#include <fstream>
#include <sstream>

#include "program.hpp"

GLuint Program::getId(){
    return this->id;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
void Program::createGpuProgram(GLuint vertexShader, GLuint fragmentShader){
    // Criamos um identificador (ID) para este programa de GPU
    this->id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(this->id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(this->id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE ){
        GLint log_length = 0;
        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(this->id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }
}