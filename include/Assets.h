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
    Mesh plane;
    Mesh cube;
    Mesh spinningTop1;
    Mesh spinningTop2;
    Mesh spinningTop3;
    Mesh spinningTop4;
    Mesh spinningTop3Top;
    Mesh spinningTop3Bottom;
    
    // Texture blank;
    Texture checkerboard;
    
    Material planeMaterial;
    Material blueMaterial;
    Material redMaterial;
    Material yellowMaterial;

	void init() {
		phongShader = Shader("shaders/phong.glslv", "shaders/phong.glslf");
        textureShader = Shader("shaders/texturedPhong.glslv", "shaders/texturedPhong.glslf");
        shadowShader = Shader("shaders/planarShadows.glslv", "shaders/planarShadows.glslf");
        
        // Sphere model with right texturecoords taken from: https://www.opengl.org/discussion_boards/showthread.php/176762-looking-for-a-simple-sphere-obj-file
        sphere = Mesh("models/sphere.obj");
        cube = Mesh("models/cube.obj");
        plane = Mesh("models/plane.obj");
        spinningTop1 = Mesh("models/spinningTop1.obj");
        spinningTop2 = Mesh("models/spinningTop2.obj");
        spinningTop3 = Mesh("models/spinningTop3.obj");
        spinningTop4 = Mesh("models/spinningTop4.obj");
        spinningTop3Top = Mesh("models/spinningTop3_TopHeavy.obj");
        spinningTop3Bottom = Mesh("models/spinningTop3_BottomHeavy.obj");
        
        // blank = Texture("textures/blank_low.png");
        checkerboard = Texture("textures/checkerboard_low.png");
        
        planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
        blueMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
        blueMaterial.setAmbientFactor(0.3f * blueMaterial.getDiffuseFactor());
        
        redMaterial = Material(glm::vec3(0.6, 0.25, 0.32));      // red-ish
        redMaterial.setAmbientFactor(0.3f * redMaterial.getDiffuseFactor());
        
        yellowMaterial = Material(glm::vec3(0.6, 0.6, 0.32));      // red-ish
        yellowMaterial.setAmbientFactor(0.3f * yellowMaterial.getDiffuseFactor());
	}

	void destroy() {
		phongShader.destroy();
        textureShader.destroy();
        
		sphere.destroy();
        plane.destroy();
	}
};