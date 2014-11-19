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
void glfwFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

void setupContext();
void destroyContext();

static int width = 640;
static int height = 640;

float timeStep = 0.02;
bool fixTimestep = false;

float camSpeed = 8.0f;
float camRotSpeed = 100.0f * M_PI / 180.0f;

bool slowMotion;

GLFWwindow *window;
Camera camera;

Body plane;
RigidBody spinningTop;

void reset() {
    spinningTop = RigidBody();
    spinningTop.setPosition(glm::vec3(0, 5, 0));
    spinningTop.setMaterial(&Assets::sphereMaterial);
    spinningTop.setTexture(&Assets::checkerboard);
    
    //spinningTop.setOrientation(glm::quat_cast(glm::rotate((float)M_PI, glm::vec3(1, 0, 0))));
}

void resetCamera()
{
    camera.setPosition(glm::vec3(0.0f, 8.f, 15.f));
    camera.setOrientation(fquat(1.0,0,0,0));
    camera.pitch(10 * M_PI / 180.0f);
    camera.setAspectRatio((float)width/height);
}

void resetSphere() {
    reset();
    spinningTop.setMesh(&Assets::sphere);
}

void resetSpinningTop1() {
    reset();
    spinningTop.setMesh(&Assets::spinningTop1);
}

void resetSpinningTop2() {
    reset();
    spinningTop.setMesh(&Assets::spinningTop2);
}

void resetSpinningTop3() {
    reset();
    spinningTop.setMesh(&Assets::spinningTop3);
}

void addTorque() {
    spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition() + glm::vec3(1, 0, 0));
    spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition() + glm::vec3(-1, 0, 0));
}

void addReverseTorque() {
    spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition() + glm::vec3(1, 0, 0));
    spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition() + glm::vec3(-1, 0, 0));
}

