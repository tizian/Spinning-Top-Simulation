#include "Texture.h"

#include <SOIL/SOIL.h>

int Texture::nextTextureUnit = 0;

Texture::Texture(const std::string & texturePath) {
    m_textureUnit = Texture::nextTextureUnit;
    Texture::nextTextureUnit++;
    
    glGenTextures(1, &m_textureID);
    
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    int width, height;
    unsigned char* image = SOIL_load_image(texturePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glActiveTexture(GL_TEXTURE0);
}

GLint Texture::getTextureUnit() const {
    return m_textureUnit;
}

GLint Texture::getTextureID() const {
    return m_textureID;
}

void Texture::use() {
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
//    glBindTexture(GL_TEXTURE_2D, m_textureID);
}