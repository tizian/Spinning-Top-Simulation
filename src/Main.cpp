#define GLM_FORCE_RADIANS

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include "Assets.h"
#include "Camera.h"
#include "Shader.h"
#include "Body.h"
#include "Mesh.h"
#include "Color.h"
#include "PointLight.h"
#include "Material.h"
#include "RigidBody.h"

using namespace std;

void glfwWindowResizeCallback(GLFWwindow *window, int width, int height);

void setupContext();
void destroyContext();

static int width = 640;
static int height = 640;

float camSpeed = 8.0f;
float camRotSpeed = 100.0f * M_PI / 180.0f;

GLFWwindow *window;
Camera camera;

Body plane;
RigidBody sphere;

void resetSphere() {
    sphere = RigidBody(glm::vec3(0, 5, 0));
    sphere.setMesh(&Assets::textureSphere);
    sphere.setMaterial(&Assets::sphereMaterial);
    sphere.setTexture(&Assets::checkerboard);
    
    sphere.setOrientation(glm::quat_cast(glm::rotate(0.3f, glm::vec3(0.9, 1, 0.9))));
}

void addTorque() {
    sphere.addForce(glm::vec3(0, 0, 10), sphere.getPosition() + glm::vec3(1, 0, 0));
    sphere.addForce(glm::vec3(0, 0, -10), sphere.getPosition() + glm::vec3(-1, 0, 0));
}

int main()
{
    clock_t begin = clock();
    
	setupContext();
	Assets::init();

	// DEPTH TESTING
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// CULLING
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // GL_CCW for counter clock-wise
    
    plane = Body(glm::vec3(0, 0, 0));
    plane.setScale(glm::vec3(10, 1, 10));
    plane.setMesh(&Assets::plane);
    plane.setMaterial(&Assets::planeMaterial);
    plane.setTexture(&Assets::blank);
    
    resetSphere();

	camera.setPosition(glm::vec3(0.0f, 8.f, 15.f));
    camera.pitch(10 * M_PI / 180.0f);
	camera.setAspectRatio((float)width/height);

    PointLight testLight(glm::vec3(0, 10, 0));

	bool lightFollowsCamera = false;
    bool camMoved = true;

    bool lKeyPressed = false;
    
    glfwGetFramebufferSize(window, &width, &height);
    printf("Framebuffer width: %d height: %d\n", width, height);
	while (!glfwWindowShouldClose(window)) {
		// Timer
		static double previous = glfwGetTime();
		double current = glfwGetTime();
		float deltaTime = (float) current - (float) previous;
		previous = current;
        
        sphere.update(deltaTime);
        
		// clear drawing surface
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		glViewport(0, 0, width, height);

        plane.render(&Assets::textureShader);
        sphere.render(&Assets::textureShader);
        
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		glm::vec3 camOffset = glm::vec3(0,0,0);

		float deltaTheta = camRotSpeed * deltaTime;

        
        if (glfwGetKey(window, GLFW_KEY_R)) {
            resetSphere();
        }
        
        if (glfwGetKey(window, GLFW_KEY_T)) {
            addTorque();
        }
        
		if (glfwGetKey(window, GLFW_KEY_A)) {
			camera.moveLeftRight(camSpeed * deltaTime);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			camera.moveLeftRight(-camSpeed * deltaTime);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			camera.moveUpDown(-camSpeed * deltaTime);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) {
			camera.moveUpDown(camSpeed * deltaTime);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			camera.moveForwardBackward(camSpeed * deltaTime);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera.moveForwardBackward(-camSpeed * deltaTime);
			camMoved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_UP)) {
			camera.pitch(-deltaTheta);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			camera.pitch(deltaTheta);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			camera.yaw(-deltaTheta);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			camera.yaw(deltaTheta);
			camMoved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_0)) {
			camera.setPosition(glm::vec3());
			camMoved = true;
		}
        
		if (glfwGetKey(window, GLFW_KEY_L) && !lKeyPressed) {
            lKeyPressed = true;
			if (lightFollowsCamera) {
				lightFollowsCamera = false;
			}
			else {
				lightFollowsCamera = true;
			}
		} else if (!glfwGetKey(window, GLFW_KEY_L))
        {
            lKeyPressed = false;
        }
        
		if (lightFollowsCamera) {
            testLight.setPosition(camera.getPosition());
        }
        if (camMoved) {
            //printf("Camera position: %f %f %f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
            camera.move(camOffset);
        }
        
        Assets::textureShader.use();
        testLight.setUniforms(&Assets::textureShader);
        
        Assets::textureShader.setUniform("viewMatrix", camera.view());
        Assets::textureShader.setUniform("projectionMatrix", camera.projection());
        Assets::textureShader.setUniform("cameraPosition", camera.getPosition());
        
        camMoved = false;
	}

	Assets::destroy();
	destroyContext();
    clock_t endtime = clock();
    printf("The program ran for %f seconds.\n", double(endtime-begin) / CLOCKS_PER_SEC);
	return 0;
}

void setupContext() {
	// start GL context
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
	}
    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, "OpenGL Project", NULL, NULL);

	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte *renderer = glGetString(GL_RENDERER);	// get renderer string
	const GLubyte *version = glGetString(GL_VERSION);	// get version string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version: " << version << std::endl;
}

void destroyContext() {
	// close GL context and any other GLFW resources

	glfwDestroyWindow(window);

	glfwTerminate();
}

void glfwWindowResizeCallback(GLFWwindow *window, int w, int h) {
		width = w;
		height = h;

		camera.setAspectRatio((float)width/height);
}