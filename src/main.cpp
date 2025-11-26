#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "texture.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bufferRenderer.hpp"
#include "./IO/input.hpp"


#include "lights.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./objects/cube.hpp"
#include "./objects/model.hpp"
#include "object.hpp"

#include "framebuffer.hpp"
#include "scene.hpp"
#include "cubemap.hpp"
#include <memory> 

#include "renderer.hpp"
#include "tinyfiledialogs/tinyfiledialogs.h"

#include "./editor/mainEditorWindows.hpp"

#include <iostream>

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
    Renderer Render(800, 600, "OpenGL Tutorial");
    GLFWwindow* window = Render.GetWindow();

    //Render.SetInputProcessor([&Render](GLFWwindow* window) {
    //    processInput(window, Render.GetInput());
    //});
    //Render.SetMouseInputProcessor(mouse_callback);
    //Render.SetScrollInputProcessor(scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    Shader shader("assets/shaders/shader.vs", "assets/shaders/shader.fs");
    Shader screenShader("assets/shaders/screen_shader.vs", "assets/shaders/screen_shader.fs");
    Shader lightCubeShader("assets/shaders/light_cube.vs", "assets/shaders/light_cube.fs");
    Shader skyboxShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
    Shader reflectShader("assets/shaders/reflect.vs", "assets/shaders/reflect.fs");

    Shader gridShader("assets/shaders/grid.vs", "assets/shaders/grid.fs");
    Shader instancedShader("assets/shaders/instanced.vs", "assets/shaders/instanced.fs");

    Texture tex("assets/textures/container2.png");
    Texture spec("assets/textures/container2_specular.png");

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

    Framebuffer buffer(0, Render.SCR_W, Render.SCR_H, &screenShader);
    Render.InsertFrameBuffer(&buffer);

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
    float qVertices[] = {
        -1000.0f, 0.0f, -1000.0f,
        1000.0f, 0.0f, -1000.0f,
        1000.0f, 0.0f,  1000.0f,
        -1000.0f, 0.0f,  1000.0f
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    BufferRenderer grid;
    grid.setVertices(qVertices, sizeof(qVertices)/sizeof(float));
    grid.setIndices(quadIndices, sizeof(quadIndices)/sizeof(int));

    grid.addAttrib(0, 3, GL_FLOAT, GL_FALSE); // Position

    grid.link();

    unsigned int amount = 100000;
    std::vector<glm::mat4> modelMatrices;
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 150.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices.push_back(model);
    }


    std::vector<std::string> faces{
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };

    Cubemap skybox(skyboxShader, faces, "skybox");

    Scene scene;
    scene.camera = &camera;
    
    for (int i = 0; i < 10; i++) {
        auto cube = scene.NewInstance<Cube>();
        cube->position = cubePositions[i];
        cube->scale = glm::vec3(1.0f);
        cube->diffuse = &tex;
        cube->specular = &spec;

        scene.addObject("cube" + std::to_string(i), std::move(cube));
    }

    auto cube = scene.NewInstance<Cube>();
    cube->position = glm::vec3(-1.0f,-1.0f,-1.0f);
    cube->scale = glm::vec3(1.0f);
    cube->shader = &reflectShader;

    skybox.BindTex(reflectShader, "skybox", 0);

    scene.addObject("cube" + std::to_string(10), std::move(cube));
    

    auto model = scene.NewInstance<Model>("assets/models/backpack/backpack.obj");

    
    scene.addObject("model", std::move(model));
    
    
    // render loop
    // -----------
    while (Render.RenderLoop()) {

        int sx;
        int sy;
        MainEditorWindows::ViewportRenderer(sx, sy, buffer.Texture(), camera, Render.GetWindow(), scene);
        Renderer::Viewport(Render.GetWindow(), sx, sy);

        dt = Render.GetDeltaTime();
        
        reflectShader.use();
        reflectShader.setVec3("cameraPos", camera.Position);

        buffer.BindFrameBuffer();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        ImGui::Begin("Render");
        ImGui::Checkbox("Wireframe", &scene.wireframe);
        ImGui::End();

        MainEditorWindows::RenderObjectExplorer(scene.Objects());
        MainEditorWindows::TextureViewerRenderer();
        MainEditorWindows::PropertiesRenderer();
        
        scene.render();
        
        skybox.Draw(camera);

        gridShader.use();
        gridShader.setMat4("view", camera.view);
        gridShader.setMat4("projection", camera.projection);
        gridShader.setVec3("cameraPos", camera.Position);
        gridShader.setFloat("gridSize", 1.0f);

        grid.draw();

        buffer.UnbindFrameBuffer();
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer.Texture());
        br.draw();

        Render.SetClearColor(0.1f,0.1f,0.1f);

        glClear(GL_COLOR_BUFFER_BIT);

        Render.RenderLast();
    }

    return 0;
}