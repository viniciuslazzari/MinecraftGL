#ifndef WINDOW_PROVIDER_H
#define WINDOW_PROVIDER_H

#include <string>
#include <glfw/glfw3.h> 

using namespace std;

class WindowProvider {
    private:
        int width;
        int height;
        string title;

    public:
        WindowProvider(int width, int height, string title);

        GLFWwindow* initWindow(GLFWerrorfun errorCallback, GLFWkeyfun keyCallback, GLFWmousebuttonfun mouseButtonCallback, 
                               GLFWcursorposfun cursorPosCallback, GLFWscrollfun scrollCallback, GLFWframebuffersizefun framebufferSizeCallback);
};

#endif
