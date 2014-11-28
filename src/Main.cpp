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
#include <vector>

#include "Assets.h"
#include "Camera.h"
#include "Shader.h"
#include "Body.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Material.h"
#include "RigidBody.h"
#include "RigidBodyFactory.h"

using namespace std;

void glfwWindowResizeCallback(GLFWwindow *window, int width, int height);
void glfwFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

void setupContext();
void destroyContext();

void render();
void input(float dt);

static int width = 640;
static int height = 640;

float timeStep = 0.01;
bool fixTimestep = true;

float camSpeed = 8.0f;
float camRotSpeed = 100.0f * M_PI / 180.0f;

// slow motion does not work for fixed time steps. (That's fine!)
bool slowMotion;

// to pause the simulation
bool pause;

bool lightFollowsCamera = false;

bool lKeyPressed = false;
bool xKeyPressed = false;
bool pKeyPressed = false;
bool nKeyPressed = false;
bool bKeyPressed = false;

GLFWwindow *window;

Camera camera;
PointLight light;

Body plane;

const int MAX_SIMULATION_SAVE_STATES = 1000;

int currentRenderState;
vector<vector<RigidBody> > simulationStates;

RigidBody spinningTop;

void resetCamera()
{
    camera.setPosition(glm::vec3(0.0f, 8.f, 15.f));
    camera.setOrientation(fquat(1.0,0,0,0));
    camera.pitch(10 * M_PI / 180.0f);
    camera.setAspectRatio((float)width/height);
}

void addTorque() {
    spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition() + glm::vec3(1, 0, 0));
    spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition() + glm::vec3(-1, 0, 0));
}

void addReverseTorque() {
    spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition() + glm::vec3(1, 0, 0));
    spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition() + glm::vec3(-1, 0, 0));
}

void resetStates() {
    currentRenderState = 0;
    simulationStates = vector<vector<RigidBody> >();
}

void forwardStep() {
    if (currentRenderState == simulationStates.size()) {
        spinningTop.update(timeStep);
        vector<RigidBody> bodies = vector<RigidBody> {spinningTop};
        simulationStates.push_back(bodies);
        
        if (simulationStates.size() > MAX_SIMULATION_SAVE_STATES) {
            simulationStates.erase(simulationStates.begin());
        }
        else {
            currentRenderState++;
        }
    }
    else if (currentRenderState < simulationStates.size()) {
        vector<RigidBody> bodies = simulationStates[currentRenderState];
        spinningTop = bodies[0];
        currentRenderState++;
    }
}

void backwardStep() {
    if (currentRenderState == 1) return;
    currentRenderState--;
    vector<RigidBody> bodies = simulationStates[currentRenderState-1];
    spinningTop = bodies[0];
}

void removeOldStates() {
    for (int i = 0; i < simulationStates.size() - currentRenderState; i++) {
        simulationStates.pop_back();
    }
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
    
    resetStates();
    
    RigidBodyFactory::resetSpinningTop1(spinningTop);

    glfwGetFramebufferSize(window, &width, &height);
    printf("Framebuffer width: %d height: %d\n", width, height);
    
    resetCamera();

    light = PointLight(glm::vec3(0, 10, 0));
    
    pause = false;
    
	while (!glfwWindowShouldClose(window)) {
        
		// Timer
		static double previous = glfwGetTime();
		double current = glfwGetTime();
		float deltaTime = (float) current - (float) previous;
        previous = current;
        
        if (slowMotion) {
            deltaTime /= 8;
        }
        
        if (!pause)
        {
            //printf("update call\n");
            if (fixTimestep)
            {
                forwardStep();
            } else {
                spinningTop.update(deltaTime);
            }
        }
        
        if (slowMotion) {
            deltaTime = deltaTime * 8;
        }
        
        input(deltaTime);
        
        render();
        
        //glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Assets::destroy();
	destroyContext();
    time_t endtime = time(0);
    
    printf("The program ran for %d seconds.\n", (int)difftime(endtime, begin));
	return 0;
}

void render() {
    // clear drawing surface
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, width, height);
    
    Assets::textureShader.use();
    
    light.setUniforms();
    camera.setUniforms();
    
    spinningTop.render();
    
    Assets::phongShader.use();
    
    light.setUniforms();
    camera.setUniforms();
    
    plane.render();
    
    Assets::shadowShader.use();
    
    light.setUniforms();
    camera.setUniforms();
    Shader::setUniform("shadowColor", glm::vec4(0, 0, 0, 0.6));
    
    spinningTop.render();
}

