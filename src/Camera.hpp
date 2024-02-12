#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm-master/glm/glm.hpp>

class Camera
{
private:
    /* data */
    glm::vec3 mEye;
    glm::vec3 mViewDirection;
    glm::vec3 mUpVector;
    glm::vec2 mOldMousePosition;

public:
    glm::mat4 GetViewMatrix() const;

    void MouseLook(int mouseX, int mouseY);
    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);

    // Default constructor
    Camera(/* args */);
    ~Camera();
};

#endif