#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <glm/mat4x4.hpp>

enum Direction { up, down, left, right, none};

class Camera {
    private:
        const glm::vec4 upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        bool isFree = true;

        float phiFree = 0.0f;
        float tethaFree = 0.0f;
        float phiLook = 0.0f;
        float tethaLook = 0.0f;

        float near = -0.1f;
        float far = -50.0f;
        const float fieldOfView = 3.141592 / 3.0f;
        float screenRatio = 1.0f;

        Direction updatingPosition = none;

        float speed;
        float distance;

        glm::vec4 positionFree;
        glm::vec4 positionLook;

        glm::vec4 viewFree;
        glm::vec4 viewLook;

    public:
        Camera(float speed, float distance, glm::vec4 positionFree, glm::vec4 positionLook, glm::vec4 viewFree, glm::vec4 viewLook);
        void move();
        float returnX();
        float returnY();
        float returnZ();
        void updateMouse(float dx, float dy);
        glm::mat4 getViewFree();
        glm::mat4 getViewLook();
        glm::mat4 getView();
        void changeMode();
        void setUpdatingPosition(Direction direction);
        void updateDistance(float dy);
        glm::mat4 getProjection();
        void updateScreenRatio(float screenRatio);
};

#endif
