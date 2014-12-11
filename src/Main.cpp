#define GLM_FORCE_RADIANS

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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
#include "Simulation.h"

#include "PointLight.h"
#include "Camera.h"

using namespace std;
using namespace glm;

// glfwGetKeyOnce(...)
// http://sourceforge.net/p/glfw/discussion/247562/thread/8f3df980/
char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY) (glfwGetKey(WINDOW, KEY) ?(keyOnce[KEY] ? false : (keyOnce[KEY] = true)) : (keyOnce[KEY] = false))

void glfwWindowResizeCallback(GLFWwindow *window, int width, int height);
void glfwFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

void setupContext();
void destroyContext();

void render(vector<RigidBody> * state);
void input(float dt);

static int width = 640;
static int height = 640;

// default = fixed; 1 = dynamic; 2 = gafferongames fixed; 3 = gafferongames semi-fixed
int timeStepMethod = 2;
float timeStep = 0.01;

// slow motion does not work for fixed time steps. (That's fine!)
bool slowMotion;

// to pause the simulation
bool pause;

// render debug information (CoM only currently)
bool debug;

// render the octrees of the spinning tops
bool showOctree;

GLFWwindow *window;

Camera camera;
PointLight light;

Body skybox;

Body plane;
Body table;

Material debugMaterial;
Body debugPoint;

Simulation simulation;

void resetCamera()
{
    camera.setPosition(glm::vec3(0.0f, 8.f, 15.f));
    camera.setOrientation(fquat(1.0,0,0,0));
    camera.pitch(10 * M_PI / 180.0f);
    camera.setAspectRatio((float)width/height);
}

void addTorque(RigidBody * rb, vec3 force) {
    vec3 F1 = force;
    vec3 F2 = -1.f * force;
    
    vec3 P1 = rb->getPosition() + glm::vec3(1, 0, 0);
    vec3 P2 = rb->getPosition() + glm::vec3(-1, 0, 0);
    
    F1 = mat3(rb->model()) * F1;
    F2 = mat3(rb->model()) * F2;
    
    P1 = mat3(rb->model()) * P1;
    P2 = mat3(rb->model()) * P2;
    
    rb->addForce(F1, P1);
    rb->addForce(F2, P2);
}

// return (1-alpha) * fromState + alpha * toState;
// interpolates positiona and orientation
void interpolateStates(vector<RigidBody> * fromState, vector<RigidBody> * toState, vector<RigidBody> *interpolatedState, float alpha)
{
    if (interpolatedState->size() > 0) {
        interpolatedState->clear();
    }
    
    if (fromState->size() != toState->size()) {
        interpolatedState = toState;
    }
    else {
        for (int i = 0; i < fromState->size(); i++) {
            RigidBody current = fromState->at(i);
            
            current.setPosition(mix(fromState->at(i).getPosition(), toState->at(i).getPosition(), alpha));
            current.setOrientation(mix(fromState->at(i).getOrientation(), toState->at(i).getOrientation(), alpha));
            
            interpolatedState->push_back(current);
        }
    }
}

