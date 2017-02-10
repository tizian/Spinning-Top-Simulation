#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

namespace Assets {
    extern Shader *getPhongShader();
    extern Shader *getTextureShader();
    extern Shader *getShadowShader();
    extern Shader *getSkyboxShader();
    
    extern Mesh *getSphere();
    extern Mesh *getCube();
    extern Mesh *getSpinningTop1();
    extern Mesh *getSpinningTop2();
    extern Mesh *getSpinningTop3();
    extern Mesh *getSpinningTop4();
    extern Mesh *getSpinningTop5();
    extern Mesh *getSpinningTop6();
    extern Mesh *getSpinningTop3Top();
    extern Mesh *getSpinningTop3Bottom();
    extern Mesh *getPlane();
    extern Mesh *getTable();
    extern Mesh *getSkybox();
    
    extern Texture *getLightWood();
    extern Texture *getDarkWood();
    extern Texture *getCheckerboard();
    extern Texture *getBrushedMetal();
    extern Texture *getSkyboxTexture();
    
    extern Material *getPlaneMaterial();
    extern Material *getBlueMaterial();
    extern Material *getRedMaterial();
    extern Material *getYellowMaterial();
    extern Material *getWhiteMaterial();
    extern Material *getSlightlyRedMaterial();
    extern Material *getSlightlyGreenMaterial();
};