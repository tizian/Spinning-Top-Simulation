#pragma once

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

class Texture
{
public:
    Texture() {};
    Texture(const std::string & texturePath);
    
    GLint getTextureID() const;
    GLint getTextureUnit() const;
    
    void use();
    
private:
    static GLint nextTextureUnit;
    
    GLuint m_textureUnit;
    
    char * readFile(const std::string & filename);
    
    GLuint m_textureID;
};