int main()
{
    time_t begin = time(0);
    
	setupContext();
	Assets::init();
    
    if (timeStepMethod == 2) { // turn off vsync or so..
        glfwSwapInterval(0);
    }
    
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
    
    // STENCIL BUFFER
    glEnable(GL_STENCIL_TEST);
    
    skybox = Body(glm::vec3(0.0));
    skybox.setScale(glm::vec3(250.0));
    skybox.setMesh(&Assets::skyboxBox);
    skybox.setTexture(&Assets::skybox);
    skybox.setPosition(vec3(0, 0, 0));
    
    plane = Body(glm::vec3(0, 0, 0));
    plane.setScale(glm::vec3(10, 1, 10));
    plane.setMesh(&Assets::plane);
    plane.setMaterial(&Assets::planeMaterial);
    
    table = Body(glm::vec3(0, -1, 0));
    table.setScale(glm::vec3(15, 1, 15));
    table.setMesh(&Assets::cube);
    table.setMaterial(&Assets::whiteMaterial);
    table.setTexture(&Assets::darkWood);
    
    debugMaterial = Material();
    
    debugPoint = Body(glm::vec3(0.0));
    debugPoint.setScale(glm::vec3(0.1));
    debugPoint.setMesh(&Assets::sphere);
    debugPoint.setMaterial(&debugMaterial);
    
    simulation = Simulation();

    glfwGetFramebufferSize(window, &width, &height);
    printf("Framebuffer width: %d height: %d\n", width, height);
    
    resetCamera();

    light = PointLight(glm::vec3(0, 10, 0));
    
    pause = false;
    
    debug = false;
    
    double accumulator = 0.0;
    //forwardStep(timeStep);
    
	while (!glfwWindowShouldClose(window)) {
        
		// Timer
		static double previous = glfwGetTime();
		double current = glfwGetTime();
		float deltaTime = (float) current - (float) previous;
        previous = current;
        
        if (slowMotion) {
            deltaTime /= 8;
        }
        
        vector<RigidBody> renderState = *simulation.getCurrentState();
        
        double tBeforeUpdate = glfwGetTime();
        
        if (!pause)
        {
            //printf("update call\n");
            if (timeStepMethod == 0)
            {
                simulation.forwardStep(timeStep);
                renderState = *simulation.getCurrentState();
            }
            if (timeStepMethod == 1)
            {
                simulation.forwardStep(deltaTime);
                renderState = *simulation.getCurrentState();
            }
            else if (timeStepMethod == 2)
            {
                if (deltaTime > 0.25)
                {
                    deltaTime = 0.25;
                    printf("Warning: deltaTime is too big!\n");
                }
                
                accumulator += deltaTime;
                
//                printf("accumulator: %f\n", accumulator);
                while (accumulator >= timeStep) {
                    simulation.forwardStep(timeStep);
                    accumulator -= timeStep;
                }
                
                double alpha = accumulator / timeStep;
                
                if (simulation.getNumberOfStates() > 1)
                {
                    vector<RigidBody> interpolatedState = vector<RigidBody>();
                    
                    interpolateStates(simulation.getLastState(), simulation.getCurrentState(), &interpolatedState, alpha);
                    
                    renderState = interpolatedState;
                }
                else {
                    renderState = *simulation.getCurrentState();
                }
            }
            else if (timeStepMethod == 3)
            {
                while (deltaTime > 0.0) {
                    float actualDeltaTime = glm::min(deltaTime, timeStep);
                    simulation.forwardStep(actualDeltaTime);
                    renderState = *simulation.getCurrentState();
                    deltaTime -= actualDeltaTime;
                }
            }
        }
        
        if (slowMotion) {
            deltaTime = deltaTime * 8;
        }
        
        input(deltaTime);
        
        double tAfterUpdate = glfwGetTime();
        
        double tBeforeRender = glfwGetTime();
        
        render(&renderState);
        
        double tAfterRender = glfwGetTime();
        
        double updateTime = tAfterUpdate - tBeforeUpdate;
        double renderTime = tAfterRender - tBeforeRender;
        
//        printf("time\tupdate: %f\trender: %f\n", updateTime, renderTime);
        
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Assets::destroy();
	destroyContext();
    time_t endtime = time(0);
    
    printf("The program ran for %d seconds.\n", (int)difftime(endtime, begin));
	return 0;
}

void render(vector<RigidBody> * state) {
    // clear drawing surface
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    glStencilMask(0xFF);
    
    glViewport(0, 0, width, height);
    
    Assets::skyboxShader.use();
    
    camera.setUniforms();
    
    skybox.render();
    
    Assets::textureShader.use();
    
    light.setUniforms();
    camera.setUniforms();
    
    for (int i = 0; i < state->size(); ++i)
    {
        state->at(i).render();
    }
    
    table.render();
    
    Assets::phongShader.use();
    
    light.setUniforms();
    camera.setUniforms();
    
    if (showOctree)
    {
        for (int i = 0; i < state->size(); ++i) {
            state->at(i).renderOctree();
        }
    }
    
    if (debug) {
        glDisable(GL_DEPTH_TEST);
        
        vector<DebugPoint> debugPoints = simulation.getDebugPoints();
        for (int i = 0; i < debugPoints.size(); i++) {
            debugMaterial.setColor(debugPoints[i].color);
            debugPoint.setPosition(debugPoints[i].position);
            
            debugPoint.render();
        }
        
        glEnable(GL_DEPTH_TEST);
        
//        plane.render();
    }
    
    Assets::shadowShader.use();
    
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glStencilMask(0xFF);
    
    light.setUniforms();
    camera.setUniforms();
    Shader::setUniform("shadowColor", glm::vec4(0, 0, 0, 0.6));
    
    for (int i = 0; i < state->size(); ++i)
    {
        state->at(i).render();
    }
}

void input(float dt) {
    
    if (glfwGetKeyOnce(window, GLFW_KEY_PERIOD)) {
        debug = !debug;
    }
    
    if (glfwGetKeyOnce(window, GLFW_KEY_O)) {
        showOctree = !showOctree;
    }
    
    if (glfwGetKeyOnce(window, GLFW_KEY_TAB)) {
        simulation.toggleActiveRigidBody();
    }
    
    if (!pause) {
        
        if (glfwGetKeyOnce(window, GLFW_KEY_1)) {
            simulation.addRigidBody(1);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_2)) {
            simulation.addRigidBody(2);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_3)) {
            simulation.addRigidBody(3);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_4)) {
            simulation.addRigidBody(4);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_5)) {
            simulation.addRigidBody(5);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_6)) {
            simulation.addRigidBody(6);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_9)) {
            simulation.addRigidBody(9);
        }
        else if (glfwGetKeyOnce(window, GLFW_KEY_0)) {
            simulation.addRigidBody(0);
        }
        
        if (glfwGetKeyOnce(window, GLFW_KEY_BACKSPACE)) {
            simulation.removeActiveRigidBody();
        }
        
        if (glfwGetKeyOnce(window, GLFW_KEY_Q)) {
            simulation.removeAllRigidBodies();
        }
        
        
        // Control spinning top
        RigidBody * spinningTop = simulation.getActiveRigidBody();
        if (spinningTop != nullptr) {
            
            if (glfwGetKey(window, GLFW_KEY_T)) {
                addTorque(spinningTop, vec3(0,0,10) * dt/timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_R)) {
                addTorque(spinningTop, vec3(0,0,-10) * dt/timeStep);
            }
            
            if (glfwGetKey(window, GLFW_KEY_U)) {
                spinningTop->addForce(glm::vec3(0, 0, -10) * dt/timeStep, spinningTop->getPosition());
            }
            if (glfwGetKey(window, GLFW_KEY_H)) {
                spinningTop->addForce(glm::vec3(-10, 0, 0) * dt/timeStep, spinningTop->getPosition());
            }
            if (glfwGetKey(window, GLFW_KEY_J)) {
                spinningTop->addForce(glm::vec3(0, 0, 10) * dt/timeStep, spinningTop->getPosition());
            }
            if (glfwGetKey(window, GLFW_KEY_K)) {
                spinningTop->addForce(glm::vec3(10, 0, 0) * dt/timeStep, spinningTop->getPosition());
            }
            if (glfwGetKey(window, GLFW_KEY_Y)) { // Is actually Z on a swiss/german keyboard
                spinningTop->addForce(glm::vec3(0, 20, 0) * dt/timeStep, spinningTop->getPosition());
            }
            if (glfwGetKey(window, GLFW_KEY_I)) {
                spinningTop->addForce(glm::vec3(5, 0, 0) * dt/timeStep, spinningTop->getPosition() + glm::vec3(0, 1, 0));
            }
        }
    }

    // Camera
    
    float deltaX = camera.getSpeed() * dt;
    
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        camera.moveUpDown(-deltaX);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) {
        camera.moveUpDown(deltaX);
    }
    
    
    if (glfwGetKey(window, GLFW_KEY_W)) {
        camera.moveForwardBackward(deltaX);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera.moveLeftRight(deltaX);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        camera.moveForwardBackward(-deltaX);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        camera.moveLeftRight(-deltaX);
    }
    
    if (glfwGetKey(window, GLFW_KEY_C)) {
        resetCamera();
    }
    
