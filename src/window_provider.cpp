#include "window_provider.hpp"

WindowProvider::WindowProvider(int width, int height, string title){
    this->width = width;
    this->height = height;
    this->title = title;
};

GLFWwindow* WindowProvider::initWindow(GLFWerrorfun errorCallback, GLFWkeyfun keyCallback, GLFWmousebuttonfun mouseButtonCallback, GLFWcursorposfun cursorPosCallback, GLFWscrollfun scrollCallback, GLFWframebuffersizefun framebufferSizeCallback){
    int success = glfwInit();

    if (!success){
        printf("ERROR glfwInit()!\n");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            
    if (!window){
        glfwTerminate();
        printf("ERROR glfwCreateWindow()!\n");
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback); 
    glfwSetScrollCallback(window, scrollCallback);
            
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetWindowSize(window, width, height);

    glfwMakeContextCurrent(window);
            
    return window;
}