void input(float dt) {
    
    bool cleanStates = false;
    
    if (!pause) {
        // Select Spinning Top
        
        if (glfwGetKey(window, GLFW_KEY_1)) {
            RigidBodyFactory::resetSpinningTop1(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2)) {
            RigidBodyFactory::resetSpinningTop2(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_3)) {
            RigidBodyFactory::resetSpinningTop3(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_4)) {
            RigidBodyFactory::resetSpinningTop4(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_5)) {
            RigidBodyFactory::resetSpinningTop3Top(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_6)) {
            RigidBodyFactory::resetSpinningTop3Bottom(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_9)) {
            RigidBodyFactory::resetCube(spinningTop);
            cleanStates = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_0)) {
            RigidBodyFactory::resetSphere(spinningTop);
            cleanStates = true;
        }
        
        // Control Spinning Top
        
        if (glfwGetKey(window, GLFW_KEY_T)) {
            addTorque();
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_R)) {
            addReverseTorque();
            cleanStates = true;
        }
        
        if (glfwGetKey(window, GLFW_KEY_U)) {
            spinningTop.addForce(glm::vec3(0, 0, -10), spinningTop.getPosition());
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_H)) {
            spinningTop.addForce(glm::vec3(-10, 0, 0), spinningTop.getPosition());
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_J)) {
            spinningTop.addForce(glm::vec3(0, 0, 10), spinningTop.getPosition());
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_K)) {
            spinningTop.addForce(glm::vec3(10, 0, 0), spinningTop.getPosition());
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_Y)) { // Is actually Z on a swiss/german keyboard
            spinningTop.addForce(glm::vec3(0, 20, 0), spinningTop.getPosition());
            cleanStates = true;
        }
        if (glfwGetKey(window, GLFW_KEY_I)) {
            spinningTop.addForce(glm::vec3(5, 0, 0), spinningTop.getPosition() + glm::vec3(0, 1, 0));
            cleanStates = true;
        }
        
        if (cleanStates) {
            removeOldStates();
        }
    }

    // Camera
    
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        camera.moveUpDown(-camSpeed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) {
        camera.moveUpDown(camSpeed * dt);
    }
    
    
    if (glfwGetKey(window, GLFW_KEY_W)) {
        camera.moveForwardBackward(camSpeed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera.moveLeftRight(camSpeed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        camera.moveForwardBackward(-camSpeed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        camera.moveLeftRight(-camSpeed * dt);
    }
    
    if (glfwGetKey(window, GLFW_KEY_C)) {
        resetCamera();
    }
    
    float deltaTheta = camRotSpeed * dt;
    
    if (glfwGetKey(window, GLFW_KEY_UP)) {
        camera.pitch(-deltaTheta);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        camera.pitch(deltaTheta);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
        camera.yaw(-deltaTheta);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
        camera.yaw(deltaTheta);
    }
    
    // Control Simulation
    
    if (glfwGetKey(window, GLFW_KEY_P) && !pKeyPressed)
    {
        pKeyPressed = true;
        pause = !pause;
    } else if (!glfwGetKey(window, GLFW_KEY_P))
    {
        pKeyPressed = false;
    }
    
    if (pause) {
        if (glfwGetKey(window, GLFW_KEY_X) && !xKeyPressed) {
            xKeyPressed = true;
            slowMotion = !slowMotion;
        }
        else if (!glfwGetKey(window, GLFW_KEY_X)) {
            xKeyPressed = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_N) && !nKeyPressed) {
            nKeyPressed = true;
            forwardStep();
        }
        else if (!glfwGetKey(window, GLFW_KEY_N)) {
            nKeyPressed = false;
        }
        
        if (glfwGetKey(window, GLFW_KEY_N) && glfwGetKey(window, GLFW_KEY_M)) {
            forwardStep();
        }
        if (glfwGetKey(window, GLFW_KEY_B) && glfwGetKey(window, GLFW_KEY_M)) {
            backwardStep();
        }
        
        if (glfwGetKey(window, GLFW_KEY_B) && !bKeyPressed) {
            bKeyPressed = true;
            backwardStep();
        }
        else if (!glfwGetKey(window, GLFW_KEY_B)) {
            bKeyPressed = false;
        }
    }
    
    // Control Light
    
//    if (glfwGetKey(window, GLFW_KEY_L) && !lKeyPressed) {
//        lKeyPressed = true;
//        lightFollowsCamera = !lightFollowsCamera;
//    } else if (!glfwGetKey(window, GLFW_KEY_L))
//    {
//        lKeyPressed = false;
//    }
    
    if (lightFollowsCamera) {
        light.setPosition(camera.getPosition());
    }
    
    // Window closing
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, 1);
    }
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