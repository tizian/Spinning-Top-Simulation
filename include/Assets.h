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
    
    // Texture blank;
    Texture checkerboard;
    
    Material planeMaterial;
    Material sphereMaterial;

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
        
        // blank = Texture("textures/blank_low.png");
        checkerboard = Texture("textures/checkerboard_low.png");
        
        planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
        sphereMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
        sphereMaterial.setAmbientFactor(0.3f * sphereMaterial.getDiffuseFactor());
	}

	void destroy() {
		phongShader.destroy();
        textureShader.destroy();
        
		sphere.destroy();
        plane.destroy();
	}
};