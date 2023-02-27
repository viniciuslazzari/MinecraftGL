#ifndef PROGRAM_H
#define PROGRAM_H

#include <glad/glad.h>

class Program {
    private:
        GLuint id = 0;

    public:
        GLuint getId();
        void createGpuProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif