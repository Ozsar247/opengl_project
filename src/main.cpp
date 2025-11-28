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
#include "./editor/projectLoader.hpp"

#include "./editor/scriptEditor.hpp"

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

    Shader shader("assets/shaders/shader.vs", "assets/shaders/shader.fs");
    Shader screenShader("assets/shaders/screen_shader.vs", "assets/shaders/screen_shader.fs");
    Shader lightCubeShader("assets/shaders/light_cube.vs", "assets/shaders/light_cube.fs");
    Shader skyboxShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
    Shader reflectShader("assets/shaders/reflect.vs", "assets/shaders/reflect.fs");

    //Texture tex("assets/textures/container2.png");
    //Texture spec("assets/textures/container2_specular.png");

    ProjectLoader project("test_project/test_project.xml");

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

    std::vector<std::string> faces{
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };

    Cubemap skybox(skyboxShader, faces, "skybox");

    Scene scene = project.loadSceneFromXML(&Render);
    scene.camera = &camera;

    
    
    // for (int i = 0; i < 10; i++) {
    //     auto cube = scene.NewInstance<Cube>();
    //     cube->position = cubePositions[i];
    //     cube->scale = glm::vec3(1.0f);
    //     cube->diffuse = &tex;
    //     cube->specular = &spec;

    //     scene.addObject("cube" + std::to_string(i), std::move(cube));
    // }

    // auto cube = scene.NewInstance<Cube>();
    // cube->position = glm::vec3(-1.0f,-1.0f,-1.0f);
    // cube->scale = glm::vec3(1.0f);
    // cube->shader = &reflectShader;

    skybox.BindTex(reflectShader, "skybox", 0);

    // scene.addObject("cube" + std::to_string(10), std::move(cube));
    

    // auto model = scene.NewInstance<Model>(project.GetAbsoluteProjectPath() / "bin/models/backpack/backpack.obj");
  
    // scene.addObject("model", std::move(model));
    
    // render loop
    // -----------

    while (Render.RenderLoop()) {

        int sx;
        int sy;
        MainEditorWindows::ViewportRenderer(sx, sy, buffer.Texture(), camera, Render.GetWindow(), scene);
        Renderer::Viewport(Render.GetWindow(), sx, sy);

        dt = Render.GetDeltaTime();
        Render.GetGlobalShader(1)->use();
        Render.GetGlobalShader(1)->setVec3("cameraPos", camera.Position);

        buffer.BindFrameBuffer();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render.GetGlobalShader(0)->use();
        // directional light
        Lights::dirLight(shader, glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 0.95f, 0.8f), 0.3f);
        for (unsigned int i = 0; i < 4; i++) {
            Lights::pointLight(i, shader, pointLightPositions[i], glm::vec3(1.0f, 1.0f, 1.0f), 0.4f, 50.0f);
        }
        // spotLight
        Lights::spotLight(shader, camera.Position, camera.Front, glm::vec3(1.0f,1.0f,1.0f), 0.8f, 20.0f, 12.5f, 15.0f); 

        ImGui::ShowDemoWindow();

        ImGui::Begin("Render");
        ImGui::Checkbox("Wireframe", &scene.wireframe);
        ImGui::End();

        project.RenderFileExplorer();

        ScriptEditors::RenderAll();

        MainEditorWindows::RenderObjectExplorer(scene.Objects());
        MainEditorWindows::TextureViewerRenderer();
        MainEditorWindows::PropertiesRenderer();
        
        scene.render();
        
        skybox.Draw(camera);

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