#include "Camera.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Camera::Camera() {
	m_position = glm::vec3(0, 0, 0);
	m_orientation = glm::fquat(1, 0, 0, 0);
	m_fov = 50.0f * M_PI / 180.0f;
	m_aspect = 4.0f/3.0f;
	m_nearPlane = 0.1f;
	m_farPlane = 500.0f;
    m_speed = 8.0f;
    m_rotationSpeed = 100.0f * M_PI / 180.0f;
}

const glm::vec3 Camera::getPosition() const {
	return m_position;
}

const glm::vec3 Camera::right() const {
	return glm::vec3(glm::row(rotation(), 0));
}

const glm::vec3 Camera::up() const {
    return glm::vec3(0, 1, 0);
}

const glm::vec3 Camera::forward() const {
	return glm::vec3(glm::row(rotation(), 2));
}

void Camera::setPosition(const glm::vec3 & position) {
	m_position = position;
}

// Sets camera orientation
void Camera::setOrientation(const glm::fquat & orientation)
{
    m_orientation = orientation;
}

// Set camera speed
void Camera::setSpeed(const float speed) {
    m_speed = speed;
}

void Camera::setRotationSpeed(const float speed) {
    m_rotationSpeed = speed;
}

float Camera::getSpeed() const {
    return m_speed;
}

float Camera::getRotationSpeed() const {
    return m_rotationSpeed;
}

void Camera::move(const glm::vec3 & delta) {
	m_position += delta;
}

void Camera::moveLeftRight(float delta) {
	m_position -= delta * right();
}

void Camera::moveUpDown(float delta) {
	m_position -= delta * up();
}

void Camera::moveForwardBackward(float delta) {
	m_position -= delta * forward();
}

void Camera::rotate(float angle, glm::vec3 & axis) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, axis));
	
	m_orientation = m_orientation * rot;
}

void Camera::pitch(float angle) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, right()));

	m_orientation = m_orientation * rot;
}

void Camera::yaw(float angle) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, up()));

	m_orientation = m_orientation * rot;
}

glm::mat4 Camera::projection() const {
	return glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::translation() const {
	return glm::translate(glm::mat4(), -m_position);
}

glm::mat4 Camera::rotation() const {
	return glm::toMat4(m_orientation);
}

glm::mat4 Camera::view() const {
	return rotation() * translation();
}

glm::mat4 Camera::matrix() const {
	return projection() * view();
}

void Camera::setFieldOfView(float fieldOfView) {
	m_fov = fieldOfView;
}

void Camera::setAspectRatio(float aspectRatio) {
	m_aspect = aspectRatio;
}

void Camera::setNearFarPlanes(float near, float far) {
	m_nearPlane = near;
	m_farPlane = far;
}

void Camera::setUniforms() {
    Shader::setUniform("viewMatrix", view());
    Shader::setUniform("projectionMatrix", projection());
    Shader::setUniform("cameraPosition", getPosition());
}