#include "globals.hpp"
#include "collisions.hpp"

void collideCameraWithMap(glm::vec4& position, float mapData[64][64]) {
    int x = (int)floor(position.x)+32;
    int z = (int)floor(position.z)+32;

    if (x >= 0 && x < 64 && z >= 0 && z < 64) {
        if (position.y < mapData[x][z]+1) {
            position.y = (float)mapData[x][z]+1;
        }
    }
}
