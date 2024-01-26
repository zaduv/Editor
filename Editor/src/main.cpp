#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

bool rightMouseButton = false;
bool leftMouseButton = false;
bool paint = false;

double lastX;
double lastY;
int lastWidth;
int lastHeight;

#include "camera.h"
#include "loader.h"
#include "volume.h"
#include "grid.h"
#include "tool.h"

void processInput(GLFWwindow* window, float deltaTime);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Camera camera(glm::vec3(-30.5f, 50.0f, 30.5f));
Grid grid;
Tool tool;

glm::vec3 origin;
glm::vec3 direction;

int main()
{
    // Initialize the GLFW library
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, true);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(640, 480, "", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set needed callbacks
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW to setup the OpenGL Function pointers
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Retrieves information about window
    glfwGetFramebufferSize(window, &lastWidth, &lastHeight);
    glfwGetCursorPos(window, &lastX, &lastY);

    float currentFrame = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float framesPerSecond = 0.0f;
    float lastTime = 0.0f;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 MVP;

    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(lastWidth) / static_cast<float>(lastHeight), 1.0f, 1000.0f);
    view = camera.get3rdPersonMatrix();

    origin = ExtractCameraPos(view);
    direction = glm::normalize(glm::inverse(view) * glm::vec4(glm::vec2(glm::inverse(projection) * glm::vec4((2.0f * lastX) / lastWidth - 1.0f, 1.0f - (2.0f * lastY) / lastHeight, -1.0, 1.0)), -1.0, 0.0));

    grid.init();
    grid.updateIntersection(origin, direction);

    tool.init();

    Volume volume;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Show frame rate in the title
        ++framesPerSecond;
        if (currentFrame - lastTime > 1.0f)
        {
            glfwSetWindowTitle(window, ("Current frames per second: " + std::to_string((int)framesPerSecond)).c_str());
            lastTime = currentFrame;
            framesPerSecond = 0.0f;
        }

        processInput(window, deltaTime);

        // Calculate projection and view matrices
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(lastWidth) / static_cast<float>(lastHeight), 1.0f, 1000.0f);
        view = camera.get3rdPersonMatrix();
        model = glm::mat4(1.0f);
        MVP = projection * view * model;

        // Calculate origin and direction
        origin = ExtractCameraPos(view);
        direction = glm::normalize(glm::inverse(view) * glm::vec4(glm::vec2(glm::inverse(projection) * glm::vec4((2.0f * lastX) / lastWidth - 1.0f, 1.0f - (2.0f * lastY) / lastHeight, -1.0, 1.0)), -1.0, 0.0));

        // Allow paint
        if (leftMouseButton && tool.getIndex() == -1)
        {
            paint = true;
        }

        if (!leftMouseButton)
        {
            paint = false;
        }

        tool.updateTool(origin, direction, currentFrame);
        grid.updateIntersection(origin, direction);

        camera.Center = 0.5f * (grid.getMin() + grid.getMax());
        tool.setPosition(grid.getMin());

        if (grid.rayAABBIntersection(origin, 1.0f / direction) > 0.0f)
        {
            volume.paintVolume(grid.getIntersection(), grid.getLastIntersection(), tool.getColor(), paint); 
        }

        volume.rebuildPyramid(grid.getMin(), grid.getMax());

        // Render scene as normal
        glViewport(0, 0, lastWidth, lastHeight);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        volume.renderPyramid(MVP, origin);
        grid.renderGrid(MVP, origin);
        tool.renderTool(MVP, origin);

        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate, clearing all previously allocated GLFW resources
    glfwTerminate();

    return 0;
}

// Query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
}

// Whenever the mouse moves, this callback is called
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.processMouseMovement(xoffset, yoffset, true);
    }
}

// Whenever the mouse button is pressed, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        rightMouseButton = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        rightMouseButton = false;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        leftMouseButton = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        leftMouseButton = false;
}

// Whenever a key is pressed, this callback is called
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        tool.goToNext(glfwGetTime());
}

// Whenever the mouse is scrolled, this callback function executes
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.processMouseScroll(yoffset);
    }
    else
    {
        grid.processScroll(yoffset, origin, direction);
    }
}

// Whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    lastWidth = width;
    lastHeight = height;
}