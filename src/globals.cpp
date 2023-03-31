#include "globals.hpp"

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTriangles() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

bool windowIsFocused = false;
bool win = false;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;

double g_LastCursorPosX = 0.0f;
double g_LastCursorPosY = 0.0f;

double deltaTime = 0.0f;

glm::vec4 camera_position_free = glm::vec4(-1.0f, 1.0f, 5.0f, 1.0f);
glm::vec4 camera_position_look = glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
glm::vec4 camera_view_free = glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
glm::vec4 camera_view_look = glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);

Camera camera = Camera(10.0f, 2.5f, camera_position_free, camera_position_look, camera_view_free, camera_view_look);
glm::vec4 mapData[MAP_SIZE][MAP_SIZE] = {glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)};
glm::vec3 shellPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 shellRotate = glm::vec3(0.0f,0.0f,0.0f);

void ShellMovement(int direction){
    float speed = 20.0f;

    if (direction == 0) {
        shellPosition.x += deltaTime * speed;
    }
    else if (direction == 1) {
        shellPosition.x -= deltaTime * speed;
    }
    else if (direction == 2) {
        shellPosition.z -= deltaTime * speed;
    }
    else if (direction == 3) {
        shellPosition.z += deltaTime * speed;
    }
}
