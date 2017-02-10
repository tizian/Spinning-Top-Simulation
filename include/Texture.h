#pragma once

#include <GL/gl3w.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

class Texture
{
public:
    Texture() {};
    Texture(const std::string & texturePath);
    Texture(const std::string & xpos, const std::string & xneg, const std::string & ypos, const std::string & yneg, const std::string & zpos, const std::string & zneg);
    
    GLint getTextureID() const;
    GLint getTextureUnit() const;
    
    void use();
    
private:
    static GLint nextTextureUnit;
    
    GLuint m_textureUnit;
    
    char * readFile(const std::string & filename);
    
    GLuint m_textureID;
};