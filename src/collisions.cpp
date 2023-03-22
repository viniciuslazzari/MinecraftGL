#include "globals.hpp"
#include "collisions.hpp"

void collideCameraWithMap(glm::vec4& position, glm::vec4 mapData[64][64]) {
    int x = (int)floor(position.x)+MAP_SIZE/2;
    int z = (int)floor(position.z)+MAP_SIZE/2;

    if (x >= 0 && x < 64 && z >= 0 && z < 64) {
        if (position.y < mapData[x][z].y+1) {
            position.y = (float)mapData[x][z].y+1;
        }
    }
}
