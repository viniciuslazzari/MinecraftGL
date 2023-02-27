#include <string>

#include "program.hpp"

GLuint Program::getId(){
    return this->id;
}

void Program::createGpuProgram(GLuint vertexShader, GLuint fragmentShader){
    this->id = glCreateProgram();

    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);

    glLinkProgram(this->id);

    GLint linked = GL_FALSE;
    glGetProgramiv(this->id, GL_LINK_STATUS, &linked);

    if (linked == GL_FALSE){
        GLint logLenght = 0;
        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &logLenght);

        GLchar* log = new GLchar[logLenght];

        glGetProgramInfoLog(this->id, logLenght, &logLenght, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }
}