#include "InputModule/IEKeyboard.hpp"
#include "GraphicsModule/VulkanOpenGL/IERenderEngine.hpp"

/**
 * @brief A key callback function. Moves the camera forward.
 * @param window
 */
void moveCameraForward(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position + renderEngine->camera.front * renderEngine->frameTime *
            renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera backward.
 * @param window
 */
void moveCameraBackward(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position - renderEngine->camera.front * renderEngine->frameTime *
            renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera right.
 * @param window
 */
void moveCameraRight(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position + renderEngine->camera.right * renderEngine->frameTime *
            renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera left.
 * @param window
 */
void moveCameraLeft(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position - renderEngine->camera.right * renderEngine->frameTime *
            renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera up.
 * @param window
 */
void moveCameraUp(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position + renderEngine->camera.up * renderEngine->frameTime * renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera down.
 * @param window
 */
void moveCameraDown(GLFWwindow* window) {
    auto renderEngine = static_cast<IERenderEngine *>(static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window))->attachment);
    renderEngine->camera.position = renderEngine->camera.position - renderEngine->camera.up * renderEngine->frameTime * renderEngine->camera.speed;
}

/**
 * @brief A key callback function. Moves the camera slower.
 * @param window
 */
void moveSlower(GLFWwindow* window);

/**
 * @brief A key callback function. Moves the camera faster.
 * @param window
 */
void moveFaster(GLFWwindow* window) {
    auto keyboard = static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window));
    auto renderEngine = static_cast<IERenderEngine *>(keyboard->attachment);
    renderEngine->camera.speed *= 3;
}

void moveSlower(GLFWwindow* window) {
    auto keyboard = static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window));
    auto renderEngine = static_cast<IERenderEngine *>(keyboard->attachment);
    renderEngine->camera.speed /= 3;
}

/**
 * @brief A key callback function. Resets the camera position and keyboard queue.
 * @param window
 */
void resetAll(GLFWwindow* window) {
    auto keyboard = static_cast<IEKeyboard *>(glfwGetWindowUserPointer(window));
    auto renderEngine = static_cast<IERenderEngine *>(keyboard->attachment);
    keyboard->clearQueue();
    renderEngine->camera = IECamera{};
    renderEngine->camera.linkedRenderEngine = &renderEngine->renderEngineLink;
}

/**
 * @brief Code for testing the entire engine with all its modular parts goes here.
 */
int main(int argc, char **argv) {
    IERenderEngine renderEngine{};
    IERenderable cube{&renderEngine.renderEngineLink, "res/Models/Cube/cube.obj"};
    cube.position = {0, -3, 0};
    renderEngine.loadRenderable(&cube);
    IEKeyboard keyboard{renderEngine.window};
    keyboard.attachment = &renderEngine;
    keyboard.editActions(GLFW_KEY_W, moveCameraForward);
    keyboard.editActions(GLFW_KEY_A, moveCameraLeft);
    keyboard.editActions(GLFW_KEY_S, moveCameraBackward);
    keyboard.editActions(GLFW_KEY_D, moveCameraRight);
    keyboard.editActions(GLFW_KEY_SPACE, moveCameraUp);
    keyboard.editActions(GLFW_KEY_LEFT_SHIFT, moveCameraDown);
    keyboard.editActions({GLFW_KEY_LEFT_CONTROL, GLFW_PRESS}, moveFaster, false);
    keyboard.editActions({GLFW_KEY_LEFT_CONTROL, GLFW_RELEASE}, moveSlower, false);
    keyboard.editActions(GLFW_KEY_HOME, resetAll, false);
    while (renderEngine.update()) {
        glfwPollEvents();
        keyboard.handleQueue();
    }
}