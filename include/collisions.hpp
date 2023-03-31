#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <glm/glm.hpp>
#include "globals.hpp"

void collideCameraWithMap(glm::vec4& position, glm::vec4 mapData[MAP_SIZE][MAP_SIZE]);
void collideCameraWithShell(glm::vec4 &cameraPosition, glm::vec3 &shellPosition);
bool collideShellWithMap(glm::vec3 &shellPosition, glm::vec4 mapData[MAP_SIZE][MAP_SIZE]);

#endif 
