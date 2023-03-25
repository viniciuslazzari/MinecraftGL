#include "bezier.hpp"

glm::vec4 BezierCurve::calculate(glm::vec4 p0, glm::vec4 p1, glm::vec4 c0, glm::vec4 c1, float delta){
    glm::vec4 vecP0C0 = p0 + delta * (c0 - p0);
    glm::vec4 vecP1C1 = p1 + delta * (c1 - p1);

    glm::vec4 vecC0C1 = c0 + delta * (c1 - c0);

    glm::vec4 aux0 = vecP0C0 + delta * (vecC0C1 - vecP0C0);
    glm::vec4 aux1 = vecP1C1 + delta * (vecC0C1 - vecP1C1);

    return aux0 + delta * (aux1 - aux0);
};