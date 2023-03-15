#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Texture {
    private:
        std::string file;
        GLenum target;
        GLuint object;

    public:
        Texture(std::string file, GLenum target);
        void load();
        void bind(GLenum unit);
};

#endif