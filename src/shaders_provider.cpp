#include <string>
#include <fstream>
#include <sstream>

#include "shaders_provider.hpp"
#include "program.hpp"

using namespace std;

void ShadersProvider::loadShader(const char* filename, GLuint shaderId){
    ifstream file;

    try {
        file.exceptions(ifstream::failbit);
        file.open(filename);
    }catch (exception& e){
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        exit(EXIT_FAILURE);
    }

    stringstream shader;
    shader << file.rdbuf();
    string str = shader.str();
    const GLchar* shaderString = str.c_str();
    const GLint shaderStringLenght = static_cast<GLint>( str.length() );

    glShaderSource(shaderId, 1, &shaderString, &shaderStringLenght);

    glCompileShader(shaderId);

    GLint compiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);

    GLint logLenght = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLenght);

    GLchar* log = new GLchar[logLenght];
    glGetShaderInfoLog(shaderId, logLenght, &logLenght, log);

    if (logLenght != 0){
        string output;

        if (!compiled){
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }else{
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    delete [] log;
}

GLuint ShadersProvider::loadShaderByType(const char* filename, GLenum shaderType){
    GLuint shaderId = glCreateShader(shaderType);

    loadShader(filename, shaderId);

    return shaderId;
}

GLuint ShadersProvider::loadShadersFromFiles(){
    this->vertexShader = loadShaderByType(vertexShaderFilename, GL_VERTEX_SHADER);
    this->fragmentShader = loadShaderByType(fragmentShaderFilename, GL_FRAGMENT_SHADER);

    Program program = Program();

    program.createGpuProgram(this->vertexShader, this->fragmentShader);

    return program.getId();
}