int main()
{
    time_t begin = time(0);
    
	setupContext();
	Assets::init();

	// DEPTH TESTING
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// CULLING
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // GL_CCW for counter clock-wise
    
    // BLENDING
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    plane = Body(glm::vec3(0, 0, 0));
    plane.setScale(glm::vec3(10, 1, 10));
    plane.setMesh(&Assets::plane);
    plane.setMaterial(&Assets::planeMaterial);
    
    resetSphere();

    glfwGetFramebufferSize(window, &width, &height);
    printf("Framebuffer width: %d height: %d\n", width, height);
    
    resetCamera();

    PointLight testLight(glm::vec3(0, 10, 0));

	bool lightFollowsCamera = false;
    bool camMoved = true;

    bool lKeyPressed = false;
    bool xKeyPressed = false;
    
	while (!glfwWindowShouldClose(window)) {
        
		// Timer
		static double previous = glfwGetTime();
		double current = glfwGetTime();
		float deltaTime = (float) current - (float) previous;
        
        if (slowMotion) {
            deltaTime /= 8;
        }
        
        if (fixTimestep)
        {
            if (deltaTime > timeStep)
            {
                previous = current;
                spinningTop.update(timeStep);
            }
        } else {
            previous = current;
            spinningTop.update(deltaTime);
        }
        
		// clear drawing surface
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		glViewport(0, 0, width, height);

        Assets::textureShader.use();
        
        testLight.setUniforms();
        Shader::setUniform("viewMatrix", camera.view());
        Shader::setUniform("projectionMatrix", camera.projection());
        Shader::setUniform("cameraPosition", camera.getPosition());
        
        spinningTop.render(false);
        
        Assets::phongShader.use();
        
        testLight.setUniforms();
        Shader::setUniform("viewMatrix", camera.view());
        Shader::setUniform("projectionMatrix", camera.projection());
        Shader::setUniform("cameraPosition", camera.getPosition());
        
        plane.render(false);
        
        Assets::shadowShader.use();
        glPolygonOffset(0, -1);
        
        glm::vec3 l = testLight.getPosition();
        glm::mat4 planeMatrix = glm::mat4();
        planeMatrix[0] = glm::vec4(l.y, 0, 0, 0);
        planeMatrix[1] = glm::vec4(-l.x, 0, -l.z, -1);
        planeMatrix[2] = glm::vec4(0, 0, l.y, 0);
        planeMatrix[3] = glm::vec4(0, 0, 0, l.y);
        
        Shader::setUniform("shadowColor", glm::vec4(0, 0, 0, 0.6));
        Shader::setUniform("viewMatrix", camera.view());
        Shader::setUniform("projectionMatrix", camera.projection());
        Shader::setUniform("planeMatrix", planeMatrix);
        
        spinningTop.render(true);
        
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		glm::vec3 camOffset = glm::vec3(0,0,0);

        if (slowMotion) {
            deltaTime = deltaTime * 8;
        }
        
		float deltaTheta = camRotSpeed * deltaTime;

        
        if (glfwGetKey(window, GLFW_KEY_1)) {
            resetSpinningTop1();
        }
        else if (glfwGetKey(window, GLFW_KEY_2)) {
            resetSpinningTop2();
        }
        else if (glfwGetKey(window, GLFW_KEY_3)) {
            resetSpinningTop3();
        }
        else if (glfwGetKey(window, GLFW_KEY_0)) {
            resetSphere();
        }
        
        if (glfwGetKey(window, GLFW_KEY_T)) {
            addTorque();
        }
        if (glfwGetKey(window, GLFW_KEY_R)) {
            addReverseTorque();
        }
        
        if (glfwGetKey(window, GLFW_KEY_U)) {
            spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition());
        }
        if (glfwGetKey(window, GLFW_KEY_H)) {
            spinningTop.addForce(glm::vec3(-10, 0, 0), spinningTop.getPosition());
        }
        if (glfwGetKey(window, GLFW_KEY_J)) {
            spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition());
        }
        if (glfwGetKey(window, GLFW_KEY_K)) {
            spinningTop.addForce(glm::vec3(10, 0, 0), spinningTop.getPosition());
        }
        if (glfwGetKey(window, GLFW_KEY_Y)) { // Is actually Z on a swiss/german keyboard
            spinningTop.addForce(glm::vec3(0, 20, 0), spinningTop.getPosition());
        }
        if (glfwGetKey(window, GLFW_KEY_I)) {
            spinningTop.addForce(glm::vec3(5, 0, 0), spinningTop.getPosition() + glm::vec3(0, 1, 0));
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
        if (glfwGetKey(window, GLFW_KEY_A)) {
            camera.moveLeftRight(camSpeed * deltaTime);
            camMoved = true;
        }
		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera.moveForwardBackward(-camSpeed * deltaTime);
			camMoved = true;
		}
        if (glfwGetKey(window, GLFW_KEY_D)) {
            camera.moveLeftRight(-camSpeed * deltaTime);
            camMoved = true;
        }
        
        if (glfwGetKey(window, GLFW_KEY_C)) {
            resetCamera();
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
        
        if (glfwGetKey(window, GLFW_KEY_X) && !xKeyPressed) {
            xKeyPressed = true;
            slowMotion = !slowMotion;
        }
        else if (!glfwGetKey(window, GLFW_KEY_X)) {
            xKeyPressed = false;
        }
        
		if (glfwGetKey(window, GLFW_KEY_L) && !lKeyPressed) {
            lKeyPressed = true;
            lightFollowsCamera = !lightFollowsCamera;
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

        camMoved = false;
	}

	Assets::destroy();
	destroyContext();
    time_t endtime = time(0);
    
    printf("The program ran for %d seconds.\n", (int)difftime(endtime, begin));
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
    glfwSetFramebufferSizeCallback(window, glfwFrameBufferSizeCallback);

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
    
        glfwGetFramebufferSize(window, &width, &height);
        printf("Framebuffer width: %d height: %d\n", width, height);
    
		camera.setAspectRatio((float)width/height);
}

void glfwFrameBufferSizeCallback(GLFWwindow* window, int w, int h)
{
    glfwGetFramebufferSize(window, &width, &height);
    printf("Framebuffer width: %d height: %d\n", width, height);
    
    camera.setAspectRatio((float)width/height);
}