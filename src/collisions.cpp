#include "collisions.hpp"
#include "globals.hpp"
#include <iostream>

// map block collision
#define DISTANCE 2.0f

glm::vec4 previousCameraPosition;

// point-cube collision
void collideCameraWithMap(glm::vec4 &position, glm::vec4 mapData[MAP_SIZE][MAP_SIZE]) {
    int max_coord = MAP_SIZE / 2;
    int x = static_cast<int>(floor(position.x)) + max_coord;
    int z = static_cast<int>(floor(position.z)) + max_coord;

    if (x < 1.5f) {
        position.x = -max_coord+DISTANCE;
    } else if (x >= MAP_SIZE) {
        position.x = max_coord-0.5f;
    } else if (z < 1.5f) {
        position.z = -max_coord+DISTANCE;
    } else if (z >= MAP_SIZE) {
        position.z = max_coord-0.5f;
    } else {
        float blockTop = mapData[x][z].y + DISTANCE;

        if (position.y < blockTop) {
            position.y = blockTop;
        } else {
            float blockLeft = mapData[x][z].x;
            float blockRight = blockLeft + DISTANCE;
            float blockFront = mapData[x][z].z;
            float blockBack = blockFront + DISTANCE;

            if (position.x < blockLeft) {
                position.x = blockLeft;
            } else if (position.x > blockRight) {
                position.x = blockRight;
            }

            if (position.z < blockFront) {
                position.z = blockFront;
            } else if (position.z > blockBack) {
                position.z = blockBack;
            }
        }
    }
}

// point-sphere collision
void collideCameraWithShell(glm::vec4 &cameraPosition, glm::vec3 &shellPosition) {
    // posicao da camera interna da funcao
    glm::vec3 cameraPos = glm::vec3(cameraPosition);

    // posicao da vaca e estimativa do bounding box
    glm::vec3 cowPos = glm::vec3(shellPosition);
    float cowRadius = 2.0f;

    // Checa colisao da vaca com esfera de hit
    if (glm::distance(cameraPos, cowPos) < cowRadius) {
        std::cout << "\n\n";
        std::cout << "╔══════════════════════════╗\n";
        std::cout << "║      PARABENS! VOCE      ║\n";
        std::cout << "║          GANHOU!         ║\n";
        std::cout << "╚══════════════════════════╝\n";
        std::cout << "\n\n";
        win = true;
    }
}

// cube-cube collision
bool collideShellWithMap(glm::vec3 &shellPosition, glm::vec4 mapData[MAP_SIZE][MAP_SIZE]) {
    int x = int(shellPosition.x) - 1 + MAP_SIZE / 2;
    int z = int(shellPosition.z) - 1 + MAP_SIZE / 2;
    float threshold = 0.6f;

    if (shellPosition.y < mapData[x - 1][z].y + threshold ||
        shellPosition.y < mapData[x - 2][z].y + threshold ||
        shellPosition.y < mapData[x - 3][z].y + threshold) {
        return true;
    }
    if (shellPosition.y < mapData[x][z - 1].y + threshold ||
        shellPosition.y < mapData[x][z - 2].y + threshold ||
        shellPosition.y < mapData[x][z - 3].y + threshold) {
        return true;
    }

    return false;
}
