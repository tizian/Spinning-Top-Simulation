#define GLM_FORCE_RADIANS

#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// Quaternion based, free moving camera.

class Camera
{
public:
    Camera();

    const glm::vec3 getPosition()   const;
    const glm::vec3 right()     const;
    const glm::vec3 up()        const;
    const glm::vec3 forward()   const;

    // Sets camera position
    void setPosition(const glm::vec3 & position);

    // Sets camera orientation
    void setOrientation(const glm::fquat & orientation);
    
    // Set camera speed
    void setSpeed(const float speed);
    void setRotationSpeed(const float speed);
    
    // Get camera speed
    float getSpeed() const;
    float getRotationSpeed() const;
    
    // Moves camera using world space axes
    void move(const glm::vec3 & delta);

    // Moves camera relative to itself
    void moveLeftRight(float delta);
    void moveUpDown(float delta);
    void moveForwardBackward(float delta);

    // Rotates camera by an angle around an axis
    void rotate(float angle, glm::vec3 & axis);

    // Performs pitch, yaw, roll by an angle
    void pitch(float angle);
    void yaw(float angle);
    // void roll(float angle);

    // Returns only the projection matrix
    glm::mat4 projection() const;

    // Returns only the tranlation matrix
    glm::mat4 translation() const;

    // Returns only the rotation matrix
    glm::mat4 rotation() const;

    // Returns the view matrix (view = rotation * translation)
    glm::mat4 view() const;

    // Returns the camera transformation matrix (projection * rotation * tranlation = projection * view)
    glm::mat4 matrix() const;

    // Sets projection settings
    void setFieldOfView(float fieldOfView);
    void setAspectRatio(float aspectRatio);
    void setNearFarPlanes(float near, float far);
    
    // Sets appropriate uniform values for the given shader program
    void setUniforms();
    
private:
    glm::vec3 m_position;

    glm::fquat m_orientation;

    float m_fov;
    float m_aspect;

    float m_nearPlane;
    float m_farPlane;
    
    float m_speed;
    float m_rotationSpeed;
};