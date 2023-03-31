#include <cstdio>
#include <cstdlib>
#include <limits>

#include "globals.hpp"

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height){
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    camera.updateScreenRatio((float) width / height);
}

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if (!windowIsFocused) {
            windowIsFocused = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else {
            // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
            // posição atual do cursor nas variáveis g_LastCursorPosX e
            // g_LastCursorPosY.  Também, setamos a variável
            // g_LeftMouseButtonPressed como true, para saber que o usuário está
            // com o botão esquerdo pressionado.
            glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
            g_LeftMouseButtonPressed = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (!windowIsFocused) return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    camera.updateMouse(dx, dy);

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = 400;
    g_LastCursorPosY = 400;

    glfwSetCursorPos(window, 400, 400);   
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    float dy = 0.1f * yoffset;

    camera.updateDistance(dy);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod){
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    camera.setUpdatingPosition(none);

    // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS) camera.changeMode();

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) camera.setUpdatingPosition(up);
    else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) camera.setUpdatingPosition(down);
    else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) camera.setUpdatingPosition(left);
    else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) camera.setUpdatingPosition(right);

    if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT)) shellRotate.y+=0.1f;
    if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT)) shellRotate.y-=0.1f;
    // smooth translation up arrow key, using acceleration x axis based on time, change shellPosition to change speed using multiplication for accel
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) CowMovement(0);
    // smooth translation down arrow key, using acceleration x axis based on time, change shellPosition to change speed using multiplication for accel
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) CowMovement(1);
    // smooth translation left arrow key, using acceleration x axis based on time, change shellPosition to change speed using multiplication for accel
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) CowMovement(2);
    // smooth translation right arrow key, using acceleration x axis based on time, change shellPosition to change speed using multiplication for accel
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) CowMovement(3);
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description){
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
