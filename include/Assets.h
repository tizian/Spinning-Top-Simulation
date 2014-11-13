#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace Assets {

	Shader phongShader;
    Shader textureShader;

    Mesh sphere;
    Mesh plane;
    
    Texture blank;
    Texture checkerboard;
    
    Material planeMaterial;
    Material sphereMaterial;

	void init() {
		phongShader = Shader("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");
        textureShader = Shader("shaders/texture_vertex.glsl", "shaders/texture_fragment.glsl");
        
        // Sphere model with right texturecoords taken from: https://www.opengl.org/discussion_boards/showthread.php/176762-looking-for-a-simple-sphere-obj-file
        sphere = Mesh(&textureShader, "models/sphere.obj");
        plane = Mesh(&textureShader, "models/plane.obj");
        
        blank = Texture("textures/blank_low.png");
        checkerboard = Texture("textures/checkerboard_low.png");
        
        planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
        sphereMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
	}

	void destroy() {
		phongShader.destroy();
        textureShader.destroy();
        
		sphere.destroy();
        plane.destroy();
	}
};