#include "Camera.hpp"
#include <glm-master/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm-master/glm/gtx/rotate_vector.hpp>
#include <iostream>

Camera::Camera(/* args */)
{
    mEye = glm::vec3(0.0f, 0.0f, 0.0f);
    mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
}

void Camera::MouseLook(int mouseX, int mouseY)
{
    static bool firstLook = true;
    glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);
    if (firstLook)
    {
        mOldMousePosition = currentMouse;
        firstLook = false;
    }
    glm::vec2 mouseDelta = (mOldMousePosition - currentMouse);
    mViewDirection = glm::rotate(mViewDirection, glm::radians(mouseDelta.x * .01f), mUpVector);
    std::cout << "mouseX: " << mouseX << ","
              << "mouseY: " << mouseY << std::endl;
}

void Camera::MoveForward(float speed)
{
    mEye += (mViewDirection * speed);
}
void Camera::MoveBackward(float speed)
{
    mEye -= (mViewDirection * speed);
}
void Camera::MoveLeft(float speed)
{
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
    mEye -= rightVector * speed;
}
void Camera::MoveRight(float speed)
{
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
    mEye += rightVector * speed;
}