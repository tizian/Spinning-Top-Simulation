#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace Assets {

	Shader phongShader;
    Shader textureShader;
    Shader shadowShader;

    Mesh sphere;
    Mesh cube;
    Mesh spinningTop1;
    Mesh spinningTop2;
    Mesh spinningTop3;
    Mesh spinningTop4;
    Mesh spinningTop5;
    Mesh spinningTop6;
    Mesh spinningTop3Top;
    Mesh spinningTop3Bottom;
    
    Mesh plane;
    Mesh table;
    
    // Texture blank;
    Texture checkerboard;
    Texture lightWood;
    Texture darkWood;
    Texture brushedMetal;
    
    Material planeMaterial;
    Material blueMaterial;
    Material redMaterial;
    Material yellowMaterial;
    Material whiteMaterial;

	void init() {
		phongShader = Shader("shaders/phong.glslv", "shaders/phong.glslf");
        textureShader = Shader("shaders/texturedPhong.glslv", "shaders/texturedPhong.glslf");
        shadowShader = Shader("shaders/planarShadows.glslv", "shaders/planarShadows.glslf");
        
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
        
        // blank = Texture("textures/blank_low.png");
        lightWood = Texture("textures/lightWood.png");
        darkWood = Texture("textures/darkWood.png");
        checkerboard = Texture("textures/checkerboard_low.png");
        brushedMetal = Texture("textures/brushedMetal.png");
        
        planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
        blueMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
        blueMaterial.setAmbientFactor(0.3f * blueMaterial.getDiffuseFactor());
        
        redMaterial = Material(glm::vec3(0.6, 0.25, 0.32));      // red-ish
        redMaterial.setAmbientFactor(0.3f * redMaterial.getDiffuseFactor());
        
        yellowMaterial = Material(glm::vec3(0.6, 0.6, 0.32));      // red-ish
        yellowMaterial.setAmbientFactor(0.3f * yellowMaterial.getDiffuseFactor());
        
        whiteMaterial = Material(glm::vec3(1.0, 1.0, 1.0));      // red-ish
        whiteMaterial.setAmbientFactor(0.5f * whiteMaterial.getDiffuseFactor());
	}

	void destroy() {
		phongShader.destroy();
        textureShader.destroy();
        
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
	}
};