#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "texture.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bufferRenderer.hpp"
#include "./IO/camera.hpp"
#include "./IO/input.hpp"


#include "lights.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./objects/cube.hpp"
#include "./objects/model.hpp"
#include "object.hpp"

#include "scene.hpp"
#include <memory> 

#include "renderer.hpp"

#include <iostream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Input* input);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera Defaults
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
bool firstMouse = true;

float dt;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool cursorEnabled = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    Renderer Render(SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial");
    GLFWwindow* window = Render.GetWindow();

    Render.SetInputProcessor([&Render](GLFWwindow* window) {
        processInput(window, Render.GetInput());
    });
    Render.SetMouseInputProcessor(mouse_callback);
    Render.SetScrollInputProcessor(scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    Shader shader("assets/shaders/shader.vs", "assets/shaders/shader.fs");
    Shader screenShader("assets/shaders/screen_shader.vs", "assets/shaders/screen_shader.fs");
    Shader lightCubeShader("assets/shaders/light_cube.vs", "assets/shaders/light_cube.fs");

    Texture tex("assets/textures/container2.png");
    Texture spec("assets/textures/container2_specular.png");
    Texture tex2("assets/textures/uv.png");

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    Render.InitFrameBuffer(Render.SCR_W, Render.SCR_H, &screenShader);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    BufferRenderer br;
    br.setVertices(quadVertices, sizeof(quadVertices)/sizeof(float));

    br.addAttrib(0, 2, GL_FLOAT);
    br.addAttrib(1, 2, GL_FLOAT);

    br.link();

    Scene scene;
    
    for (int i = 0; i < 10; i++) {
        auto cube = scene.NewInstance<Cube>();
        cube->position = cubePositions[i];
        cube->scale = glm::vec3(1.0f);
        cube->diffuse = tex;
        cube->specular = spec;

        scene.addObject("cube" + std::to_string(i), std::move(cube));
    }

    auto cube = scene.NewInstance<Cube>();
    cube->position = glm::vec3(-1.0f,-1.0f,-1.0f);
    cube->scale = glm::vec3(1.0f);
    cube->diffuse = tex2;

    scene.addObject("cube" + std::to_string(10), std::move(cube));

    auto model = scene.NewInstance<Model>("assets/models/backpack/backpack.obj");
    
    scene.addObject("model", std::move(model));
    
    
    // render loop
    // -----------
    while (Render.RenderLoop()) {
        dt = Render.GetDeltaTime();


        scene.view = camera.GetViewMatrix();
        scene.projection = glm::perspective(glm::radians(camera.Zoom), (float)Render.SCR_W / (float)Render.SCR_H, 0.1f, 100.0f);

        Render.BindFrameBuffer();

        shader.use();
        // directional light
        Lights::dirLight(shader, glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 0.95f, 0.8f), 0.3f);
        for (unsigned int i = 0; i < 4; i++) {
            Lights::pointLight(i, shader, pointLightPositions[i], glm::vec3(1.0f, 1.0f, 1.0f), 0.4f, 50.0f);
        }
        // spotLight
        Lights::spotLight(shader, camera.Position, camera.Front, glm::vec3(1.0f,1.0f,1.0f), 0.8f, 20.0f, 12.5f, 15.0f); 

        if (Object* obj = scene.getObject("model")) {
            obj->rotation = glm::vec3(0.0f, glfwGetTime() * 20, 0.0f);
        }
        ImGui::ShowDemoWindow();

        scene.render();

        Render.UnbindFrameBuffer();
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Render.GetFrameBufferTexture());
        br.draw();

        Render.RenderLast();
    }

    Render.Cleanup();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Input* input)
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    if (input->isKeyUp(GLFW_KEY_ESCAPE)) {
        //glfwSetWindowShouldClose(window, true);
        if (!cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
        }
        cursorEnabled = !cursorEnabled;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, dt);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, dt);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);
    if (ImGui::GetIO().WantCaptureMouse) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (cursorEnabled) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (cursorEnabled) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}