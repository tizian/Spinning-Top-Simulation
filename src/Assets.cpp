#include "Assets.h"

Shader *Assets::getPhongShader() {
    static Shader phongShader = Shader("shaders/phong.glslv", "shaders/phong.glslf");
    return &phongShader;
}

Shader *Assets::getTextureShader() {
    static Shader textureShader = Shader("shaders/texturedPhong.glslv", "shaders/texturedPhong.glslf");
    return &textureShader;
}

Shader *Assets::getShadowShader() {
    static Shader shadowShader = Shader("shaders/planarShadows.glslv", "shaders/planarShadows.glslf");
    return &shadowShader;
}

Shader *Assets::getSkyboxShader() {
    static Shader skyboxShader = Shader("shaders/skybox.glslv", "shaders/skybox.glslf");
    return &skyboxShader;
}

// Sphere model with right texturecoords taken from: https://www.opengl.org/discussion_boards/showthread.php/176762-looking-for-a-simple-sphere-obj-file
Mesh *Assets::getSphere() {
    static Mesh sphere = Mesh("models/sphere.obj");
    return &sphere;
}

Mesh *Assets::getCube() {
    static Mesh cube = Mesh("models/cube.obj");
    return &cube;
}

Mesh *Assets::getSpinningTop1() {
    static Mesh spinningTop = Mesh("models/spinningTop1.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop2() {
    static Mesh spinningTop = Mesh("models/spinningTop2.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop3() {
    static Mesh spinningTop = Mesh("models/spinningTop3.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop4() {
    static Mesh spinningTop = Mesh("models/spinningTop4.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop5() {
    static Mesh spinningTop = Mesh("models/spinningTop5.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop6() {
    static Mesh spinningTop = Mesh("models/spinningTop6.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop3Top() {
    static Mesh spinningTop = Mesh("models/spinningTop3_TopHeavy.obj");
    return &spinningTop;
}

Mesh *Assets::getSpinningTop3Bottom() {
    static Mesh spinningTop = Mesh("models/spinningTop3_BottomHeavy.obj");
    return &spinningTop;
}

Mesh *Assets::getPlane() {
    static Mesh plane = Mesh("models/plane.obj");
    return &plane;
}

Mesh *Assets::getTable() {
    static Mesh table = Mesh("models/table.obj");
    return &table;
}

Mesh *Assets::getSkybox() {
    static Mesh skyboxBox = Mesh("models/skybox.obj");
    return &skyboxBox;
}

Texture *Assets::getLightWood() {
    static Texture lightWood = Texture("textures/lightWood.png");
    return &lightWood;
}

Texture *Assets::getDarkWood() {
    static Texture darkWood = Texture("textures/wood4096.png");
    return &darkWood;
}

Texture *Assets::getCheckerboard() {
    static Texture checkerboard = Texture("textures/checkerboard_low.png");
    return &checkerboard;
}

Texture *Assets::getBrushedMetal() {
    static Texture brushedMetal = Texture("textures/brushedMetal.png");
    return &brushedMetal;
}

// Skybox taken from: http://www.keithlantz.net/2011/10/rendering-a-skybox-using-a-cube-map-with-opengl-and-glsl/
Texture *Assets::getSkyboxTexture() {
    static std::string skbx = "skybox-sea";
    static Texture skybox = Texture("textures/"+skbx+"/xpos.png", "textures/"+skbx+"/xneg.png", "textures/"+skbx+"/ypos.png", "textures/"+skbx+"/yneg.png", "textures/"+skbx+"/zpos.png", "textures/"+skbx+"/zneg.png");
    
    return &skybox;
}

Material *Assets::getPlaneMaterial() {
    static Material mat = Material(glm::vec3(0.16, 0.55, 0.16));    // green-ish
    return &mat;
}

Material *Assets::getBlueMaterial() {
    static Material mat = Material(glm::vec3(0.25, 0.32, 0.6));     // blue-ish
    return &mat;
}

Material *Assets::getRedMaterial() {
    static Material mat = Material(glm::vec3(1.0, 0.0, 0.0));       // red
    return &mat;
}

Material *Assets::getYellowMaterial() {
    static Material mat = Material(glm::vec3(0.6, 0.6, 0.32));      // red-ish
    return &mat;
}

Material *Assets::getWhiteMaterial() {
    static Material mat = Material(glm::vec3(1.0, 1.0, 1.0));
    return &mat;
}

Material *Assets::getSlightlyRedMaterial() {
    static Material mat = Material(glm::vec3(1.0, 0.6, 0.6));       // slightly red
    return &mat;
}

Material *Assets::getSlightlyGreenMaterial() {
    static Material mat = Material(glm::vec3(0.6, 1.0, 0.6));
    return &mat;
}