//    skybox.setPosition(camera.getPosition());
    
    float deltaTheta = camera.getRotationSpeed() * dt;
    
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
    
    if (glfwGetKeyOnce(window, GLFW_KEY_X)) {
        slowMotion = !slowMotion;
        if (slowMotion) {
            printf("Slow motion turned on.\n");
        } else {
            printf("Slow motion turned off.\n");
        }
    }
    
    if (glfwGetKeyOnce(window, GLFW_KEY_P))
    {
        pause = !pause;
    }
    
    if (pause) {
        if (glfwGetKeyOnce(window, GLFW_KEY_N)) {
            simulation.forwardStep(timeStep);
        }
        
        if (glfwGetKey(window, GLFW_KEY_N) && glfwGetKey(window, GLFW_KEY_M)) {
            simulation.forwardStep(timeStep);
        }
        if (glfwGetKey(window, GLFW_KEY_B) && glfwGetKey(window, GLFW_KEY_M)) {
            simulation.backwardStep();
        }
        
        if (glfwGetKeyOnce(window, GLFW_KEY_B)) {
            simulation.backwardStep();
        }
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

	window = glfwCreateWindow(width, height, "Spinning spinning tops and other things", NULL, NULL);

	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
	}

    glfwSetWindowPos(window, 0, 0);
    
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