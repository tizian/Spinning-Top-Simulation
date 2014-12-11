#include "Assets.h"

Shader Assets::phongShader;
Shader Assets::textureShader;
Shader Assets::shadowShader;
Shader Assets::skyboxShader;

Mesh Assets::sphere;
Mesh Assets::cube;
Mesh Assets::spinningTop1;
Mesh Assets::spinningTop2;
Mesh Assets::spinningTop3;
Mesh Assets::spinningTop4;
Mesh Assets::spinningTop5;
Mesh Assets::spinningTop6;
Mesh Assets::spinningTop3Top;
Mesh Assets::spinningTop3Bottom;

Mesh Assets::plane;
Mesh Assets::table;
Mesh Assets::skyboxBox;

// Texture blank;
Texture Assets::checkerboard;
Texture Assets::lightWood;
Texture Assets::darkWood;
Texture Assets::brushedMetal;

Texture Assets::skybox;

Material Assets::planeMaterial;
Material Assets::blueMaterial;
Material Assets::redMaterial;
Material Assets::yellowMaterial;
Material Assets::whiteMaterial;
Material Assets::slightlyRedMaterial;
Material Assets::slightlyGreenMaterial;

void Assets::init() {
    phongShader = Shader("shaders/phong.glslv", "shaders/phong.glslf");
    textureShader = Shader("shaders/texturedPhong.glslv", "shaders/texturedPhong.glslf");
    shadowShader = Shader("shaders/planarShadows.glslv", "shaders/planarShadows.glslf");
    skyboxShader = Shader("shaders/skybox.glslv", "shaders/skybox.glslf");
    
    // Sphere model with right texturecoords taken from: https://www.opengl.org/discussion_boards/showthread.php/176762-looking-for-a-simple-sphere-obj-file
    sphere = Mesh("models/sphere.obj");
    cube = Mesh("models/cube.obj");
    spinningTop1 = Mesh("models/spinningTop1.obj");
    spinningTop2 = Mesh("models/spinningTop2.obj");
    spinningTop3 = Mesh("models/spinningTop3.obj");
    spinningTop4 = Mesh("models/spinningTop4.obj");
    spinningTop5 = Mesh("models/spinningTop5.obj");
    spinningTop6 = Mesh("models/spinningTop6.obj");
    spinningTop3Top = Mesh("models/spinningTop3_TopHeavy.obj");
    spinningTop3Bottom = Mesh("models/spinningTop3_BottomHeavy.obj");
    
    plane = Mesh("models/plane.obj");
    table = Mesh("models/table.obj");
    skyboxBox = Mesh("models/skybox.obj");
    
    // blank = Texture("textures/blank_low.png");
    lightWood = Texture("textures/lightWood.png");
    darkWood = Texture("textures/darkWood.png");
    checkerboard = Texture("textures/checkerboard_low.png");
    brushedMetal = Texture("textures/brushedMetal.png");
    
    // Skybox taken from: http://www.keithlantz.net/2011/10/rendering-a-skybox-using-a-cube-map-with-opengl-and-glsl/
    std::string skbx = "skybox-sky";
    skybox = Texture("textures/"+skbx+"/xpos.png", "textures/"+skbx+"/xneg.png", "textures/"+skbx+"/ypos.png", "textures/"+skbx+"/yneg.png", "textures/"+skbx+"/zpos.png", "textures/"+skbx+"/zneg.png");
    
    planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
    blueMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
    
    redMaterial = Material(glm::vec3(1.0, 0.0, 0.0));      // red
    
    yellowMaterial = Material(glm::vec3(0.6, 0.6, 0.32));  // red-ish
    
    whiteMaterial = Material(glm::vec3(1.0, 1.0, 1.0));
    
    slightlyRedMaterial = Material(glm::vec3(1.0, 0.6, 0.6));      // slightly red
    
    slightlyGreenMaterial = Material(glm::vec3(0.6, 1.0, 0.6));
}

void Assets::destroy() {
    phongShader.destroy();
    textureShader.destroy();
    shadowShader.destroy();
    skyboxShader.destroy();
    
    sphere.destroy();
    cube.destroy();
    spinningTop1.destroy();
    spinningTop2.destroy();
    spinningTop3.destroy();
    spinningTop4.destroy();
    spinningTop5.destroy();
    spinningTop6.destroy();
    spinningTop3Top.destroy();
    spinningTop3Bottom.destroy();
    
    plane.destroy();
    table.destroy();
    skyboxBox.destroy();
}