#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int Texture::nextTextureUnit = 0;

Texture::Texture(const std::string &texturePath) {
    m_textureUnit = Texture::nextTextureUnit;
    Texture::nextTextureUnit++;
    
    glGenTextures(1, &m_textureID);
    
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    int width, height, channels;
    uint8_t *image = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    
    // glBindTexture(GL_TEXTURE_2D, 0);
    // glActiveTexture(GL_TEXTURE0);
}

Texture::Texture(const std::string &xpos, const std::string &xneg, const std::string &ypos, const std::string &yneg, const std::string &zpos, const std::string &zneg) {
    m_textureUnit = Texture::nextTextureUnit;
    Texture::nextTextureUnit++;
    
    glEnable(GL_TEXTURE_CUBE_MAP);
    
    glGenTextures(1, &m_textureID);
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    int width, height, channels;
    uint8_t* image;
    
    image = stbi_load(xpos.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    
    image = stbi_load(xneg.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    
    image = stbi_load(ypos.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    
    image = stbi_load(yneg.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    
    image = stbi_load(zpos.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    
    image = stbi_load(zneg.c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
}

GLint Texture::getTextureUnit() const {
    return m_textureUnit;
}

GLint Texture::getTextureID() const {
    return m_textureID;
}

void Texture::use() {
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    // glBindTexture(GL_TEXTURE_2D, m_textureID);
}