#ifndef SHADERS_PROVIDER_H
#define SHADERS_PROVIDER_H

class ShadersProvider {
    private:
        const char* vertexShaderFilename = "./src/shaders/shader_vertex.glsl";
        const char* fragmentShaderFilename = "./src/shaders/shader_fragment.glsl";
        GLuint vertexShader;
        GLuint fragmentShader;

    public:
        void loadShadersFromFiles();
        GLuint loadShaderByType(const char* filename, GLenum shaderType);
        void loadShader(const char* filename, GLuint shaderId);
};

#endif