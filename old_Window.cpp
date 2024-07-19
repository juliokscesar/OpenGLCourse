#include "Window.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <stb/stb_image.h>

#include "Input.hpp"
#include "StaticMesh.hpp"
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "UIHelper.hpp"
#include "ModelFactory.hpp"
#include "glm/ext/matrix_transform.hpp"


static bool g_bResized = false;
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_bResized = true;
}

void Window::Init()
{
    // glfw initialization and setup
    glfwInit();

    // set opengl version to 3.3 and core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // set to focus when window opens
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_glfwWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Set current context to this window
    glfwMakeContextCurrent(m_glfwWindow);

    // Disable vsync
    glfwSwapInterval(0);

    // initialzie GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // set viewport size
    glViewport(0, 0, m_width, m_height);
    // set callback for viewport resize
    glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizeCallback);

    // hide cursor and capture it (to capture mouse movement)
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Init mouse and keyboard callbacks
    Input::RegisterCallbacks(m_glfwWindow);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initiate ImGui
    UIHelper::Init(m_glfwWindow);

    stbi_set_flip_vertically_on_load(true);
}

void Window::MainLoop()
{
    Camera camera{};
    
    glm::mat4 projection(1.0f);
    float pNear =  10.0f;
    float pFar  = 100.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // start dear imgui frame
        UIHelper::NewFrame();

        UIHelper::FrameStatsWindow(deltaTime);

        if (g_bResized)
            this->updateWindowProperties();

        if (Input::GetKeyState(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(m_glfwWindow, true);
        }
        
        // TODO: make this 'check once' better
        // Toggle mouse cursor capture and thus camera mouse movement 
        static bool tabChanged = false;
        if (Input::GetKeyState(GLFW_KEY_TAB) && !tabChanged)
        {
            int cursor = glfwGetInputMode(m_glfwWindow, GLFW_CURSOR);
            int mode = (cursor == GLFW_CURSOR_DISABLED) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
            glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, mode);

            tabChanged = true;
        }
        else if (!Input::GetKeyState(GLFW_KEY_TAB) && tabChanged)
        {
            tabChanged = false;
        }

        static bool pChanged = false;
        if (Input::GetKeyState(GLFW_KEY_P) && !pChanged)
        {
            int polygonMode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygonMode);
            if (polygonMode[1] == GL_FILL)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            pChanged = true;
        }
        else if (!Input::GetKeyState(GLFW_KEY_P) && pChanged)
            pChanged = false;


        camera.Update(deltaTime);

        projection = glm::perspective(
            glm::radians(camera.FOV),
            m_aspectRatio,
            near,
            far
        );


        // lighting 
        /* lightingShader.Use(); */
        /* //lightingShader.SetMat4("model", noTexCube.Transform.GetTransformMatrix()); */
        /* lightingShader.SetMat4("view", camera.GetLookAtMatrix()); */
        /* lightingShader.SetMat4("projection", projection); */

        /* lightingShader.SetBool("u_bEnableLighting", bEnableLighting); */
        /* lightingShader.SetBool("u_bUseDirectionalLight", bUseDirectionalLight); */
        /* lightingShader.SetBool("u_bUsePointLight", bUsePointLight); */
        /* lightingShader.SetBool("u_bUseSpotLight", bUseSpotLight); */
    
        /* // Directional light: */
        /* if (bUseDirectionalLight) */
        /* { */
        /*     lightingShader.SetVec3("u_dirLight.direction", dirLight.Direction); */

        /*     lightingShader.SetVec3("u_dirLight.ambient", dirLight.Ambient); */
        /*     lightingShader.SetVec3("u_dirLight.diffuse", dirLight.Diffuse); */
        /*     lightingShader.SetVec3("u_dirLight.specular", dirLight.Specular); */
        /* } */

        /* // Point light: */
        /* if (bUsePointLight) */
        /* { */
        /*     // update position */
        /*     pointLight.Position = lightSource.Transform.GetPosition(); */

        /*     lightingShader.SetVec3("u_pointLight.position", pointLight.Position); */
        /*     lightingShader.SetFloat("u_pointLight.attConstant", pointLight.Attenuation.Constant); */
        /*     lightingShader.SetFloat("u_pointLight.attLinear", pointLight.Attenuation.Linear); */
        /*     lightingShader.SetFloat("u_pointLight.attQuadratic", pointLight.Attenuation.Quadratic); */

        /*     lightingShader.SetVec3("u_pointLight.ambient", pointLight.Ambient); */
        /*     lightingShader.SetVec3("u_pointLight.diffuse", pointLight.Diffuse); */
        /*     lightingShader.SetVec3("u_pointLight.specular", pointLight.Specular); */
        /* } */

        /* // Spotlight */
        /* if (bUseSpotLight) */
        /* { */
        /*     // update position and direction */
        /*     spotLight.Position = camera.Transform.GetPosition(); */
        /*     spotLight.Direction = camera.GetFrontVector(); */

        /*     lightingShader.SetVec3("u_spotLight.position", spotLight.Position); */
        /*     lightingShader.SetVec3("u_spotLight.direction", spotLight.Direction); */
        /*     lightingShader.SetFloat("u_spotLight.innerCutoff", glm::radians(spotLight.InnerCutoff)); */
        /*     lightingShader.SetFloat("u_spotLight.outerCutoff", glm::radians(spotLight.OuterCutoff)); */
        /*     lightingShader.SetFloat("u_spotLight.attConstant", spotLight.Attenuation.Constant); */
        /*     lightingShader.SetFloat("u_spotLight.attLinear", spotLight.Attenuation.Linear); */
        /*     lightingShader.SetFloat("u_spotLight.attQuadratic", spotLight.Attenuation.Quadratic); */

        /*     lightingShader.SetVec3("u_spotLight.ambient", spotLight.Ambient); */
        /*     lightingShader.SetVec3("u_spotLight.diffuse", spotLight.Diffuse); */
        /*     lightingShader.SetVec3("u_spotLight.specular", spotLight.Specular); */
        /* } */

        /* lightingShader.SetVec3("u_viewPos", camera.Transform.GetPosition()); */
        
        /* // USING TEXTURE MATERIAL: */
        /* woodCrateTex.Activate(); */
        /* woodCrateSpecular.Activate(); */
        /* lightingShader.SetInt("u_material.diffuseMap", woodCrateMat.diffuseMapTex); */
        /* lightingShader.SetInt("u_material.specularMap", woodCrateMat.specularMapTex); */
        /* lightingShader.SetFloat("u_material.shininess", woodCrateMat.shininess); */

        /* for (size_t i = 0; i < 10; i++) */
        /* { */
        /*     float angle = 20.0f * i; */
        /*     glm::vec3 rot(-0.2f*angle, -1.0f*angle, -0.3f*angle); */
        /*     cubes[i].Transform.SetRotation(rot); */

        /*     lightingShader.SetMat4("model", cubes[i].Transform.GetTransformMatrix()); */
        /*     cubes[i].Draw(); */
        /* } */


        /* // light source cube (different shader) */
        /* lightSourceShader.Use(); */ 
        /* lightSourceShader.SetMat4("model", lightSource.Transform.GetTransformMatrix()); */
        /* lightSourceShader.SetMat4("view", camera.GetLookAtMatrix()); */
        /* lightSourceShader.SetMat4("projection", projection); */
        /* lightSourceShader.SetVec3("lightColor", lightDiffuse); */
        /* lightSource.Draw(); */


        /* // light orbiting around cube */
        /* if (bOrbitingCube) */
        /* { */
        /*     float radius = 10.0f; */
        /*     float time = static_cast<float>(glfwGetTime()); */
        /*     float light_x = radius * std::sin(time); */
        /*     float light_z = radius * std::cos(time); */
        /*     float light_y = 1.0f; */
        /*     lightSource.Transform.SetPosition(glm::vec3(light_x, light_y, light_z)); */
        /* } */

	modelShader.Use();
	modelShader.SetMat4("u_projection", projection);
	modelShader.SetMat4("u_view", camera.GetLookAtMatrix());

	glm::mat4 backpackTrans(1.0f);
	backpackTrans = glm::translate(backpackTrans, glm::vec3(0.0f, 0.0f, 0.0f));
	modelShader.SetMat4("u_model", backpackTrans);

	backpack.Draw(modelShader);

        UIHelper::Render();

        glfwSwapBuffers(m_glfwWindow);
        glfwPollEvents();
    }

    Terminate();
}

void Window::Terminate()
{
    UIHelper::Terminate();

    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void Window::updateWindowProperties()
{
    // i can't update the members width and height of the window in the callback
    // so this is an ugly turnaround
    int width = 0, height = 0;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    m_width = width, m_height = height;

    m_aspectRatio = (float)m_width / (float)m_height;

    g_bResized = false;
}
