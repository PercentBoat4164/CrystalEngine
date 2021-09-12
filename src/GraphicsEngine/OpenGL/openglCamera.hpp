#pragma once

#include "openglSettings.hpp"
#include "openglGraphicsEngineLink.hpp"

#ifndef ILLUMINATION_ENGINE_PI
#define ILLUMINATION_ENGINE_PI 3.141592653589793238462643383279
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

/**
 * This class holds the OpenGLCamera methods
 * @class OpenGLCamera
 */
class OpenGLCamera {
public:
    /**
     * This method creates the a few variables that are used by the class.
     * @param engineLink This variable is the OpenGL render engine link.
     */
    void create(OpenGLGraphicsEngineLink *engineLink) {
        linkedRenderEngine = engineLink;
        updateSettings();
        proj = glm::perspective(glm::radians(horizontalFOV), aspectRatio, 0.01, linkedRenderEngine->settings->renderDistance);
    }

    /**
     * This method updates the OpenGL camera view.
     */
    void update() {
        front = glm::normalize(glm::vec3{cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch))});
        right = glm::normalize(glm::cross(front, up));
        view = glm::lookAt(position, position + front, up);
        proj = glm::perspective(glm::radians(horizontalFOV), aspectRatio, 0.01, linkedRenderEngine->settings->renderDistance);
        proj[1][1] *= 1;
    }

    /**
     * This method updates the OpenGL camera settings
     */
    void updateSettings() {
        aspectRatio = double(linkedRenderEngine->settings->resolution[0]) / linkedRenderEngine->settings->resolution[1];
        fov = linkedRenderEngine->settings->fov;
        horizontalFOV = tanh(tan(linkedRenderEngine->settings->fov*(ILLUMINATION_ENGINE_PI/360)) * 1 / aspectRatio) * (360 / ILLUMINATION_ENGINE_PI);
    };

    /** This is the OpenGL render engine variable.*/
    OpenGLGraphicsEngineLink *linkedRenderEngine{};
    /** This is a vector3 variable called position.*/
    glm::vec3 position{0, 0, 0};
    /** This is a vector3 variable called front.*/
    glm::vec3 front{0, 1, 0};
    /** This is a vector3 variable called up.*/
    glm::vec3 up{0, 0, 1};
    /** This is a vector3 variable called right.*/
    glm::vec3 right{glm::cross(front, up)};
    /** This is a float variable called yaw.*/
    float yaw{-90};
    /** This is a float variable called pitch.*/
    float pitch{};
    /** This is a double variable called aspectRatio.*/
    double aspectRatio{1};
    /** This is a double variable called fov.*/
    double fov{90};
    /** This is a double variable called horizontalFOV.*/
    double horizontalFOV{tanh(tan(fov*(ILLUMINATION_ENGINE_PI/360)) * 1 / aspectRatio) * (360 / ILLUMINATION_ENGINE_PI)};
    /** This is a matrix4 variable called view.*/
    glm::mat4 view{glm::lookAt(position, position + front, up)};
    /** This is a matrix4 variable called proj.*/
    glm::mat4 proj{};
};