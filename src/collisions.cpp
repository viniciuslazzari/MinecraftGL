#include "collisions.hpp"
#include "globals.hpp"
// map block collision
#define DISTANCE 2.0f

// point-cube collision
void collideCameraWithMap(glm::vec4 &position,
                          glm::vec4 mapData[MAP_SIZE][MAP_SIZE]) {
  int max_coord = MAP_SIZE / 2;
  int x = static_cast<int>(floor(position.x)) + max_coord;
  int z = static_cast<int>(floor(position.z)) + max_coord;

  if (x < 0) {
    position.x = -max_coord;
  } else if (x >= MAP_SIZE) {
    position.x = max_coord;
  } else if (z < 0) {
    position.z = -max_coord;
  } else if (z >= MAP_SIZE) {
    position.z = max_coord;
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
glm::vec4 previousCameraPosition;
void collideCameraWithCow(glm::vec4 &cameraPosition, glm::vec3 &cowPosition) {
  // posicao da camera interna da funcao
  glm::vec3 cameraPos = glm::vec3(cameraPosition);

  // posicao da vaca e estimativa do bounding box
  glm::vec3 cowPos = glm::vec3(cowPosition);
  float cowRadius = 1.5f;

  // Checa colisao da vaca com esfera de hit
  if (glm::distance(cameraPos, cowPos) < cowRadius) {
    cameraPosition = previousCameraPosition;
  } else {
    previousCameraPosition = cameraPosition;
  }
}

// cube-cube collision
bool collideCowWithMap(glm::vec3 &cowPosition, glm::vec4 mapData[64][64]) {
  int x = int(cowPosition.x) - 1 + MAP_SIZE / 2;
  int z = int(cowPosition.z) - 1 + MAP_SIZE / 2;
  if (cowPosition.y < mapData[x - 1][z].y + 2 ||
      cowPosition.y < mapData[x - 2][z].y + 2 ||
      cowPosition.y < mapData[x - 3][z].y + 2) {
    return true;
  }
  if (cowPosition.y < mapData[x][z - 1].y + 2 ||
      cowPosition.y < mapData[x][z - 2].y + 2 ||
      cowPosition.y < mapData[x][z - 3].y + 2) {
    return true;
  }
  return false;
}
