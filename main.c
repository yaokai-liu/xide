#include "runtime.h"
#include "shader.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

GLFWmonitor * switchMonitor(int index, int * width, int * height) {
    int monitorCount;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
    rt_message("found monitors: %d", monitorCount);
    for (int i = 0; i < monitorCount; i++) {
        const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
        rt_message("size of monitor[i]: %dx%d", mode->width, mode->height);
    }
    GLFWmonitor * monitor = monitors[index];
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[index]);
    *width = mode->width;
    *height = mode->height;
    rt_message("switch to monitor[i]: %dx%d", mode->width, mode->height);
    return monitor;
}

extern DrawTask * TASK;
GLfloat viewSize[2] = {0.0f, 0.0f};
extern iXGLshProg BUILTIN_GRADUAL_SHADER_PROGRAM;
int main(int argc, char * argv[]) {

    const Allocator * const allocator = &STDAllocator;

    if (!glfwInit()) { return -1; }
    rt_message("Using GLFW Version: %d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
    // Required OpenGL version: 4.6.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    int width = 200, height = 200; GLFWmonitor *monitor = NULL;
    if (argc == -1) { monitor = switchMonitor(0, &width, &height); }
    GLFWwindow * glfwWindow = glfwCreateWindow(width, height, "glfwWindow title", monitor, NULL);
    if (!glfwWindow) {
        glfwTerminate();
        return -1;
    }
    glfwSwapInterval(1);
    glfwSetWindowSizeCallback(glfwWindow, setWindowSize);
    glfwSetWindowRefreshCallback(glfwWindow, windowRefreshCallback);
    glfwMakeContextCurrent(glfwWindow);

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!status) { rt_error("failed to initialize GLAD: %d", 0); }
    rt_message("OpenGL Vendor: %s", glGetString(GL_VENDOR));
    rt_message("Using OpenGL Version: %s", glGetString(GL_VERSION));
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags){
        rt_message("%s", "Debug Enabled");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                              0, nullptr, GL_TRUE);
    }

    // shader program
    GLuint vertexShader = compileShader("shader/vertex.glsl", GL_VERTEX_SHADER, allocator);
    GLuint fragmentShader = compileShader("shader/fragment.glsl", GL_FRAGMENT_SHADER, allocator);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        rt_error("Failed to link shader program: \n%s", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    BUILTIN_GRADUAL_SHADER_PROGRAM = shaderProgram;

    // TODO: bind widget to main glfwWindow
    MainWindow *mainWindow = setupWindow(glfwWindow, allocator);

//    Line lines[] = {
//        {
//            .start = {100, 100},
//            .end = {700, 100},
//            .colors = {0x00FF00FF, 0x00FF00FF},
//        },
//        {
//            .start = {700, 100},
//            .end = {700, 300},
//            .colors = {0x00FF00FF, 0x00FF00FF},
//        },
//        {
//            .start = {700, 500},
//            .end = {100, 500},
//            .colors = {0x00FF00FF, 0x00FF00FF},
//        },
//        {
//            .start = {100, 500},
//            .end = {100, 100},
//            .colors = {0x00FF00FF, 0x00FF00FF},
//        },
//    };
//    TASK = xglCreateLines(lines, 4, 0, mainWindow);

    Vertex vertices[] = {
        {.coord = {20, 40}, .color = 0xFFFFFFFF},
        {.coord = {30, 20}, .color = 0xFFFFFFFF},
        {.coord = {70, 10}, .color = 0xFFFFFFFF},
        {.coord = {80, 30}, .color = 0xFFFFFFFF},
        {.coord = {70, 60}, .color = 0xFFFFFFFF},
        {.coord = {90, 80}, .color = 0xFFFFFFFF},
        {.coord = {60, 70}, .color = 0xFFFFFFFF},
        {.coord = {40, 60}, .color = 0xFFFFFFFF},
    };
    TASK = xglCreateSolidPolygon(vertices, 8, 0, mainWindow);
//    TASK = xglCreatePolyline(vertices, 5, false, 0, mainWindow);

//    int count = 0;
//    Array * vertices = Array_new(sizeof(Vertex), allocator);
//    for (int i = 1; i <= 100; i++) {
//        Vertex vertex = {};
//        vertex.coord[0] = (int) roundf(200 * cosf(M_PI / 100 * i)) + 400;
//        vertex.coord[1] = (int) roundf(200 * sinf(M_PI / 100 * i)) + 300;
//        vertex.color = 0xFFFFFFFF;
//        Array_append(vertices, &vertex, 1);
//    }
//    TASK = xglCreateSolidPolygon(Array_get(vertices, 0), 100, 0, mainWindow);
////    TASK = xglCreatePolyline(Array_get(vertices, 0), count, true, 0, mainWindow);

    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    viewSize[0] = (float) viewport[2];
    viewSize[1] = (float) viewport[3];

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(glfwWindow)) {
        processInput(glfwWindow);
        drawUI(glfwWindow);
        glfwPollEvents();
    }

    xglDestroyDrawTask(TASK);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
