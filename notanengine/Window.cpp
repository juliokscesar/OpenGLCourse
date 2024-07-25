#include "Window.hpp"

#include <vector>

#include <GLFW/glfw3.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <stb/stb_image.h>

#include "Shader.hpp"
#include "Input.hpp"
#include "Camera.hpp"
#include "StaticMesh.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"
#include "UIHelper.hpp"
#include "Light.hpp"

static bool g_bResized = false;
static struct {int newWidth; int newHeight; } g_updatedProperties;
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_bResized = true;
    
    g_updatedProperties.newWidth = width;
    g_updatedProperties.newHeight = height;
}


void updateAndDrawEntity(Entity& entity, const Shader& shader, float deltaTime, const Camera& camera, const glm::mat4& projection)
{
    entity.Update(deltaTime);
    if (!entity.IsVisible())
	return;

    shader.Use();

    shader.SetMat4("u_model", entity.Transform.GetTransformMatrix());
    shader.SetMat4("u_view", camera.GetLookAtMatrix());
    shader.SetMat4("u_projection", projection);

    for (auto& meshData : entity.GetMeshRef().GetSubMeshesRef())
    {
        shader.SetBool("u_useMaterial", meshData.UseMaterial);

        if (meshData.UseMaterial && meshData.Mat.DiffuseMaps.empty())
        {
            shader.SetBool("u_useMaterial", false);
            meshData.UseMaterial = false;
        }

        else if (meshData.UseMaterial)
            shader.SetMaterial("u_material", meshData.Mat);

        glBindVertexArray(meshData.VAO);
        glDrawElements(GL_TRIANGLES, meshData.NumIndices, GL_UNSIGNED_INT, 0);
    }
}


void updateAndDrawEntities(const EntityRenderMap& entities, float deltaTime, const Camera& camera, const glm::mat4& projection)
{
    for (auto& [name, tupleEntityShader] : entities)
    {
        Entity& entity = std::get<0>(tupleEntityShader);
        const Shader& shader = std::get<1>(tupleEntityShader);

        updateAndDrawEntity(entity, shader, deltaTime, camera, projection);
    }
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
    glDepthFunc(GL_LESS); // already set by default

    // Initiate ImGui
    UIHelper::Init(m_glfwWindow);

    // Get assets locations
    ResourceManager::InitializeLocations();

    stbi_set_flip_vertically_on_load(true);
}

void Window::MainLoop()
{


    Camera camera;
    camera.Transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    float pNear =  0.1f;
    float pFar  = 100.0f;
    glm::mat4 projection = glm::perspective(
	glm::radians(camera.FOV),
	m_aspectRatio,
	pNear,
	pFar
    );

    
    Shader basicShader = ResourceManager::LoadShader("shaders/basic_shader.vert", "shaders/basic_shader.frag");
    Shader lightingShader = ResourceManager::LoadShader("shaders/objfile_shaders/entity_lighting.vert", "shaders/objfile_shaders/entity_lighting.frag");


    stbi_set_flip_vertically_on_load(false);
    Entity sponza(ResourceManager::LoadModel("models/Sponza/sponza.obj").Mesh);
    sponza.Transform.Scale(0.1f);

    EntityRenderMap entitiesMap = {
        { "Sponza", { sponza, basicShader } },
    };
    


    DirectionalLight dirLight;
    dirLight.UniformName = "u_dirLight";
    dirLight.Direction = glm::vec3(-0.2f, -1.0f, 0.0f);
    dirLight.Ambient = glm::vec3(0.05f);
    dirLight.Diffuse = glm::vec3(0.4f);
    dirLight.Specular = glm::vec3(0.5f);

    SpotLight spotLight;
    spotLight.UniformName = "u_spotLight";
    spotLight.Position = camera.Transform.GetPosition();
    spotLight.Direction = camera.GetFrontVector();
    spotLight.Ambient = glm::vec3(0.0f);
    spotLight.Diffuse = glm::vec3(0.4f);
    spotLight.Specular = glm::vec3(0.4f);
    spotLight.Attenuation = { 1.0f, 0.09f, 0.032f };
    spotLight.InnerCutoff = 12.5f;
    spotLight.OuterCutoff = 17.5f;

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
	
        UIHelper::EntityPropertiesManager(entitiesMap);

        UIHelper::DirectionalLightPropertiesManager(dirLight);

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
            pNear,
            pFar
        );

        spotLight.Position = camera.Transform.GetPosition();
        spotLight.Direction = camera.GetFrontVector();
        
        lightingShader.SetVec3("u_viewPos", camera.Transform.GetPosition());

        lightingShader.SetBool("u_useDirectionalLight", true);
        dirLight.SetLightUniforms(lightingShader);


        updateAndDrawEntities(entitiesMap, deltaTime, camera, projection);


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
    // also, getting state variables is expensive, so we keep track of the new
    // widht and height through a unnamed struct g_updatedProperties
    m_width = g_updatedProperties.newWidth;
    m_height = g_updatedProperties.newHeight;

    m_aspectRatio = (float)m_width / (float)m_height;

    g_bResized = false;
}
