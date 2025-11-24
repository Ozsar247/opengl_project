#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <unordered_map>
#include <vector>

class Input {
public:
    Input(GLFWwindow* window)
        : m_window(window) {
        m_keys.fill(false);
        m_keysPrevious.fill(false);
        m_mouseButtons.fill(false);
        m_mouseButtonsPrevious.fill(false);
    }

    // Call this once per frame
    void update() {
        updateKeyboard();
        updateMouse();
        updateGamepads();
    }

    // -------------------- Keyboard --------------------
    bool isKeyDown(int key) const {
        return m_keys[key] && !m_keysPrevious[key];
    }

    bool isKeyHeld(int key) const {
        return m_keys[key];
    }

    bool isKeyUp(int key) const {
        return !m_keys[key] && m_keysPrevious[key];
    }

    // -------------------- Mouse --------------------
    bool isMouseButtonDown(int button) const {
        return m_mouseButtons[button] && !m_mouseButtonsPrevious[button];
    }

    bool isMouseButtonHeld(int button) const {
        return m_mouseButtons[button];
    }

    bool isMouseButtonUp(int button) const {
        return !m_mouseButtons[button] && m_mouseButtonsPrevious[button];
    }

    void getMousePosition(double& x, double& y) const {
        x = m_mouseX;
        y = m_mouseY;
    }

    void getMouseDelta(double& dx, double& dy) const {
        dx = m_mouseX - m_mouseLastX;
        dy = m_mouseY - m_mouseLastY;
    }

    // -------------------- Gamepad --------------------
    bool isGamepadConnected(int jid) const {
        return glfwJoystickPresent(jid) == GLFW_TRUE && glfwJoystickIsGamepad(jid);
    }

    bool isGamepadButtonHeld(int jid, int button) const {
        if (!isGamepadConnected(jid)) return false;
        return m_gamepadButtons.at(jid)[button];
    }

    bool isGamepadButtonDown(int jid, int button) const {
        if (!isGamepadConnected(jid)) return false;
        const auto& buttons = m_gamepadButtons.at(jid);
        const auto& prev = m_gamepadButtonsPrev.at(jid);
        return buttons[button] && !prev[button];
    }


    float getGamepadAxis(int jid, int axis) const {
        if (!isGamepadConnected(jid)) return 0.0f;
        int count;
        const float* axes = glfwGetJoystickAxes(jid, &count);
        if (axis >= count) return 0.0f;
        return axes[axis];
    }

private:
    GLFWwindow* m_window;

    // Keyboard state
    std::array<bool, GLFW_KEY_LAST> m_keys;
    std::array<bool, GLFW_KEY_LAST> m_keysPrevious;

    // Mouse state
    std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_mouseButtons;
    std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_mouseButtonsPrevious;
    double m_mouseX = 0.0, m_mouseY = 0.0;
    double m_mouseLastX = 0.0, m_mouseLastY = 0.0;

    // Gamepad state
    std::unordered_map<int, std::vector<bool>> m_gamepadButtons;
    std::unordered_map<int, std::vector<bool>> m_gamepadButtonsPrev;

    void updateKeyboard() {
        for (int key = 0; key < GLFW_KEY_LAST; ++key) {
            m_keysPrevious[key] = m_keys[key];
            m_keys[key] = (glfwGetKey(m_window, key) == GLFW_PRESS);
        }
    }

    void updateMouse() {
        m_mouseLastX = m_mouseX;
        m_mouseLastY = m_mouseY;
        glfwGetCursorPos(m_window, &m_mouseX, &m_mouseY);

        for (int button = 0; button < GLFW_MOUSE_BUTTON_LAST; ++button) {
            m_mouseButtonsPrevious[button] = m_mouseButtons[button];
            m_mouseButtons[button] = (glfwGetMouseButton(m_window, button) == GLFW_PRESS);
        }
    }

    void updateGamepads() {
        for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
            if (glfwJoystickIsGamepad(jid)) {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(jid, &state)) {
                    if (m_gamepadButtons.find(jid) == m_gamepadButtons.end()) {
                        m_gamepadButtons[jid] = std::vector<bool>(GLFW_GAMEPAD_BUTTON_LAST, false);
                        m_gamepadButtonsPrev[jid] = std::vector<bool>(GLFW_GAMEPAD_BUTTON_LAST, false);
                    }

                    for (int b = 0; b < GLFW_GAMEPAD_BUTTON_LAST; ++b) {
                        m_gamepadButtonsPrev[jid][b] = m_gamepadButtons[jid][b];
                        m_gamepadButtons[jid][b] = state.buttons[b] == GLFW_PRESS;
                    }
                }
            }
        }
    }
};
