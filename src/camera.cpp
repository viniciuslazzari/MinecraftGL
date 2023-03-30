#include "camera.hpp"
#include "globals.hpp"
#include "collisions.hpp"
#include "std/matrices.h"

const float phimax = 3.141592f / 2;
const float phimin = -phimax;

Camera::Camera(float speed, float distance, glm::vec4 positionFree, glm::vec4 positionLook, glm::vec4 viewFree, glm::vec4 viewLook){
    this->speed = speed;
    this->distance = distance;
    this->positionFree = positionFree;
    this->positionLook = positionLook;
    this->viewFree = viewFree;
    this->viewLook = viewLook;
}

void Camera::setUpdatingPosition(Direction direction){
    this->updatingPosition = direction;
}

void Camera::move(){
    Direction direction = this->updatingPosition;

    if (direction == none) return;

    glm::vec4 w = -this->viewFree / norm(-this->viewFree);
    glm::vec4 u = crossproduct(this->upVector, w) / norm(crossproduct(this->upVector, w));

    switch(direction){
        case(up): this->positionFree += -w * (float)(deltaTime * this->speed); break;
        case(down): this->positionFree += w * (float)(deltaTime * this->speed); break;
        case(left): this->positionFree += -u * (float)(deltaTime * this->speed); break;
        case(right): this->positionFree += u * (float)(deltaTime * this->speed); break;
        default: break;
    }

}


float Camera::returnX(){
    float phi = this->isFree ? this->phiFree : this->phiLook;
    float tetha = this->isFree ? this->tethaFree : this->tethaLook;

    float r = this->distance;

    return -r * cos(phi) * sin(tetha);
}

float Camera::returnY(){
    float phi = this->isFree ? this->phiFree : this->phiLook;
    float r = this->distance;

    return -r * sin(phi);
}

float Camera::returnZ(){
    float phi = this->isFree ? this->phiFree : this->phiLook;
    float tetha = this->isFree ? this->tethaFree : this->tethaLook;

    float r = this->distance;

    return -r * cos(phi) * cos(tetha);
}

void Camera::updateMouse(float dx, float dy){
    if (this->isFree){
        this->tethaFree -= 0.0005f * dx;
        this->phiFree += 0.0005f * dy;

        if (this->phiFree > phimax) this->phiFree = phimax;
        if (this->phiFree < phimin) this->phiFree = phimin;

        return;
    }

    this->tethaLook -= 0.0005f * dx;
    this->phiLook += 0.0005f * dy;

    if (this->phiLook > phimax) this->phiLook = phimax;
    if (this->phiLook < phimin) this->phiLook = phimin;
}

glm::mat4 Camera::getViewFree(){
    float x = this->returnX();
    float y = this->returnY();
    float z = this->returnZ();

    this->viewFree = glm::vec4(x, y, z, 0.0f);

    return Matrix_Camera_View(this->positionFree, this->viewFree, this->upVector);
}

glm::mat4 Camera::getViewLook(){
    float x = this->returnX();
    float y = this->returnY();
    float z = this->returnZ();

    this->positionLook = glm::vec4(x, y, z, 1.0f);
    glm::vec4 lookAt = glm::vec4(0.0f, -20.0f, 0.0f, 1.0f);
    this->viewLook = lookAt - this->positionLook;

    return Matrix_Camera_View(this->positionLook, this->viewLook, this->upVector);
}

glm::mat4 Camera::getView(){
    glm::mat4 view;

    if (this->isFree){
        this->move();
        view = this->getViewFree();
    } else {
        view = this->getViewLook();
    }

    return view;
}

void Camera::changeMode(){
    this->isFree = !this->isFree;
}

void Camera::updateDistance(float dy){
    this->distance -= dy;
}

glm::mat4 Camera::getProjection(){
    return Matrix_Perspective(this->fieldOfView, this->screenRatio, this->near, this->far);
}

void Camera::updateScreenRatio(float screenRatio){
    this->screenRatio = screenRatio;
}
