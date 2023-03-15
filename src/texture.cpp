#include "texture.hpp"
#include "stb_image.h"

Texture::Texture(std::string file, GLenum target){
    this->target = target;
    this->file = file;
}

void Texture::load(){
    stbi_set_flip_vertically_on_load(1);
    int width = 0, height = 0, bpp = 0;

    const std::string path = this->file;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &bpp, 4);

    printf("width: %d, height = %d, bpp = %d\n", width, height, bpp);

    glGenTextures(1, &this->object);
    glBindTexture(this->target, this->object);

    glTexParameterf(this->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(this->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(this->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(this->target, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(this->target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(this->target, 0);
}

void Texture::bind(GLenum unit){
    glActiveTexture(unit);
    glBindTexture(this->target, this->object);
}