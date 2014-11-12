#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace Assets {

	Shader phongShader;
    Shader textureShader;

	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh suzanne;
    Mesh textureSphere;
    Mesh plane;
    
    Texture blank;
    Texture checkerboard;
    
    Material planeMaterial;
    Material sphereMaterial;

	void init() {
		phongShader = Shader("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");
        textureShader = Shader("shaders/texture_vertex.glsl", "shaders/texture_fragment.glsl");
        
        cube = Mesh(&phongShader, "Models/cube.dae");
        
        sphere = Mesh(&phongShader, "Models/sphere.dae");
        
        bunny = Mesh(&phongShader, "Models/bunny.dae");
        
        suzanne = Mesh(&phongShader, "Models/suzanne.dae");
        
        // Sphere model with right texturecoords taken from: https://www.opengl.org/discussion_boards/showthread.php/176762-looking-for-a-simple-sphere-obj-file
        textureSphere = Mesh(&textureShader, "Models/textureSphere.obj");
        
        plane = Mesh(&textureShader, "Models/plane.obj");
        
        blank = Texture("textures/blank.png");
        checkerboard = Texture("textures/checkerboard.png");
        
        planeMaterial = Material(glm::vec3(0.16, 0.55, 0.16));     // green-ish
        sphereMaterial = Material(glm::vec3(0.25, 0.32, 0.6));      // blue-ish
	}

	void destroy() {
		phongShader.destroy();
        textureShader.destroy();
        
		cube.destroy();
		sphere.destroy();
		bunny.destroy();
		suzanne.destroy();
	}
};