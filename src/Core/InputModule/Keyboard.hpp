#pragma once

#include "Core/AssetModule/Aspect.hpp"

#ifndef GLEW_IMPLEMENTATION
#    define GLEW_IMPLEMENTATION
#    include <GL/glew.h>
#endif

#include <any>
#include <cstdint>
#include <functional>
#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace IE::Core::detail {
/**
 * @brief A class that stores the data related to a key press action.
 */
struct KeyPressDescription {
    uint16_t key;
    uint8_t  action;
    uint8_t  modifiers;
    uint16_t scancode;

    /**
     * @brief Constructs a KeyPressDescription from a key, action and modifiers.
     * @param initialKey
     * @param initialAction
     * @param initialModifiers=0
     * @returns IeKeyPressDescription
     */
    KeyPressDescription(int initialKey, int initialAction, int initialModifiers = 0);

    /**
     * Constructs a KeyPressDescription from a key. Sets action to pressed with no modifiers.
     * @param initialKey
     */
    explicit KeyPressDescription(int initialKey);

    /**
     * @brief The == operator for the IeKeyPressDescription structure.
     * @param other
     * @return true if the values of the object and argument are the same, false if not.
     */
    bool operator==(const IE::Core::detail::KeyPressDescription &other) const;
};
}  // namespace IE::Core::detail

/**
 * @brief The hash method for the IeKeyPressDescription structure.
 * @return A hash value for an IeKeyPressDescription.
 */
template<>
struct [[maybe_unused]] std::hash<IE::Core::detail::KeyPressDescription> {
    size_t operator()(const IE::Core::detail::KeyPressDescription &k) const;
};

namespace IE::Core {
/**
 * @brief The Keyboard class is intended to manage keyboard event handling.
 */
class Keyboard {
public:
    /**
     * @brief Adds the key callback function for this keyboard to this window.
     * @param t_window
     * @return None
     */
    void addWindow(GLFWwindow *t_window);

    /**
     * @brief Handles all the actions indicated by the key presses logged in the queue.
     */
    void handleQueue();

    /**
     * @brief Adds or changes a key press to function correlation.
     * @param keyPressDescription
     * @param action
     */
    void editActions(
      const IE::Core::detail::KeyPressDescription              &keyPressDescription,
      const std::pair<std::function<void(GLFWwindow *)>, bool> &action
    );

    /**
     * @brief Adds or changes key press to function correlation.
     * @param key
     * @param action
     */
    void editActions(uint16_t key, const std::pair<std::function<void(GLFWwindow *)>, bool> &action);

    /**
     * @brief Adds or changes key press to function correlation.
     * @param key
     * @param keyAction
     * @param modifiers
     * @param action
     */
    void editActions(
      uint16_t                                                  key,
      uint16_t                                                  keyAction,
      uint16_t                                                  modifiers,
      const std::pair<std::function<void(GLFWwindow *)>, bool> &action
    );

    /**
     * @brief Adds or changes a key press to function correlation.
     * @param keyPressDescription
     * @param action
     * @param repeat
     */
    void editActions(
      const IE::Core::detail::KeyPressDescription &keyPressDescription,
      const std::function<void(GLFWwindow *)>     &action
    );

    /**
     * @brief Adds or changes key press to function correlation.
     * @param key
     * @param action
     * @param repeat
     */
    void editActions(uint16_t key, const std::function<void(GLFWwindow *)> &action, bool repeat = true);

    /**
     * @brief Adds or changes key press to function correlation.
     * @param key
     * @param keyAction
     * @param modifiers
     * @param action
     * @param repeat
     */
    void editActions(
      uint16_t                                 key,
      uint16_t                                 keyAction,
      uint16_t                                 modifiers,
      const std::function<void(GLFWwindow *)> &action,
      bool                                     repeat = true
    );

    /**
     * @brief Clears the event queue.
     */
    void clearQueue();

    /**
     * @brief Default key event handler function. Enqueues the key as pressed or released. Does not handle repeats.
     * @param window
     * @param key
     * @param scancode
     * @param action
     * @param modifiers
     */
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int modifiers);

private:
    GLFWwindow                                        *window;   // window this keyboard manages
    std::vector<IE::Core::detail::KeyPressDescription> queue{};  // queue of key presses
    std::unordered_map<IE::Core::detail::KeyPressDescription, std::pair<std::function<void(GLFWwindow *)>, bool>>
      actionsOptions{};                                          // hash table of key press description to function
};
}  // namespace IE::Core