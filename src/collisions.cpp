#include "collisions.hpp"
#include "globals.hpp"

// map block collision
#define DISTANCE 2.0f

void collideCameraWithMap(glm::vec4 &position, glm::vec4 mapData[64][64]) {
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

// bounding box approx collision
glm::vec4 previousCameraPosition;
void collideCameraWithCow(glm::vec4 &cameraPosition) {
  // posicao da camera interna da funcao
  glm::vec3 cameraPos = glm::vec3(cameraPosition);

  // posicao da vaca e estimativa do bounding box
  glm::vec3 cowPos = glm::vec3(-2.0f, 0.0f, -2.0f);
  glm::vec3 cowSize = glm::vec3(2.0f, 2.0f, 2.0f);

  // ve se a camera entra na bounding box da vaca
  if (cameraPos.x > cowPos.x - cowSize.x / 2 &&
      cameraPos.x < cowPos.x + cowSize.x / 2 &&
      cameraPos.y > cowPos.y - cowSize.y / 2 &&
      cameraPos.y < cowPos.y + cowSize.y / 2 &&
      cameraPos.z > cowPos.z - cowSize.z / 2 &&
      cameraPos.z < cowPos.z + cowSize.z / 2) {
    // se entrar (colisao) retorna pra fora, fazendo efeito que colide com o
    // modelo
    cameraPosition = previousCameraPosition;
  } else {
    // se nao detectar, atualiza a ultima posicao
    previousCameraPosition = cameraPosition;
  }
}
