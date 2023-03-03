#include <map>

#include <glad/glad.h>
#include <glfw/glfw3.h>

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
class SceneObject {
    public:
        const char* name;
        void* firstIndex;
        int numIndexes;
        GLenum renderingMode;
};

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map). Veja dentro da função BuildTriangles() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
extern std::map<const char*, SceneObject> g_VirtualScene;

extern bool windowIsFocused;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
extern float g_ScreenRatio;

extern int g_Camera;
extern float g_CameraSpeed;

// "g_LeftMouseButtonPressed se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
extern bool g_LeftMouseButtonPressed;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
extern float g_CameraThetaFree;     // Ângulo no plano ZX em relação ao eixo Z
extern float g_CameraPhiFree;       // Ângulo em relação ao eixo Y
extern float g_CameraThetaLook;     // Ângulo no plano ZX em relação ao eixo Z
extern float g_CameraPhiLook;       // Ângulo em relação ao eixo Y
extern float g_CameraDistance;  // Distância da câmera para a origem

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
extern double g_LastCursorPosX, g_LastCursorPosY;

extern bool isFreeCamera;
extern bool cameraModeChanged;