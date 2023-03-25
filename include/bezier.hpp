#ifndef BEZIER_H
#define BEZIER_H

#include <glm/mat4x4.hpp>

class BezierCurve {
    public:
        glm::vec4 calculate(glm::vec4 p0, glm::vec4 p1, glm::vec4 c0, glm::vec4 c1, float delta);
};

#endif