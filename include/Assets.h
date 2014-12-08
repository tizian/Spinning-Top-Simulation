#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace Assets {

	extern Shader phongShader;
    extern Shader textureShader;
    extern Shader shadowShader;
    extern Shader skyboxShader;

    extern Mesh sphere;
    extern Mesh cube;
    extern Mesh spinningTop1;
    extern Mesh spinningTop2;
    extern Mesh spinningTop3;
    extern Mesh spinningTop4;
    extern Mesh spinningTop5;
    extern Mesh spinningTop6;
    extern Mesh spinningTop3Top;
    extern Mesh spinningTop3Bottom;
    
    extern Mesh plane;
    extern Mesh table;
    extern Mesh skyboxBox;
    
    // Texture blank;
    extern Texture checkerboard;
    extern Texture lightWood;
    extern Texture darkWood;
    extern Texture brushedMetal;
    
    extern Texture skybox;
    
    extern Material planeMaterial;
    extern Material blueMaterial;
    extern Material redMaterial;
    extern Material yellowMaterial;
    extern Material whiteMaterial;
    extern Material slightlyRedMaterial;
    
    extern void init();
    
    extern void destroy();
};